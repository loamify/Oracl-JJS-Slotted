#include "callback.h" 
#pragma warning (disable : 4100)
#include "skCrypt.h"
#include "mousedefs.h"

#define WINDOWS_1803 17134
#define WINDOWS_1809 17763
#define WINDOWS_1903 18362
#define WINDOWS_1909 18363
#define WINDOWS_2004 19041
#define WINDOWS_20H2 19569
#define WINDOWS_21H1 20180
#define WINDOWS_22H2 19045

ULONGLONG process_base;

const DWORD GetUserDirectoryTableBaseOffset()
{
	RTL_OSVERSIONINFOW ver = { 0 };
	RtlGetVersion(&ver);
	switch (ver.dwBuildNumber)
	{
	case WINDOWS_1803:
		return 0x0278;
		break;
	case WINDOWS_1809:
		return 0x0278;
		break;
	case WINDOWS_1903:
		return 0x0280;
		break;
	case WINDOWS_1909:
		return 0x0280;
		break;
	case WINDOWS_2004:
		return 0x0388;
		break;
	case WINDOWS_20H2:
		return 0x0388;
		break;
	case WINDOWS_21H1:
		return 0x0388;
		break;
	case WINDOWS_22H2:
		return 0x0388;
		break;
	default:
		return 0x0388;
	}
}

NTSTATUS get_phys_addr(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead)
{
	MM_COPY_ADDRESS AddrToRead = { 0 };
	AddrToRead.PhysicalAddress.QuadPart = (LONGLONG)TargetAddress;
	return MmCopyMemory(lpBuffer, AddrToRead, Size, MM_COPY_MEMORY_PHYSICAL, BytesRead);
}

NTSTATUS WritePhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesWritten)
{
	if (!TargetAddress)
		return STATUS_UNSUCCESSFUL;

	PHYSICAL_ADDRESS AddrToWrite = { 0 };
	AddrToWrite.QuadPart = LONGLONG(TargetAddress);

	PVOID pmapped_mem = MmMapIoSpaceEx(AddrToWrite, Size, PAGE_READWRITE);

	if (!pmapped_mem)
		return STATUS_UNSUCCESSFUL;

	memcpy(pmapped_mem, lpBuffer, Size);

	*BytesWritten = Size;
	MmUnmapIoSpace(pmapped_mem, Size);
	return STATUS_SUCCESS;
}

typedef union _virt_addr_t
{
	void* value;
	struct
	{
		uintptr_t offset : 12;
		uintptr_t pt_index : 9;
		uintptr_t pd_index : 9;
		uintptr_t pdpt_index : 9;
		uintptr_t pml4_index : 9;
		uintptr_t reserved : 16;
	};
} virt_addr_t, * pvirt_addr_t;

const UINT64 obtain_cr3(const PEPROCESS pProcess);


const UINT64 GetKernelDirBase()
{
	PUCHAR process = (PUCHAR)PsGetCurrentProcess();
	ULONG_PTR cr3 = *(PULONG_PTR)(process + 0x28); //dirbase x64, 32bit is 0x18
	return cr3;
}


#define PAGE_OFFSET_SIZE 12
static const UINT64 PMASK = (~0xfull << 8) & 0xfffffffffull;



#define PHYSICAL_MAP_THRESHOLD(Address, TotalSize) ((PAGE_SIZE - ((ULONGLONG)PAGE_SIZE - (Address & 0xFFF) & 0xFFF) < (TotalSize)) ? (PAGE_SIZE - (Address & 0xFFF)) : (TotalSize))
#define PMASK                                      (~0xfull << 8) & 0xfffffffffull

#define PHY_ADDRESS_MASK                           0x000ffffffffff000ull
#define PHY_ADDRESS_MASK_1GB_PAGES                 0x000fffffc0000000ull
#define PHY_ADDRESS_MASK_2MB_PAGES                 0x000fffffffe00000ull
#define VADDR_ADDRESS_MASK_1GB_PAGES               0x000000003fffffffull
#define VADDR_ADDRESS_MASK_2MB_PAGES               0x00000000001fffffull
#define VADDR_ADDRESS_MASK_4KB_PAGES               0x0000000000000fffull
#define ENTRY_PRESENT_BIT                          1
#define ENTRY_PAGE_SIZE_BIT                        0x0000000000000080ull

FORCEINLINE NTSTATUS MiCopyPhysicalMemory(ULONGLONG PhysicalAddress, PVOID Buffer, SIZE_T NumberOfBytes, BOOLEAN DoWrite)
{
	NTSTATUS         Status;
	SIZE_T           TotalBytes, BytesCopied, BytesToCopy;
	PVOID            MapSection;
	PHYSICAL_ADDRESS Address;

	Status = STATUS_INFO_LENGTH_MISMATCH;
	TotalBytes = NumberOfBytes;
	BytesCopied = 0;
	BytesToCopy = 0;
	MapSection = NULL;

	while (TotalBytes)
	{
		Status = STATUS_INSUFFICIENT_RESOURCES;
		Address.QuadPart = PhysicalAddress + BytesCopied;
		BytesToCopy = PHYSICAL_MAP_THRESHOLD(Address.QuadPart, TotalBytes);
		MapSection = MmMapIoSpaceEx(Address, BytesToCopy, PAGE_READWRITE);

		if (MapSection)
		{
			switch (DoWrite)
			{
			case TRUE:
				RtlCopyMemory(MapSection, RtlOffsetToPointer(Buffer, BytesCopied), BytesToCopy);
				break;
			case FALSE:
				RtlCopyMemory(RtlOffsetToPointer(Buffer, BytesCopied), MapSection, BytesToCopy);
				break;
			}

			MmUnmapIoSpace(MapSection, BytesToCopy);
			Status = STATUS_SUCCESS;
			BytesCopied += BytesToCopy;
			TotalBytes -= BytesToCopy;
		}

		if (Status != STATUS_SUCCESS) break;
	}

	return Status;
}



FORCEINLINE ULONGLONG MiVirtualToPhysical(_In_ ULONGLONG DirectoryBase, _In_ ULONGLONG VirtualAddress)
{
	ULONGLONG       table, PhysicalAddress = 0, selector, entry = 0;
	LONG            r, shift;
	SIZE_T          NumberOfBytesCopied;
	MM_COPY_ADDRESS MmAddress;

	table = DirectoryBase & PHY_ADDRESS_MASK;

	for (r = 0; r < 4; r++)
	{
		shift = 39 - (r * 9);
		selector = (VirtualAddress >> shift) & 0x1ff;
		NumberOfBytesCopied = 0;
		MmAddress.PhysicalAddress.QuadPart = table + selector * 8;

		if (!NT_SUCCESS(MmCopyMemory(&entry, MmAddress, sizeof(ULONGLONG), MM_COPY_MEMORY_PHYSICAL, &NumberOfBytesCopied)))
		{
			return PhysicalAddress;
		}

		if (!(entry & ENTRY_PRESENT_BIT))
		{
			return PhysicalAddress;
		}

		table = entry & PHY_ADDRESS_MASK;
		if (entry & ENTRY_PAGE_SIZE_BIT)
		{
			if (r == 1)
			{
				table &= PHY_ADDRESS_MASK_1GB_PAGES;
				table += VirtualAddress & VADDR_ADDRESS_MASK_1GB_PAGES;
				PhysicalAddress = table;
				return PhysicalAddress;
			}

			if (r == 2)
			{
				table &= PHY_ADDRESS_MASK_2MB_PAGES;
				table += VirtualAddress & VADDR_ADDRESS_MASK_2MB_PAGES;
				PhysicalAddress = table;
				return PhysicalAddress;
			}
		}
	}

	table += VirtualAddress & VADDR_ADDRESS_MASK_4KB_PAGES;
	PhysicalAddress = table;
	return PhysicalAddress;
}

const UINT64 translate_linear_address(UINT64 directoryTableBase, UINT64 virtualAddress)
{
	directoryTableBase &= ~0xf;

	UINT64 pageOffset = virtualAddress & ~(~0ul << PAGE_OFFSET_SIZE);
	UINT64 pte = ((virtualAddress >> 12) & (0x1ffll));
	UINT64 pt = ((virtualAddress >> 21) & (0x1ffll));
	UINT64 pd = ((virtualAddress >> 30) & (0x1ffll));
	UINT64 pdp = ((virtualAddress >> 39) & (0x1ffll));

	SIZE_T readsize = 0;
	UINT64 pdpe = 0;
	get_phys_addr(PVOID(directoryTableBase + 8 * pdp), &pdpe, sizeof(pdpe), &readsize);
	if (~pdpe & 1)
		return 0;

	UINT64 pde = 0;
	get_phys_addr(PVOID((pdpe & PMASK) + 8 * pd), &pde, sizeof(pde), &readsize);
	if (~pde & 1)
		return 0;

	/* 1GB large page, use pde's 12-34 bits */
	if (pde & 0x80)
		return (pde & (~0ull << 42 >> 12)) + (virtualAddress & ~(~0ull << 30));

	UINT64 pteAddr = 0;
	get_phys_addr(PVOID((pde & PMASK) + 8 * pt), &pteAddr, sizeof(pteAddr), &readsize);
	if (~pteAddr & 1)
		return 0;

	/* 2MB large page */
	if (pteAddr & 0x80)
		return (pteAddr & PMASK) + (virtualAddress & ~(~0ull << 21));

	virtualAddress = 0;
	get_phys_addr(PVOID((pteAddr & PMASK) + 8 * pte), &virtualAddress, sizeof(virtualAddress), &readsize);
	virtualAddress &= PMASK;

	if (!virtualAddress)
		return 0;

	return virtualAddress + pageOffset;
}

namespace physical
{

	NTSTATUS WriteProcessMemory(int pid, uint64_t Address, uint64_t AllocatedBuffer, SIZE_T size, SIZE_T* written)
	{ 
		PEPROCESS pProcess = NULL;
		if (pid == 0) return STATUS_UNSUCCESSFUL;

		NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)pid, &pProcess);
		if (NtRet != STATUS_SUCCESS) return NtRet;

		ULONG_PTR process_dirbase = obtain_cr3(pProcess);
		ObDereferenceObject(pProcess); 
		SIZE_T CurOffset = 0;
		SIZE_T TotalSize = size;
		while (TotalSize)
		{
			uint64_t CurPhysAddr = MiVirtualToPhysical(process_dirbase, (ULONG64)Address + CurOffset);
			if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

			ULONG64 WriteSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
			SIZE_T BytesWritten = 0;
			NtRet = WritePhysicalAddress((PVOID)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), WriteSize, &BytesWritten);
			TotalSize -= BytesWritten;
			CurOffset += BytesWritten;
			if (NtRet != STATUS_SUCCESS) break;
			if (BytesWritten == 0) break;
		}

		*written = CurOffset;
		return NtRet;
	}
	NTSTATUS ReadProcessMemory(int pid, uint64_t Address, uint64_t AllocatedBuffer, SIZE_T size)
	{ 
		PEPROCESS pProcess = NULL;
		if (pid == 0) return STATUS_UNSUCCESSFUL;

		NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)pid, &pProcess);
		if (NtRet != STATUS_SUCCESS) return NtRet;

		//ULONG_PTR process_dirbase = obtain_cr3(pProcess); 
		ULONG_PTR process_dirbase = process_base;
		ObDereferenceObject(pProcess);

		SIZE_T CurOffset = 0;
		SIZE_T TotalSize = size;
		while (TotalSize)
		{

			uint64_t CurPhysAddr = MiVirtualToPhysical(process_dirbase, (ULONG64)Address + CurOffset);
			if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

			ULONG64 ReadSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
			SIZE_T BytesRead = 0;
			NtRet = get_phys_addr((PVOID)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), ReadSize, &BytesRead);
			TotalSize -= BytesRead;
			CurOffset += BytesRead;
			if (NtRet != STATUS_SUCCESS) break;
			if (BytesRead == 0) break;
		}

		return NtRet;
	}

	NTSTATUS pReadPhysicalAddress(unsigned long long TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead)
	{
		return get_phys_addr((PVOID)TargetAddress, lpBuffer, Size, BytesRead);
	}
}

PVOID UtilMemMem(PVOID SearchBase, SIZE_T SearchSize, const void* Pattern, SIZE_T PatternSize)
{ 
	const UCHAR* searchBase = static_cast<const UCHAR*>(SearchBase);
	const UCHAR* pattern = static_cast<const UCHAR*>(Pattern);

	for (SIZE_T i = 0; i <= SearchSize - PatternSize; ++i)
	{
		SIZE_T j = 0;
		for (; j < PatternSize; ++j)
		{
			if (searchBase[i + j] != pattern[j])
				break;
		}

		if (j == PatternSize)
			return const_cast<UCHAR*>(&searchBase[i]);
	}

	return nullptr;
}

void* g_mmonp_MmPfnDatabase;

static NTSTATUS MmonpInitializeMmPfnDatabase()
{ 
	struct MmPfnDatabaseSearchPattern
	{
		const UCHAR* bytes;
		SIZE_T bytes_size;
		bool hard_coded;
	};
	MmPfnDatabaseSearchPattern patterns;

	// Windows 10 x64 Build 14332+
	static const UCHAR kPatternWin10x64[] = {
		0x48, 0x8B, 0xC1,        // mov     rax, rcx
		0x48, 0xC1, 0xE8, 0x0C,  // shr     rax, 0Ch
		0x48, 0x8D, 0x14, 0x40,  // lea     rdx, [rax + rax * 2]
		0x48, 0x03, 0xD2,        // add     rdx, rdx
		0x48, 0xB8,              // mov     rax, 0FFFFFA8000000008h
	};

	patterns.bytes = kPatternWin10x64;
	patterns.bytes_size = sizeof(kPatternWin10x64);
	patterns.hard_coded = true;

	const auto p_MmGetVirtualForPhysical = reinterpret_cast<UCHAR*>( MmGetVirtualForPhysical );
	if (!p_MmGetVirtualForPhysical)
	{
		return STATUS_PROCEDURE_NOT_FOUND;
	}

	auto found = reinterpret_cast<UCHAR*>(UtilMemMem(p_MmGetVirtualForPhysical, 0x20, patterns.bytes, patterns.bytes_size));
	if (!found)
	{
		return STATUS_UNSUCCESSFUL;
	}

	found += patterns.bytes_size;
	if (patterns.hard_coded)
	{
		g_mmonp_MmPfnDatabase = *reinterpret_cast<void**>(found);
	}
	else
	{
		const auto mmpfn_address = *reinterpret_cast<ULONG_PTR*>(found);
		g_mmonp_MmPfnDatabase = *reinterpret_cast<void**>(mmpfn_address);
	}

	g_mmonp_MmPfnDatabase = PAGE_ALIGN(g_mmonp_MmPfnDatabase);

	return STATUS_SUCCESS;
}


uintptr_t BruteforceCR3(void* base)
{
	if (!NT_SUCCESS(MmonpInitializeMmPfnDatabase()))
		return 0;

	virt_addr_t virt_base{}; virt_base.value = base;

	size_t read{};

	auto ranges = MmGetPhysicalMemoryRanges();

	for (int i = 0;; i++) {

		auto elem = &ranges[i];

		if (!elem->BaseAddress.QuadPart || !elem->NumberOfBytes.QuadPart)
			break;

		uintptr_t current_phys_address = elem->BaseAddress.QuadPart;

		for (int j = 0; j < (elem->NumberOfBytes.QuadPart / 0x1000); j++, current_phys_address += 0x1000) {

			_MMPFN* pnfinfo = (_MMPFN*)((uintptr_t)g_mmonp_MmPfnDatabase + (current_phys_address >> 12) * sizeof(_MMPFN));

			if (pnfinfo->u4.PteFrame == (current_phys_address >> 12))
			{
				MMPTE pml4e{};
				if (!NT_SUCCESS(physical::pReadPhysicalAddress(current_phys_address + 8 * virt_base.pml4_index, &pml4e, 8, &read)))
					continue;

				if (!pml4e.u.Hard.Valid)
					continue;

				MMPTE pdpte{};
				if (!NT_SUCCESS(physical::pReadPhysicalAddress((pml4e.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pdpt_index, &pdpte, 8, &read)))
					continue;

				if (!pdpte.u.Hard.Valid)
					continue;

				MMPTE pde{};
				if (!NT_SUCCESS(physical::pReadPhysicalAddress((pdpte.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pd_index, &pde, 8, &read)))
					continue;

				if (!pde.u.Hard.Valid)
					continue;

				MMPTE pte{};
				if (!NT_SUCCESS(physical::pReadPhysicalAddress((pde.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pt_index, &pte, 8, &read)))
					continue;

				if (!pte.u.Hard.Valid)
					continue;
				DbgPrint("GOT ADDRESS!");
				return current_phys_address;
			}
		}
	}
	DbgPrint("FAILED TO GET ADDRESS!");
	return 0;
}
  
const UINT64 obtain_cr3(const PEPROCESS pProcess)
{
	return BruteforceCR3((void*)PsGetProcessSectionBaseAddress(pProcess));
}

uint64_t VirtualAddressToPhysicalAddress(void* VirtualAddress)
{
	return MmGetPhysicalAddress(VirtualAddress).QuadPart;
}

uint64_t PhysicalAddressToVirtualAddress(uint64_t PhysicalAddress)
{
	PHYSICAL_ADDRESS PhysicalAddr = { 0 };
	PhysicalAddr.QuadPart = PhysicalAddress;

	return reinterpret_cast<uint64_t>(MmGetVirtualForPhysical(PhysicalAddr));
}

void* get_system_information(SYSTEM_INFORMATION_CLASS information_class)
{
	unsigned long size = 32;
	char buffer[32];

	ZwQuerySystemInformation(information_class, buffer, size, &size);

	void* info = ExAllocatePoolZero(NonPagedPool, size, 'shit');
	if (!info)
		return nullptr;

	if (!NT_SUCCESS(ZwQuerySystemInformation(information_class, info, size, &size))) {
		ExFreePool(info);
		return nullptr;
	}

	return info;
}

uintptr_t get_kernel_module(const char* name)
{
	const auto to_lower = [](char* string) -> const char* {
		for (char* pointer = string; *pointer != '\0'; ++pointer) {
			*pointer = (char)(short)tolower(*pointer);
		}

		return string;
		};

	const PRTL_PROCESS_MODULES info = (PRTL_PROCESS_MODULES)get_system_information(SystemModuleInformation);

	if (!info)
		return NULL;

	for (size_t i = 0; i < info->NumberOfModules; ++i) {
		const auto& mod = info->Modules[i];

		if (strcmp(to_lower((char*)mod.FullPathName + mod.OffsetToFileName), name) == 0) {
			const void* address = mod.ImageBase;
			ExFreePool(info);
			return (uintptr_t)address;
		}
	}

	ExFreePool(info);
	return NULL;
}




char* __fastcall find_jmp_rcx_in_module(char* a1)
{
	PIMAGE_NT_HEADERS v2;
	unsigned int* v3;
	unsigned int* i;
	char* result; 
	char* v6; 

	v2 = RtlImageNtHeader(a1);
	v3 = (unsigned int*)((char*)&v2->OptionalHeader.Magic + v2->FileHeader.SizeOfOptionalHeader);
	for (i = &v3[10 * v2->FileHeader.NumberOfSections]; v3 < i; v3 += 10)
	{
		if (v3 && (v3[9] & 0x28000000) == 671088640)
		{
			result = &a1[v3[3]];
			v6 = &result[v3[4]];
			while (result < v6)
			{
				if (*(_WORD*)result == 0xE1FF)
					return result;
				++result;
			}
		}
	}
	return 0i64;
}
inline UNICODE_STRING key = { 0 };

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))


PVOID FindJmp(PVOID moduleBase) {
	CONST PIMAGE_NT_HEADERS ntHeader = RtlImageNtHeader(moduleBase);
	CONST PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(ntHeader);

	for (PIMAGE_SECTION_HEADER section = firstSection; section < firstSection + ntHeader->FileHeader.NumberOfSections; section++)
	{
		if (!section)
			continue;

		if (!(section->Characteristics & 0x20000000) || !(section->Characteristics & 0x08000000))
			continue;

		CONST UINT64 sectionStart = (UINT64)moduleBase + section->VirtualAddress;
		CONST UINT64 sectionSize = section->SizeOfRawData;

		for (UINT64 current = sectionStart; current < (sectionStart + sectionSize); current++)
		{
			if (*(USHORT*)current == 0xe1ff) // jmp ecx: FF E1
				return (PVOID)current;
		}
	}

	return (PVOID)NULL;
}

NTSTATUS callback::init()
{ 
	SPOOF_FUNC;
	auto nvraid_base = (char*)get_kernel_module("nvvhci.sys"); 
	if (!nvraid_base)
		return 0xC0000008;

	auto jmp_rcx = FindJmp(nvraid_base);
	if (jmp_rcx == NULL)
		return 0xC0000225;

	RtlInitUnicodeString(&key,  L"LastScanTime");
	return CmRegisterCallback((PEX_CALLBACK_FUNCTION)jmp_rcx, callback::registry, &cookie);
}



_Use_decl_annotations_
NTSTATUS
callback::registry(
    PVOID  CallbackContext,
    PVOID  Argument1,
    PVOID  Argument2
)
{
	//SPOOF_FUNC;
	//if (!mouse_obj.service_callback || !mouse_obj.mouse_device) { mouse::init_mouse(&mouse_obj); }
	//mouse::mouse_event(mouse_obj, 1000, 1000, 0);
    if (Argument1 != (PVOID)RegNtSetValueKey) // checks to make sure that its a key value change
        return STATUS_SUCCESS;

    REG_SET_VALUE_KEY_INFORMATION* preInfo = (REG_SET_VALUE_KEY_INFORMATION*)Argument2;
     
    if (RtlEqualUnicodeString(preInfo->ValueName, &key, TRUE) == 0) // checks if registry key name is equal to our key
        return STATUS_SUCCESS;
    
    KM_REQ* request = *(KM_REQ**)preInfo->Data;

    if (!request) return STATUS_SUCCESS;
	 
    // handle requests here

    if (request->request == REQUEST_PID)
    {
		request->PID = (uintptr_t)memory::get_process_id(request->processName);

    }

	if (request->request == REQUEST_CR3)
	{
		PEPROCESS process = NULL;
		PsLookupProcessByProcessId((HANDLE)request->PID, &process);
		if (!process)
		{
			return STATUS_UNSUCCESSFUL;
		}

		process_base = obtain_cr3(process);

		if (!process_base)
		{
			process_base = obtain_cr3(process);
		}

		ObDereferenceObject(process);
	}
	 
	if (request->request == REQUEST_READ)
	{  

		if (request->address >= 0x7FFFFFFFFFFF && request->address <= 0xFFFFFFFFFFFFFFF7 || request->address <= 0 || !request->address || !request->PID || !request->pBuffer || !request->size_of_buffer)
		{
			return STATUS_UNSUCCESSFUL;
		}

		if ((RtlOffsetToPointer(request->address, request->size_of_buffer) < (PCHAR)request->address) || (RtlOffsetToPointer(request->pBuffer, request->size_of_buffer) < (PCHAR)request->pBuffer) || (RtlOffsetToPointer(request->address, request->size_of_buffer) > (PCHAR)MM_HIGHEST_USER_ADDRESS)
			|| (RtlOffsetToPointer(request->pBuffer, request->size_of_buffer) > (PCHAR)MM_HIGHEST_USER_ADDRESS) || (ULONGLONG)request->address <= 10000 || (ULONGLONG)request->pBuffer <= 10000)
		{
			return STATUS_UNSUCCESSFUL;
		}

		if (!MmIsAddressValid((PVOID)request->address))
		{
			return STATUS_UNSUCCESSFUL;
		}


		SIZE_T this_offset = NULL;
		SIZE_T total_size = request->size_of_buffer; 
		PVOID virtualaddress = (PVOID)request->address;

				//MmIsAddressValid((PVOID)request->address + this_offset);
		INT64 physical_address = MiVirtualToPhysical(process_base, (ULONG64)request->address + this_offset);
		if (!physical_address)
			return STATUS_UNSUCCESSFUL;

		ULONG64 final_size = min(PAGE_SIZE - (physical_address & 0xFFF), total_size);
		SIZE_T submitted_bytes = 0;
		NTSTATUS NtRet = get_phys_addr(PVOID(physical_address), (PVOID)((ULONG64)request->pBuffer + this_offset), final_size, &submitted_bytes);


		//SIZE_T this_offset = NULL;
		//SIZE_T total_size = request->size_of_buffer;
		//INT64 physical_address = translate_linear_address(process_base, (ULONG64)request->address + this_offset);
		//if (!physical_address)
		//	return STATUS_UNSUCCESSFUL;
		//ULONG64 final_size = min(PAGE_SIZE - (physical_address & 0xFFF), total_size);
		//SIZE_T submitted_bytes = NULL;
		//get_phys_addr(PVOID(physical_address), (PVOID)((ULONG64)request->pBuffer + this_offset), final_size, &submitted_bytes);
		//if ((RtlOffsetToPointer(request->address, request->size_of_buffer) < (PCHAR)request->address) || (RtlOffsetToPointer(request->pBuffer, request->size_of_buffer) < (PCHAR)request->pBuffer) || (RtlOffsetToPointer(request->address, request->size_of_buffer) > (PCHAR)MM_HIGHEST_USER_ADDRESS)
		//	|| (RtlOffsetToPointer(request->pBuffer, request->size_of_buffer) > (PCHAR)MM_HIGHEST_USER_ADDRESS) || (ULONGLONG)request->address <= 10000 || (ULONGLONG)request->pBuffer <= 10000)
		//{
		//	return STATUS_UNSUCCESSFUL;
		//}


		//physical::ReadProcessMemory(request->PID, request->address, (uint64_t)request->pBuffer, request->size_of_buffer);

	}


	if (request->request == REQUEST_WRITE)
	{

		SIZE_T this_offset = NULL;
		SIZE_T total_size = request->size_of_buffer;
		INT64 physical_address = translate_linear_address(process_base, (ULONG64)request->address + this_offset);
		if (!physical_address)
			return STATUS_UNSUCCESSFUL;
		ULONG64 final_size = min(PAGE_SIZE - (physical_address & 0xFFF), total_size);
		SIZE_T submitted_bytes = NULL;
		WritePhysicalAddress((PVOID)physical_address, (PVOID)((ULONG64)request->pBuffer + this_offset), final_size, &submitted_bytes);
	}

	if (request->request == REQUEST_PROBE)
	{
		request->pBuffer = (PVOID)1;
	}
	
    return STATUS_SUCCESS;
}


