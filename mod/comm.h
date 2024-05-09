#pragma once 

#define Alpine_OverwritePtr RegSetValueExA 
enum requests
{
	REQUEST_BASE,
	REQUEST_PID,
	REQUEST_MODULEBASE,
	REQUEST_READ,
	REQUEST_WRITE
};

struct KM_REQ
{
	uintptr_t request;
	uintptr_t address;
	uintptr_t PID;
	const char* processName;
	const char* moduleName;
	void* pBuffer;
	uintptr_t size_of_buffer;
};

class driver
{
private:

	HKEY targetKey;
	bool anti_duplicate;

	KM_REQ instructions;

	void make_request( )
	{
		if (!anti_duplicate)
		{
			RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\IdentityCRL", 0, KEY_ALL_ACCESS, &targetKey); // opens a registry key
			anti_duplicate = true;
		}

		if (targetKey == NULL || targetKey == INVALID_HANDLE_VALUE) {
			return;
		}

		void* val = &instructions;

		//
		// nvidia abuse PoC
		// impl by Rerout3d
		//
		Alpine_OverwritePtr(targetKey,
			skCrypt("GlobalDeviceUpdateTime"), 0, REG_QWORD,
			reinterpret_cast<BYTE*>(&val), sizeof(uint64_t));
	}

public:
	uintptr_t base;
	uintptr_t pid;

	driver() {};

	// modify request values here to return different values from kernel

	uintptr_t procID(const char* process)
	{ 
		instructions.request = REQUEST_PID;
		instructions.processName = process;

		make_request( );

		pid = instructions.PID;
		return instructions.PID;
	}
	 

	template <typename T>
	inline T rpm(uint64_t address)
	{
		T buffer{ }; 
		instructions.request = REQUEST_READ;
		instructions.PID = pid;
		instructions.size_of_buffer = sizeof(T);
		instructions.address = (unsigned long long)address;
		instructions.pBuffer = &buffer;

		make_request( );

		return buffer;
	}

	//template <typename T>
	//inline T rpm(uint64_t address) {
	//	T buffer{ };
	//	r0->read_physical((PVOID)address, &buffer, sizeof(T));
	//	return buffer;
	//}

	bool get_fort_base()
	{
		return base = (uintptr_t)LoadLibraryA(skCrypt("C:\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe"));
	}
};
inline driver* r0 = new driver;
