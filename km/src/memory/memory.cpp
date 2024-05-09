#include "memory.h"

HANDLE memory::get_process_id(const char* process_name)
{
	ANSI_STRING AS = { 0 };
	UNICODE_STRING US = { 0 };

	RtlInitAnsiString(&AS, process_name);
	RtlAnsiStringToUnicodeString(&US, &AS, true); // converting to the type used by the process ID in SYSTEM_PROCESS_INFO struct

	ULONG buffer_size = 0;
	ZwQuerySystemInformation(SystemProcessInformation, NULL, NULL, &buffer_size);  // gets the size of the SYSTEM_PROCESS_INFO struct

	PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, buffer_size, 'PYXR'); // allocates pool memory to buffer
	if (!buffer)
	{
		print("Failed to allocate pool");
		return 0;
	}

	ZwQuerySystemInformation(SystemProcessInformation, buffer, buffer_size, NULL); // returns pointer to SYSTEM_PROCESS_INFO


	PSYSTEM_PROCESS_INFO process_info = reinterpret_cast<PSYSTEM_PROCESS_INFO>(buffer);

	if (!process_info)
	{
		print("process_info is null lol get fucked");
		ExFreePoolWithTag(buffer, 'PYXR');
		return 0;
	}

	while (process_info->NextEntryOffset) // loops through all the processes
	{
		if (!RtlCompareUnicodeString(&US, &process_info->ImageName, TRUE))
		{
			ExFreePoolWithTag(buffer, 'PYXR');
			return process_info->ProcessId;
		}

		process_info = (PSYSTEM_PROCESS_INFO)((BYTE*)process_info + process_info->NextEntryOffset); // sets it to the address of the next struct

	}

	RtlFreeUnicodeString(&US);
	RtlFreeAnsiString(&AS);
	ExFreePoolWithTag(buffer, 'PYXR');
	return 0;
}

