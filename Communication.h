#pragma once 

#include <wtypes.h>
#include <stdint.h>
#include <sddl.h>
#include <iostream>
#include <Psapi.h> // To get process info
#include <TlHelp32.h>
#define Alpine_OverwritePtr RegSetValueExA 


inline std::wstring GetCurrentUserSID() {
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		std::wcerr << L"OpenProcessToken failed: " << GetLastError() << std::endl;
		return L"";
	}

	DWORD dwTokenUserSize = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwTokenUserSize);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		std::wcerr << L"GetTokenInformation (1st call) failed: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		return L"";
	}

	TOKEN_USER* pTokenUser = (TOKEN_USER*)malloc(dwTokenUserSize);
	if (!pTokenUser) {
		std::wcerr << L"Memory allocation error" << std::endl;
		CloseHandle(hToken);
		return L"";
	}

	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwTokenUserSize, &dwTokenUserSize)) {
		std::wcerr << L"GetTokenInformation (2nd call) failed: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		free(pTokenUser);
		return L"";
	}

	LPWSTR sidString = nullptr;
	if (ConvertSidToStringSidW(pTokenUser->User.Sid, &sidString)) {
		std::wstring sid(sidString);
		LocalFree(sidString);
		CloseHandle(hToken);
		free(pTokenUser);
		return sid;
	}
	else {
		std::wcerr << L"ConvertSidToStringSidW failed: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		free(pTokenUser);
		return L"";
	}
}

enum requests
{
	REQUEST_BASE,
	REQUEST_PID,
	REQUEST_MODULEBASE,
	REQUEST_READ,
	REQUEST_WRITE,
	REQUEST_CR3,
	REQUEST_PROBE
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
	bool anti_overflow;

	KM_REQ instructions;

	void make_request()
	{
			void* val = &instructions;

			//
			// nvidia abuse PoC
			// impl by Rerout3d
			//
			Alpine_OverwritePtr(targetKey,
				("LastScanTime"), 0, REG_QWORD,
				reinterpret_cast<BYTE*>(&val), sizeof(uint64_t));	
	}

public:
	uintptr_t base;
	uintptr_t pid;

	driver() {};

	// modify request values here to return different values from kernel

	bool InitComm()
	{
		RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\MemoryDiagnostic", 0, KEY_ALL_ACCESS, &targetKey); // opens a registry key
		anti_duplicate = true;


		return true;
	}

	uintptr_t procID(const char* process)
	{
		instructions.request = REQUEST_PID;
		instructions.processName = process;

		make_request();

		pid = instructions.PID;
		return instructions.PID;
	}

	void FetchCR3(const char* process)
	{
		instructions.request = REQUEST_CR3;
		instructions.processName = process;

		make_request();
	}

	static bool is_bad_ptr(uint64_t ptr)
	{
		constexpr auto min = 0x0001000;
		constexpr auto max = 0x7FFFFFFEFFFF;
		bool result = (ptr < min || ptr > max);
		return result;
	}

	bool is_valid(const uint64_t adress)
	{
		if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
			0x7FFFFFFFFFFFFFFF)
		{
			//std::cout << "BAD ADDRESS!" << std::endl;
			return false;
		}

		return true;
	}

	template <typename T>
	inline T rpm(uint64_t address)
	{
			T buffer{};
			size_t read = NULL;

			if (is_valid(address) && !is_bad_ptr(address))
			{
				instructions.request = REQUEST_READ;
				instructions.PID = pid;
				instructions.size_of_buffer = sizeof(T);
				instructions.address = (unsigned long long)address;
				instructions.pBuffer = &buffer;

				make_request();
			}
			return buffer;

	}


	template <typename T>
	inline bool read_array(uint64_t address, T* buffer, size_t size)
	{

		if (is_valid(address))
		{
			instructions.request = REQUEST_READ;
			instructions.PID = pid;
			instructions.size_of_buffer = sizeof(T) * size;
			instructions.address = static_cast<unsigned long long>(address);
			instructions.pBuffer = buffer;

			make_request();

			// Check if the read was successful
			return true;
		}
		else
			return false;
	}




	template <typename T>
	inline void wpm(uint64_t address, T buffer)
	{
		instructions.request = REQUEST_WRITE;
		instructions.PID = pid;
		instructions.size_of_buffer = sizeof(T);
		instructions.address = (unsigned long long)address;
		instructions.pBuffer = &buffer;

		make_request();
	}


	template<typename T>
	inline bool read_array_deprecated(uintptr_t address, T out[], size_t len)

	{
		for (size_t i = 0; i < len; ++i)
		{
			out[i] = rpm<T>(address + i * sizeof(T));
		}

		//T buffer{ };
		//instructions.request = REQUEST_READ;
		//instructions.PID = pid;
		//instructions.size_of_buffer = len;
		//instructions.address = (unsigned long long)address;
		//instructions.pBuffer = &buffer;
		//out = buffer;

		make_request();

		return true; // you can add additional checks to verify successful reads
	}



	inline bool init()
	{
		instructions.request = REQUEST_PROBE;
		instructions.PID = pid;
		//instructions.pBuffer = &buffer;

		make_request();

		return instructions.pBuffer;
	}


	//template<typename T>
	//inline bool read_array3(uint64_t address, T out[], size_t len)
	//{


	//	//T buffer{ };
	//	//instructions.request = REQUEST_READ;
	//	//instructions.PID = pid;
	//	//instructions.size_of_buffer = sizeof(T);
	//	//instructions.address = (unsigned long long)(address, (uint8_t*)out, sizeof(T) * len);
	//	//instructions.pBuffer = &buffer;

	//	//make_request();

	//	//return buffer;

	//	return (r0->rpm(address), r0->rpm((uint8_t*)out), r0->rpm(sizeof(T) * len));
	//}
	//template <typename T>
	//inline T rpm(uint64_t address) {
	//	T buffer{ };
	//	r0->read_physical((PVOID)address, &buffer, sizeof(T));
	//	return buffer;
	//}




	bool get_fort_base(HWND hwnd, LPARAM lParam)
	{
		char windowTitle[256];
		GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

		// Check if the window title matches the desired title
		if (strcmp(windowTitle, (const char*)lParam) == 0) {
			DWORD processId;
			GetWindowThreadProcessId(hwnd, &processId);

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
			if (hProcess != NULL) {
				char exePath[MAX_PATH];
				if (GetModuleFileNameExA(hProcess, NULL, exePath, sizeof(exePath))) {
					//std::cout << "Executable file path: " << exePath << std::endl;
					return base = (uintptr_t)LoadLibraryA((exePath));
				}
				else {
					std::cerr << "Error getting file path." << std::endl;
				}

				CloseHandle(hProcess);
			}
			else {
				std::cerr << "Error opening process." << std::endl;
			}

			return FALSE; // Stop enumerating windows
		}
	}


	void initsystem()
	{

		if (!windowid == NULL) {

		}
		else
		{
			while (windowid == NULL)
			{
				MessageBoxA(NULL, ("Waiting For Fortnite."), ("Press OK."), MB_OKCANCEL);
				windowid = FindWindowA(0, ("Fortnite  "));
			}
		}

		procID(("FortniteClient-Win64-Shipping.exe"));
		Sleep(400);
		FetchCR3(("FortniteClient-Win64-Shipping.exe"));
		FetchCR3(("FortniteClient-Win64-Shipping.exe"));
		FetchCR3(("FortniteClient-Win64-Shipping.exe"));
		Sleep(1000);
		const char* targetWindowTitle = "Fortnite  "; // Replace with your target window title
		get_fort_base(windowid, (LPARAM)targetWindowTitle);
	}


};
inline driver* r0 = new driver;
