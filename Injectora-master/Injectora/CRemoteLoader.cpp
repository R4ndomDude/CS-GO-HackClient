#include "CRemoteLoader.h"

#include <DbgHelp.h>
#pragma comment (lib, "DbgHelp.lib")

//
// Pube lick functions
//
void CRemoteLoader::SetProcess(HANDLE hProcess, DWORD dwProcessId)
{
	HMODULE hNtDll = (HMODULE)Utils::GetLocalModuleHandle("ntdll.dll");
	fnNTQIP = (tNTQIP)Utils::GetProcAddress(hNtDll, "NtQueryInformationProcess");
	fnNTQSI = (tNTQSI)Utils::GetProcAddress(hNtDll, "NtQuerySystemInformation");

	m_hProcess = hProcess;
	m_dwProcessId = dwProcessId;

	m_bIs64bit = (Utils::GetProcessPlatform(m_hProcess) == Utils::ProcessPlatformX64) ? true : false;
}

HMODULE CRemoteLoader::LdrpLoadDllA(LPCCH Path)
{
	WCHAR WideCharModule[MAX_PATH] = { 0 };
	size_t charsConverted;
	mbstowcs_s(&charsConverted, WideCharModule, Path, MAX_PATH);
	return LdrpLoadDllW(WideCharModule);
}

HMODULE CRemoteLoader::LdrpLoadDllW(LPCWCH Path)
{
	if (Path == NULL)
	{
		DbgShout("[LdrpLoadDllW] Path is NULL");
		return NULL;
	}

	// Get Ntdll module and size
	ULONG NtDllSize = 0;
	static HMODULE hNtDll = (HMODULE)GetRemoteModuleHandleA("ntdll.dll", &NtDllSize);

	// Make new unicode string object
	static tRtlInitUnicodeString RtlInitUnicodeString = (tRtlInitUnicodeString)Utils::GetProcAddress(Utils::GetLocalModuleHandle("ntdll.dll"), "RtlInitUnicodeString");
	UNICODE_STRING ModuleFileName;
	RtlInitUnicodeString(&ModuleFileName, Path);

	// Make allocate UNICODE_STRING struct in target process for the module path
	string_alloc_t path;
	path.size = (ULONG)((wcslen(Path) * 2) + 1);
	path.ptr = CommitMemory((void*)Path, path.size);
	m_CurrentInvokeInfo.strings.push_back(path);

	// Flags
	ULONG DllCharacteristics = 0; // Maybe use flags later. Idk.

								  // Return value
	void* ReturnPointerValue = RemoteAllocateMemory(sizeof(size_t));

	switch (Utils::GetOperatingSystemType())
	{

	case Utils::Windows10:
	{
		unsigned char* sig = m_bIs64bit ? sigLdrpLoadDll_10_64 : sigLdrpLoadDll_10_32;
		int sigSize = m_bIs64bit ? sizeof(sigLdrpLoadDll_10_64) : sizeof(sigLdrpLoadDll_10_32);
		static FARPROC RemoteLdrpLoadDll = (FARPROC)Utils::FindPattern(m_hProcess, (size_t)hNtDll, NtDllSize, sig, sigSize);
		if (RemoteLdrpLoadDll == NULL)
		{
			DbgShout("[LdrpLoadDllW] Windows 10 LdrpLoadDll resolve failure");
			return NULL;
		}

		DbgShout("[LdrpLoadDllW] Windows 10 LdrpLoadDll = 0x%IX", RemoteLdrpLoadDll);

		DLL_PATH_STRUCT DllPath;
		memset(&DllPath, 0, sizeof(DLL_PATH_STRUCT));
		DllPath.dllPath = (wchar_t*)path.ptr;

		struct_alloc_t dst;
		dst.size = sizeof(DLL_PATH_STRUCT);
		dst.ptr = CommitMemory((void*)&DllPath, sizeof(DLL_PATH_STRUCT));
		m_CurrentInvokeInfo.structs.push_back(dst);

		if (m_bIs64bit)
		{
			//LdrpLoadDll_10(IN PUNICODE_STRING ModuleFile, IN _DLL_PATH_STRUCT* DstFile, IN ULONG DllCharacteristics, bool CallInit);
			//
			// Backup RCX, RDX, R8 and R9 on stack
			BeginCall64();

			PushUNICODEStringStructure(&ModuleFileName); // ModuleFile
			PushInt64((unsigned __int64)dst.ptr); // DstFile
			PushInt64(DllCharacteristics); // DllCharacteristics
			PushInt64(TRUE); // CallInit

			MoveOntoStack((unsigned __int64)ReturnPointerValue, 0x20); // ModuleHandle is located at rsp + 0x20

			PushCall(CCONV_WIN64, RemoteLdrpLoadDll);
			//
			// ModuleHandle is located in an unknown struct pointed to by ReturnPointerValue.
			// ReturnPointerValue + 0x30 is where the ModuleHandle pointer is.
			//
			// mov r13, ReturnPointerValue
			AddByteToBuffer(0x49);
			AddByteToBuffer(0xBD);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);
			// mov r13, [r13 + 0x0] > deref ptr
			AddByteToBuffer(0x4D);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x6D);
			AddByteToBuffer(0x00);
			// mov r13, [r13 + 0x30]
			AddByteToBuffer(0x4D);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x6D);
			AddByteToBuffer(0x30);
			// mov rax, r13
			AddByteToBuffer(0x4C);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xE8);
			// mov [ReturnPointerValue], rax
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xA3);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);

			// Restore RCX, RDX, R8 and R9 from stack and return
			EndCall64();
		}
		else
		{
			///////////////////////////////////////////////////////////////////////
			// PushUNICODEStringStructure not implemented correctly for 32 bit yet
			// So we'll just allocate the UNICODE_STRING struct here.
			///////////////////////////////////////////////////////////////////////
			string_alloc_t s;
			s.size = (ULONG)(ModuleFileName.MaximumLength * 2) + 1;
			s.ptr = CommitMemory(ModuleFileName.Buffer, s.size);
			if (s.ptr == NULL)
			{
				DbgShout("[LdrpLoadDllW] NULL Allocated UNICODE string pointer. Failed to commit memory. Aborting...");
				return NULL;
			}
			m_CurrentInvokeInfo.strings.push_back(s);
			// Create new UNICODE_STRING object to allocate
			UNICODE_STRING unicodeParam;
			unicodeParam.Buffer = (wchar_t*)s.ptr;
			unicodeParam.Length = ModuleFileName.Length;
			unicodeParam.MaximumLength = ModuleFileName.MaximumLength;
			// Allocate UNICODE_STRING in target process
			struct_alloc_t unicodeStructAlloc;
			unicodeStructAlloc.size = sizeof(UNICODE_STRING);
			unicodeStructAlloc.ptr = CommitMemory(&unicodeParam, sizeof(UNICODE_STRING));
			m_CurrentInvokeInfo.structs.push_back(unicodeStructAlloc);
			///////////////////////////////////////////////////////////////////////

			PushInt((unsigned int)unicodeStructAlloc.ptr); // ModuleFile in ECX
			PushInt((unsigned int)dst.ptr); // DstFile in EDX
			PushInt(DllCharacteristics); // DllCharacteristics
			PushInt(TRUE); // CallInit
						   // ModuleHandle is actually pushed onto the stack rather than being put at rsp + 0x20 like in x64
			PushInt((unsigned long)ReturnPointerValue);
			// Its a __fastcall 
			PushCall(CCONV_FASTCALL, RemoteLdrpLoadDll);

			//
			// Module Handle is located in [ReturnPointerValue] + 0x18.
			// EAX holds the NT_STATUS return value, so I use EDX instead.
			//
			// mov edx, [ReturnPointerValue]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x15);
			AddLongToBuffer((unsigned long)ReturnPointerValue);
			// mov edx, [edx+0x18]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x52);
			AddByteToBuffer(0x18);
			// mov [ReturnPointerValue], edx
			AddByteToBuffer(0x89);
			AddByteToBuffer(0x15);
			AddLongToBuffer((unsigned long)ReturnPointerValue);
			// xor edx, edx > Set EDX back to 0
			AddByteToBuffer(0x31);
			AddByteToBuffer(0xD2);
			// ret 4
			AddByteToBuffer(0xC2);
			AddByteToBuffer(0x04);
			AddByteToBuffer(0x00);
		}
	}
	break;

	case Utils::Windows7:
	{
		unsigned char* sig = m_bIs64bit ? sigLdrpLoadDll_7_64 : sigLdrpLoadDll_7_32;
		int sigSize = m_bIs64bit ? sizeof(sigLdrpLoadDll_7_64) : sizeof(sigLdrpLoadDll_7_32);
		FARPROC RemoteLdrpLoadDll = (FARPROC)Utils::FindPattern(m_hProcess, (size_t)hNtDll, NtDllSize, sig, sigSize);
		if (RemoteLdrpLoadDll == NULL)
		{
			DbgShout("[LdrpLoadDllW] Windows 7 LdrpLoadDll resolve failure");
			return NULL;
		}

		DbgShout("[LdrpLoadDllW] Windows 7 LdrpLoadDll = 0x%IX", RemoteLdrpLoadDll);

		if (m_bIs64bit)
		{
			//LdrpLoadDll_7(IN PUNICODE_STRING ModuleFile, IN UNICODE_STRING* ModulePath, IN ULONG DllCharacteristics, ULONGLONG Redirected, ULONGLONG ModuleReturnHandle);
			//
			// Backup RCX, RDX, R8 and R9 on stack
			BeginCall64();

			PushUNICODEStringStructure(&ModuleFileName); // ModuleFile
			PushInt64((unsigned __int64)path.ptr); // ModulePath
			PushInt64(DllCharacteristics); // DllCharacteristics
			PushInt64(TRUE); // CallInit

			MoveOntoStack((unsigned __int64)ReturnPointerValue, 0x20); // ModuleHandle is located at rsp + 0x20

			PushCall(CCONV_WIN64, RemoteLdrpLoadDll);
			// First 

			// ModuleHandle is located in an unknown struct pointed to by ReturnPointerValue.
			// ReturnPointerValue + 0x30 is where the ModuleHandle pointer is.
			//
			// mov r13, ReturnPointerValue
			AddByteToBuffer(0x49);
			AddByteToBuffer(0xBD);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);
			// mov r13, [r13 + 0x0] > deref ptr
			AddByteToBuffer(0x4D);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x6D);
			AddByteToBuffer(0x00);
			// mov r13, [r13 + 0x30]
			AddByteToBuffer(0x4D);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x6D);
			AddByteToBuffer(0x30);
			// mov rax, r13
			AddByteToBuffer(0x4C);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xE8);
			// mov [ReturnPointerValue], rax
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xA3);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);

			// Restore RCX, RDX, R8 and R9 from stack and return
			EndCall64();
		}
		else
		{
			// TODO: finish reversing this

			//PushInt(NULL);
			//PushInt((unsigned int)flagsPtr);
			//PushUNICODEStringStructure(&unicodePath);
			//PushInt((unsigned long)ReturnPointerValue);
			//PushCall(CCONV_STDCALL, RemoteLdrLoadDll);
			//
			// Module Handle is located in unknown structure at [ebp+0x10] offset 0x18.
			//
			// mov eax, [ebp+0x10]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x45);
			AddByteToBuffer(0x10);
			// mov eax, [eax+0x18]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x40);
			AddByteToBuffer(0x18);
			// mov ptr, eax
			AddByteToBuffer(0xA3);
			AddLongToBuffer((unsigned long)ReturnPointerValue);
			// xor eax, eax
			AddByteToBuffer(0x33);
			AddByteToBuffer(0xC0);
			// ret 4
			AddByteToBuffer(0xC2);
			AddByteToBuffer(0x04);
			AddByteToBuffer(0x00);
		}
	}
	break;

	default:
		MessageBox(NULL, "Unsupported OS for LdrpLoadDll stub!", "Injectora", MB_ICONERROR);
		break;
	}

	if (!ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer, true))
	{
		DbgShout("[LdrpLoadDllW] ExecuteRemoteThreadBuffer failed");
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		return NULL;
	}

	DbgShout("[LdrpLoadDllW] ExecuteRemoteThreadBuffer succeeded");

	HMODULE RemoteModuleHandle = 0;
	if (ReadProcessMemory(m_hProcess, ReturnPointerValue, &RemoteModuleHandle, sizeof(HMODULE), NULL))
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
	}
	else
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		if (RemoteModuleHandle == 0)
			RemoteModuleHandle = GetRemoteModuleHandleW(ModuleFileName.Buffer);
	}

	return RemoteModuleHandle;
}

HMODULE CRemoteLoader::LdrLoadDllA(LPCCH Path)
{
	WCHAR Module[MAX_PATH] = { 0 };
	size_t charsConverted;
	mbstowcs_s(&charsConverted, Module, Path, MAX_PATH);
	return LdrLoadDllW(Module);
}

HMODULE CRemoteLoader::LdrLoadDllW(LPCWCH Path)
{
	if (Path == NULL)
	{
		DbgShout("[LdrLoadDllW] szString is NULL");
		return NULL;
	}

	FARPROC RemoteLdrLoadDll = (FARPROC)Utils::GetProcAddress(Utils::GetLocalModuleHandle("ntdll.dll"), "LdrLoadDll");//GetRemoteProcAddressA("ntdll.dll", "LdrLoadDll");
	if (RemoteLdrLoadDll == NULL)
	{
		DbgShout("[LdrLoadDllW] RemoteLdrLoadDll resolve failure");
		return NULL;
	}

	DbgShout("[LdrLoadDllW] LdrLoadDll = 0x%IX", RemoteLdrLoadDll);

	// Make new unicode string object
	static tRtlInitUnicodeString RtlInitUnicodeString = (tRtlInitUnicodeString)Utils::GetProcAddress(Utils::GetLocalModuleHandle("ntdll.dll"), "RtlInitUnicodeString");
	UNICODE_STRING unicodePath;
	RtlInitUnicodeString(&unicodePath, Path);

	ULONG Flags = NULL; // Maybe use flags later. Idk.
	void* flagsPtr = CommitMemory((void*)&Flags, sizeof(ULONG));

	void* ReturnPointerValue = RemoteAllocateMemory(sizeof(size_t));
	switch (Utils::GetOperatingSystemType())
	{
	case Utils::Windows10:
	{
		if (m_bIs64bit)
		{
			//LdrLoadDll(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle );
			// Backup RCX, RDX, R8 and R9 on stack
			BeginCall64();
			//
			PushInt64(NULL);
			PushInt64((unsigned __int64)flagsPtr);
			PushUNICODEStringStructure(&unicodePath);
			PushInt64((unsigned __int64)ReturnPointerValue);
			PushCall(CCONV_WIN64, RemoteLdrLoadDll);
			//
			// Module Handle is located in RDX and at QWORD PTR [ReturnPointerValue].
			// Could do 'mov rax, [ReturnPointerValue]' but it takes many more opcodes to do so.
			// We could also just RPM twice on ReturnPointerValue but it's better just to get it from rdx.
			//
			// mov rax, rdx  
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xD0);
			// mov [ReturnPointerValue], rax
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xA3);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);
			// Restore RCX, RDX, R8 and R9 from stack and return
			EndCall64();
		}
		else
		{
			PushInt(NULL);
			PushInt((unsigned int)flagsPtr);
			PushUNICODEStringStructure(&unicodePath);
			PushInt((unsigned long)ReturnPointerValue);
			PushCall(CCONV_STDCALL, RemoteLdrLoadDll);
			//
			// Module Handle is located in [EDX] -> Only in Windows 10!
			// To avoid calling RPM twice, we pass the [edx] into eax instead of just edx.
			//
			// mov    eax,DWORD PTR [edx]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x02);
			// mov ptr, eax
			AddByteToBuffer(0xA3);
			AddLongToBuffer((unsigned long)ReturnPointerValue);
			// xor eax, eax
			AddByteToBuffer(0x33);
			AddByteToBuffer(0xC0);
			// ret 4
			AddByteToBuffer(0xC2);
			AddByteToBuffer(0x04);
			AddByteToBuffer(0x00);
		}
	}
	break;

	case Utils::Windows7:
	{
		if (m_bIs64bit)
		{
			//LdrLoadDll(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle );
			// Backup RCX, RDX, R8 and R9 on stack
			BeginCall64();
			//
			PushInt64(NULL);
			PushInt64((unsigned __int64)flagsPtr);
			PushUNICODEStringStructure(&unicodePath);
			PushInt64((unsigned __int64)ReturnPointerValue);
			PushCall(CCONV_WIN64, RemoteLdrLoadDll);
			//
			// Module Handle is located in RDX and at QWORD PTR [ReturnPointerValue].
			// Could do 'mov rax, [ReturnPointerValue]' but it takes many more opcodes to do so.
			// We could also just RPM twice on ReturnPointerValue but it's better just to get it from rdx.
			//
			// mov rax, rdx  
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xD0);
			// mov [ReturnPointerValue], rax
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xA3);
			AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);
			// Restore RCX, RDX, R8 and R9 from stack and return
			EndCall64();
		}
		else
		{
			PushInt(NULL);
			PushInt((unsigned int)flagsPtr);
			PushUNICODEStringStructure(&unicodePath);
			PushInt((unsigned long)ReturnPointerValue);
			PushCall(CCONV_STDCALL, RemoteLdrLoadDll);
			//
			// Module Handle is located at [ReturnPointerValue] and [ecx]
			// This is probably uneccessary as its already located at [ReturnPointerValue]
			// 
			// mov eax, [ecx]
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x01);
			// mov [ReturnPointerValue], eax
			AddByteToBuffer(0xA3);
			AddLongToBuffer((unsigned long)ReturnPointerValue);
			// xor eax, eax
			AddByteToBuffer(0x33);
			AddByteToBuffer(0xC0);
			// ret 4
			AddByteToBuffer(0xC2);
			AddByteToBuffer(0x04);
			AddByteToBuffer(0x00);
		}
	}
	break;


	default:
		break;
	}

	if (!ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer, true))
	{
		DbgShout("[LdrLoadDllW] ExecuteRemoteThreadBuffer failed");
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		return NULL;
	}

	DbgShout("[LdrLoadDllW] ExecuteRemoteThreadBuffer succeeded");

	HMODULE RemoteModuleHandle = 0;
	if (ReadProcessMemory(m_hProcess, ReturnPointerValue, &RemoteModuleHandle, sizeof(HMODULE), NULL))
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
	}
	else
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		if (RemoteModuleHandle == 0)
			RemoteModuleHandle = GetRemoteModuleHandleW(Path);
	}

	return RemoteModuleHandle;
}

HMODULE CRemoteLoader::LoadLibraryByPathA(LPCCH Path, ULONG Flags/*= NULL*/)
{
	WCHAR Module[MAX_PATH] = { 0 };
	size_t charsConverted;
	mbstowcs_s(&charsConverted, Module, Path, MAX_PATH);
	return LoadLibraryByPathW(Module, Flags);
}

HMODULE CRemoteLoader::LoadLibraryByPathW(LPCWCH Path, ULONG Flags/*= NULL*/)
{
	if (Path == NULL)
	{
		DbgShout("[LoadLibraryByPathW] szString is NULL");
		return NULL;
	}

	FARPROC RemoteLoadLibraryW = (FARPROC)Utils::GetProcAddress(Utils::GetLocalModuleHandle("kernel32.dll"), "LoadLibraryW"); //GetRemoteProcAddressA("ntdll.dll", "LdrLoadDll");
	if (RemoteLoadLibraryW == NULL)
	{
		DbgShout("[LoadLibraryByPathW] RemoteLdrLoadDll resolve failure");
		return NULL;
	}

	DbgShout("[LoadLibraryByPathW] LoadLibraryW = 0x%IX", RemoteLoadLibraryW);

	void* ReturnPointerValue = RemoteAllocateMemory(sizeof(size_t));

	if (m_bIs64bit)
	{
		// Backup RCX, RDX, R8 and R9 on stack
		BeginCall64();
		//
		PushUNICODEString(Path);
		PushCall(CCONV_WIN64, RemoteLoadLibraryW);
		// mov [ReturnPointerValue], rax
		AddByteToBuffer(0x48);
		AddByteToBuffer(0xA3);
		AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);
		// Restore RCX, RDX, R8 and R9 from stack and return
		EndCall64();
	}
	else
	{
		PushUNICODEString(Path);
		PushCall(CCONV_STDCALL, RemoteLoadLibraryW);
		// mov [ReturnPointerValue], eax
		AddByteToBuffer(0xA3);
		AddLongToBuffer((ULONG)ReturnPointerValue);
		// xor eax, eax
		AddByteToBuffer(0x33);
		AddByteToBuffer(0xC0);
		// retn 4
		AddByteToBuffer(0xC2);
		AddByteToBuffer(0x04);
		AddByteToBuffer(0x00);
	}

	if (ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer) == false)
	{
		DbgShout("[LoadLibraryByPathW] ExecuteRemoteThreadBuffer failed");
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		return NULL;
	}

	DbgShout("[LoadLibraryByPathW] ExecuteRemoteThreadBuffer succeeded");

	HMODULE RemoteModuleHandle = 0;
	if (ReadProcessMemory(m_hProcess, ReturnPointerValue, &RemoteModuleHandle, sizeof(HMODULE), NULL))
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
	}
	else
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		if (RemoteModuleHandle == 0)
			RemoteModuleHandle = GetRemoteModuleHandleW(Path);
	}

	return RemoteModuleHandle;
}

HMODULE CRemoteLoader::LoadLibraryByPathIntoMemoryW(LPCWCH Path, BOOL PEHeader)
{
	CHAR PathAnsi[MAX_PATH] = { 0 };
	size_t charsConverted;
	wcstombs_s(&charsConverted, PathAnsi, Path, MAX_PATH);
	return LoadLibraryByPathIntoMemoryA(PathAnsi, PEHeader);
}

HMODULE CRemoteLoader::LoadLibraryByPathIntoMemoryA(LPCCH Path, BOOL PEHeader)
{
	HMODULE hReturnValue = NULL;

	DbgShout("[LoadLibraryByPathIntoMemoryA] %s", Path);

	ModuleFile File = InitModuleFile(Path);
	if (File.IsValid() == FALSE)
	{
		DbgShout("[LoadLibraryByPathIntoMemoryA] Failed to open file handle!");
		//MessageBox(0, "Failed to open DLL file!", "Injectora", MB_ICONERROR);
		return NULL;
	}

	hReturnValue = LoadLibraryFromMemory(File.Buffer, File.Size, PEHeader);
	if (FreeModuleFile(File) == FALSE) {
		DbgShout("[LoadLibraryByPathIntoMemoryA] Failed to free file handle...");
	}

	return hReturnValue;
}

HMODULE CRemoteLoader::LoadLibraryFromMemory(PVOID BaseAddress, DWORD SizeOfModule, BOOL PEHeader)
{
	DbgShout("[LoadLibraryFromMemory] BaseAddress (0x%IX) - SizeOfModule (0x%X)", BaseAddress, SizeOfModule);

	IMAGE_NT_HEADERS* ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
	{
		DbgShout("[LoadLibraryFromMemory] Invalid Image: No IMAGE_NT_HEADERS");
		return NULL;
	}

	if (ImageNtHeaders->FileHeader.NumberOfSections == 0)
	{
		DbgShout("[LoadLibraryFromMemory] Invalid Image: No Sections");
		return NULL;
	}

	if (m_bIs64bit)
	{
		//
		// Create Remote Procedure Call environment. No need for this in 32 bit
		//
		DWORD err = CreateRPCEnvironment();
		if (err != ERROR_SUCCESS)
		{
			DbgShout("[LoadLibraryFromMemory] CreateRPCEnvironment failed. Error 0x%X", err);
			return NULL;
		}

		//
		// Create activation context for the module we're injecting. Not needed for x86 modules.
		//
		DbgShout("[LoadLibraryFromMemory] Creating Activation Context!");

		if (!CreateActx(BaseAddress)) {
			DbgShout("[LoadLibraryFromMemory] Failed to obtain embedded resource! Continuing anyway without Activation Context...");
		}
		else {
			DbgShout("[LoadLibraryFromMemory] Createed Activation Context successfully!");
		}
	}

	// We do not trust the value of hdr.OptionalHeader.SizeOfImage so we calculate our own SizeOfImage.
	// This is the size of the continuous memory block that can hold the headers and all sections.
	//
	size_t rva_low = (!PEHeader) ? ((size_t)-1) : 0;
	size_t rva_high = 0;
	PIMAGE_SECTION_HEADER ImageSectionHeader = (PIMAGE_SECTION_HEADER)((ULONG_PTR)&ImageNtHeaders->OptionalHeader + ImageNtHeaders->FileHeader.SizeOfOptionalHeader);
	for (size_t i = 0; i < ImageNtHeaders->FileHeader.NumberOfSections; ++i)
	{
		if (!ImageSectionHeader[i].Misc.VirtualSize)
			continue;
		if (ImageSectionHeader[i].VirtualAddress < rva_low)
			rva_low = ImageSectionHeader[i].VirtualAddress;
		if ((ImageSectionHeader[i].VirtualAddress + ImageSectionHeader[i].Misc.VirtualSize) > rva_high)
			rva_high = ImageSectionHeader[i].VirtualAddress + ImageSectionHeader[i].Misc.VirtualSize;
	}

	// Calculated Image Size
	//
	size_t ImageSize = rva_high - rva_low;

	DbgShout("[LoadLibraryFromMemory] Calculated size (0x%IX)", ImageSize);

	if ((ImageNtHeaders->OptionalHeader.ImageBase % 4096) != 0)
	{
		DbgShout("[LoadLibraryFromMemory] Invalid Image: Not Page Aligned");
		return NULL;
	}

	if (ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size && ::ImageDirectoryEntryToData(BaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR, NULL))
	{
		DbgShout("[LoadLibraryFromMemory] This method is not supported for Managed executables!");
		return NULL;
	}

	DbgShout("[LoadLibraryFromMemory] Good, no COM/CLR data found!");

	// SizeOfImage NOT the same as module size
	void* AllocatedRemoteMemory = RemoteAllocateMemory(ImageSize);
	if (AllocatedRemoteMemory == NULL)
	{
		DbgShout("[LoadLibraryFromMemory] Failed to allocate remote memory for module!");
		return NULL;
	}

	DbgShout("[LoadLibraryFromMemory] Allocated remote module at [0x%IX]!", AllocatedRemoteMemory);
	DbgShout("[LoadLibraryFromMemory] Processing Import Tables....\n");

	//
	// Process imports
	//
	if (ProcessImportTable(BaseAddress, AllocatedRemoteMemory) == FALSE)
	{
		DbgShout("[LoadLibraryFromMemory] Failed to fix imports!");
		return NULL;
	}

	//
	// Process delayed imports
	//
	if (ProcessDelayedImportTable(BaseAddress, AllocatedRemoteMemory) == FALSE)
	{
		DbgShout("[LoadLibraryFromMemory] Failed to fix delayed imports!");
		return NULL;
	}

	DbgShout("[LoadLibraryFromMemory] Fixed Imports!");
	DbgShout("[LoadLibraryFromMemory] Processing Relocations...");

	//
	// Process relocations
	//
	if (ProcessRelocations(BaseAddress, AllocatedRemoteMemory) == FALSE)
	{
		DbgShout("[LoadLibraryFromMemory] Failed to process relocations!");
		RemoteFreeMemory(AllocatedRemoteMemory, SizeOfModule);
		return NULL;
	}

	DbgShout("[LoadLibraryFromMemory] Fixed Relocations!");
	DbgShout("[LoadLibraryFromMemory] Processing Sections!");

	//
	// Process sections
	//
	if (ProcessSections(BaseAddress, AllocatedRemoteMemory, PEHeader) == FALSE) {
		DbgShout("[LoadLibraryFromMemory] Failed to process sections!");
	}

	//if (m_bIs64bit && PEHeader)
	//	EnableExceptions(BaseAddress, AllocatedRemoteMemory, ImageSize);

	DbgShout("[LoadLibraryFromMemory] Processed sections!");
	DbgShout("[LoadLibraryFromMemory] Processing TLS Callback Entries!");

	//
	// Process thread local storage
	//
	if (ProcessTlsEntries(BaseAddress, AllocatedRemoteMemory) == FALSE)
	{
		DbgShout("[LoadModuleFromMemory] ProcessTlsEntries Failed!");
		// we can also choose to continue here, but we wont cause unsafe
		return NULL;
	}

	DbgShout("[LoadModuleFromMemory] Processed Tls Entries!");

	// Security cookie if needed
	InitializeCookie(BaseAddress, AllocatedRemoteMemory);

	if (ImageNtHeaders->OptionalHeader.AddressOfEntryPoint)
	{
		FARPROC DllEntryPoint = MakePtr(FARPROC, AllocatedRemoteMemory, ImageNtHeaders->OptionalHeader.AddressOfEntryPoint);

		DbgShout("[LoadModuleFromMemory] DllEntrypoint = 0x%IX", DllEntryPoint);

		if (CallEntryPoint(AllocatedRemoteMemory, DllEntryPoint) == false) {
			DbgShout("[LoadModuleFromMemory] Failed to execute remote thread buffer");
		}
		else {
			DbgShout("[LoadModuleFromMemory] Executed the remote thread buffer successfully [0x%IX]", DllEntryPoint);
		}
	}
	else
	{
		DbgShout("[LoadModuleFromMemory] AddressOfEntryPoint is NULL");
	}

	DbgShout("[LoadModuleFromMemory] Returning Pointer (0x%IX)", AllocatedRemoteMemory);

	if (m_bIs64bit)
	{
		TerminateWorkerThread();
	}

	return (HMODULE)AllocatedRemoteMemory;
}

//////////////////////////////////
//		Private Functions		//
//////////////////////////////////
HMODULE CRemoteLoader::GetRemoteModuleHandleA(LPCCH Module, ULONG* ModuleSize)
{
	wchar_t ModuleUnicode[MAX_PATH] = { 0 };
	size_t charsConverted;
	mbstowcs_s(&charsConverted, ModuleUnicode, Module, MAX_PATH);
	return GetRemoteModuleHandleW(ModuleUnicode, ModuleSize);
}

HMODULE CRemoteLoader::GetRemoteModuleHandleW(LPCWCH Module, ULONG* ModuleSize)
{
	void* dwModuleHandle = 0;

	PPROCESS_BASIC_INFORMATION pbi = NULL;
	PEB peb;
	PEB_LDR_DATA peb_ldr;

	// Try to allocate buffer 
	HANDLE	hHeap = GetProcessHeap();
	DWORD dwSize = sizeof(PROCESS_BASIC_INFORMATION);
	pbi = (PPROCESS_BASIC_INFORMATION)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSize);

	ULONG dwSizeNeeded = 0;
	NTSTATUS dwStatus = fnNTQIP(m_hProcess, ProcessBasicInformation, pbi, dwSize, &dwSizeNeeded);
	if (dwStatus >= 0 && dwSize < dwSizeNeeded)
	{
		if (pbi)
			HeapFree(hHeap, 0, pbi);

		pbi = (PPROCESS_BASIC_INFORMATION)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSizeNeeded);
		if (!pbi)
		{
			DbgShout("[GetRemoteModuleHandleW] Couldn't allocate heap buffer!\n");
			return NULL;
		}

		dwStatus = fnNTQIP(m_hProcess, ProcessBasicInformation, pbi, dwSizeNeeded, &dwSizeNeeded);
	}

	// Did we successfully get basic info on process
	if (dwStatus >= 0)
	{
		// Read Process Environment Block (PEB)
		if (pbi->PebBaseAddress)
		{
			SIZE_T dwBytesRead = 0;
			if (ReadProcessMemory(m_hProcess, pbi->PebBaseAddress, &peb, sizeof(peb), &dwBytesRead))
			{
				dwBytesRead = 0;
				if (ReadProcessMemory(m_hProcess, peb.Ldr, &peb_ldr, sizeof(peb_ldr), &dwBytesRead))
				{
					LIST_ENTRY *pLdrListHead = (LIST_ENTRY *)peb_ldr.InLoadOrderModuleList.Flink;
					LIST_ENTRY *pLdrCurrentNode = peb_ldr.InLoadOrderModuleList.Flink;
					do
					{
						LDR_DATA_TABLE_ENTRY lstEntry = { 0 };
						dwBytesRead = 0;
						if (!ReadProcessMemory(m_hProcess, (void*)pLdrCurrentNode, &lstEntry, sizeof(LDR_DATA_TABLE_ENTRY), &dwBytesRead))
						{
							DbgShout("[GetRemoteModuleHandleW] Could not read list entry from LDR list. Error = %s", Utils::GetLastErrorAsString().c_str());
							if (pbi)
								HeapFree(hHeap, 0, pbi);
							return NULL;
						}

						pLdrCurrentNode = lstEntry.InLoadOrderLinks.Flink;

						wchar_t wcsBaseDllName[MAX_PATH] = { 0 };
						if (lstEntry.BaseDllName.Length > 0)
						{
							dwBytesRead = 0;
							if (!ReadProcessMemory(m_hProcess, (LPCVOID)lstEntry.BaseDllName.Buffer, &wcsBaseDllName, lstEntry.BaseDllName.Length, &dwBytesRead))
							{
								DbgShout("[GetRemoteModuleHandleW] Could not read list entry DLL name. Error = %s", Utils::GetLastErrorAsString().c_str());
								if (pbi)
									HeapFree(hHeap, 0, pbi);
								return NULL;
							}
						}

						if (lstEntry.DllBase != nullptr && lstEntry.SizeOfImage != 0)
						{
							if (_wcsicmp(wcsBaseDllName, Module) == 0)
							{
								dwModuleHandle = lstEntry.DllBase;
								if (ModuleSize)
									*ModuleSize = lstEntry.SizeOfImage;
								break;
							}
						}

					} while (pLdrListHead != pLdrCurrentNode);

				} // Get Ldr
			} // Read PEB 
		} // Check for PEB
	}

	if (pbi)
		HeapFree(hHeap, 0, pbi);

	return (HMODULE)dwModuleHandle;
}

IMAGE_DOS_HEADER* CRemoteLoader::ToDos(PVOID BaseAddress)
{
	PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)(BaseAddress);
	if (!ImageDosHeader)
		return NULL;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;
	return ImageDosHeader;
}

IMAGE_NT_HEADERS* CRemoteLoader::ToNts(PVOID BaseAddress)
{
	IMAGE_DOS_HEADER* ImageDosHeader = ToDos(BaseAddress);
	if (ImageDosHeader == 0)
		return 0;
	IMAGE_NT_HEADERS* ImageNtHeaders = (IMAGE_NT_HEADERS*)((DWORD_PTR)BaseAddress + ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return 0;
	return ImageNtHeaders;
}

void* CRemoteLoader::RvaToPointer(ULONG RVA, PVOID BaseAddress)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == 0)
		return 0;
	return ::ImageRvaToVa(ImageNtHeaders, BaseAddress, RVA, 0);
}

BOOL CRemoteLoader::CallEntryPoint(void* BaseAddress, FARPROC Entrypoint)
{
	if (m_bIs64bit)
	{
		// ActivateActCtx 
		if (m_pAContext)
		{
			size_t rsp_dif = 0x28;
			rsp_dif = Utils::Align(rsp_dif, 0x10);
			// sub  rsp, (rsp_dif + 8)
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x83);
			AddByteToBuffer(0xEC);
			AddByteToBuffer((unsigned char)(rsp_dif + 8));
			// >>>
			// >>>
			// mov  rax, m_pAContext
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xB8);
			AddLong64ToBuffer((size_t)m_pAContext);
			// mov  rax, [rax]
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x00);
			// mov  rcx, rax -> first parameter
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xC1);
			// mov  rdx, (m_pAContext + sizeof(HANDLE)) -> second parameter
			LoadParam64((size_t)m_pAContext + sizeof(HANDLE), PARAM_INDEX_RDX);
			// mov  r13, calladdress
			AddByteToBuffer(0x49);
			AddByteToBuffer(0xBD);
			AddLong64ToBuffer((size_t)ActivateActCtx);
			// call r13
			AddByteToBuffer(0x41);
			AddByteToBuffer(0xFF);
			AddByteToBuffer(0xD5);
			// >>>
			// >>>
			// add rsp, (rsp_dif + 8)
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x83);
			AddByteToBuffer(0xC4);
			AddByteToBuffer((unsigned char)(rsp_dif + 8));
		}

		/* Call the actual entry point */
		PushInt64((unsigned __int64)BaseAddress);
		PushInt64(DLL_PROCESS_ATTACH);
		PushInt64(0x00);
		PushCall(CCONV_WIN64, Entrypoint);

		// DeactivateActCtx
		if (m_pAContext)
		{
			size_t rsp_dif = 0x28;
			rsp_dif = Utils::Align(rsp_dif, 0x10);
			// sub  rsp, (rsp_dif + 8)
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x83);
			AddByteToBuffer(0xEC);
			AddByteToBuffer((unsigned char)(rsp_dif + 8));
			// >>>
			// >>>
			// mov  rax, m_pAContext + sizeof(HANDLE)
			AddByteToBuffer(0x48);
			AddByteToBuffer(0xB8);
			AddLong64ToBuffer((size_t)m_pAContext + sizeof(HANDLE));
			// mov  rax, [rax]
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x8B);
			AddByteToBuffer(0x00);
			// mov  rcx, 0 -> first parameter
			LoadParam64(0, PARAM_INDEX_RCX);
			// mov  rdx, rax
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x89);
			AddByteToBuffer(0xC2);
			// mov  r13, calladdress
			AddByteToBuffer(0x49);
			AddByteToBuffer(0xBD);
			AddLong64ToBuffer((size_t)DeactivateActCtx);
			// call r13
			AddByteToBuffer(0x41);
			AddByteToBuffer(0xFF);
			AddByteToBuffer(0xD5);
			// >>>
			// >>>
			// add rsp, (rsp_dif + 8)
			AddByteToBuffer(0x48);
			AddByteToBuffer(0x83);
			AddByteToBuffer(0xC4);
			AddByteToBuffer((unsigned char)(rsp_dif + 8));
		}

		// Signal wait event
		SaveRetValAndSignalEvent();
		// Restore registers from stack and return
		EndCall64();

		size_t result;
		if (ExecuteInWorkerThread(m_CurrentRemoteThreadBuffer, result) != ERROR_SUCCESS)
		{
			TerminateWorkerThread();
			DestroyRemoteThreadBuffer();
			return FALSE;
		}

		return TRUE;
	}

	// x86 injection
	PushInt((INT)BaseAddress);
	PushInt(DLL_PROCESS_ATTACH);
	PushInt(0);
	PushCall(CCONV_STDCALL, Entrypoint);
	// Zero eax and return
	// xor eax, eax
	AddByteToBuffer(0x33);
	AddByteToBuffer(0xC0);
	// ret 4
	AddByteToBuffer(0xC2);
	AddByteToBuffer(0x04);
	AddByteToBuffer(0x00);

	DbgShout("\nCallEntryPoint [0x%IX]:", Entrypoint);

	return ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer, true);
}

bool CRemoteLoader::CreateActx(PVOID BaseAddress)
{
	if (CreateTempManifestFileFromMemory(BaseAddress, 2))
	{
		bool ret = CreateActxFromManifest(m_tempManifest);
		remove(m_tempManifest);
		ZeroMemory(m_tempManifest, MAX_PATH);
		return ret;
	}
	else
	{
		DbgShout("[LoadLibraryFromMemory] Failed to get temp manifest from memory using Resource ID 2. Trying ID 1...");
		//if (CreateTempManifestFileFromMemory(BaseAddress, 1))
		//{
		//	bool ret = CreateActxFromManifest(m_tempManifest);
		//	remove(m_tempManifest);
		//	ZeroMemory(m_tempManifest, MAX_PATH);
		//	return ret;
		//}
		//else
		return false;
	}
}

bool CRemoteLoader::CreateTempManifestFileFromMemory(PVOID BaseAddress, DWORD ResourceId)
{
	void* ManifestResource = NULL;
	DWORD ManifestSize = GetEmbeddedManifestResourceFromMemory(BaseAddress, ResourceId, &ManifestResource);
	if (ManifestResource)
	{
		if (!SetBaseDirectory())
		{
			DbgShout("[CreateTempManifestFileFromMemory] Failed to get base directory to create temp manifest in!");
			return false;
		}

		strcpy_s(m_tempManifest, m_baseDir);
		char randomFilename[MAX_PATH];
		sprintf_s(randomFilename, "%IX", GetTickCount64());
		strcat_s(m_tempManifest, randomFilename);

		DbgShout("[CreateTempManifestFileFromMemory] Temp file: %s", m_tempManifest);

		FILE* f;
		errno_t err = fopen_s(&f, m_tempManifest, "w");
		if (f && err == NULL)
			fwrite(ManifestResource, sizeof(char), ManifestSize, f);
		fclose(f);

		return true;
	}

	DbgShout("[CreateTempManifestFileFromMemory] Failed to obtain embedded Manifest resource from memory!");

	return false;
}

DWORD CRemoteLoader::GetEmbeddedManifestResourceFromMemory(PVOID BaseAddress, DWORD ResourceId, void** Resource)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	DWORD ResourceSize = ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	if (ResourceSize)
	{
		PIMAGE_RESOURCE_DIRECTORY RootResourceDir = (PIMAGE_RESOURCE_DIRECTORY)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress, BaseAddress);
		if (RootResourceDir)
		{
			const IMAGE_RESOURCE_DIR_STRING_U* dir_string = 0;

			// 
			// enumerate all types
			// 
			for (WORD i = 0; i < RootResourceDir->NumberOfIdEntries + RootResourceDir->NumberOfNamedEntries; i++)
			{
				PIMAGE_RESOURCE_DIRECTORY_ENTRY EntryType = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(RootResourceDir + 1 + i);
				if ((EntryType->OffsetToDirectory) >= ResourceSize)
					return NULL;

				PIMAGE_RESOURCE_DIRECTORY ResType = (PIMAGE_RESOURCE_DIRECTORY)((PBYTE)RootResourceDir + (EntryType->OffsetToDirectory));

				if (EntryType->NameIsString)
				{
					dir_string = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((PBYTE)RootResourceDir + EntryType->NameOffset);
					DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %S", &dir_string->NameString[0]);
				}
				else
				{
					DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %i", EntryType->Id);
				}

				//
				// enumerate all names
				//
				for (WORD j = 0; j < ResType->NumberOfIdEntries + ResType->NumberOfNamedEntries; j++)
				{
					PIMAGE_RESOURCE_DIRECTORY_ENTRY EntryIdentifier = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(ResType + 1 + j);
					if ((EntryIdentifier->OffsetToDirectory) >= ResourceSize)
						return NULL;

					// Check if the resource ID is what we're looking for or not
					if (EntryIdentifier->Id != ResourceId)
						continue;

					PIMAGE_RESOURCE_DIRECTORY ResIdentifier = (PIMAGE_RESOURCE_DIRECTORY)((PBYTE)RootResourceDir + (EntryIdentifier->OffsetToDirectory));

					if (EntryIdentifier->NameIsString)
					{
						dir_string = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((PBYTE)RootResourceDir + EntryIdentifier->NameOffset);
						DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %S", &dir_string->NameString[0]);
					}
					else
					{
						DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %i", EntryIdentifier->Id);
					}

					//
					// enumerate all languages 
					// now we have access to the offset of the data
					//
					for (WORD k = 0; k < ResIdentifier->NumberOfIdEntries + ResIdentifier->NumberOfNamedEntries; k++)
					{
						PIMAGE_RESOURCE_DIRECTORY_ENTRY DataLangEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(ResIdentifier + 1 + k);
						if ((DataLangEntry->OffsetToDirectory) >= ResourceSize)
							return FALSE;

						PIMAGE_RESOURCE_DATA_ENTRY pData = (PIMAGE_RESOURCE_DATA_ENTRY)((PBYTE)RootResourceDir + (DataLangEntry->OffsetToDirectory));

						if (DataLangEntry->NameIsString)
						{
							dir_string = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((PBYTE)RootResourceDir + DataLangEntry->NameOffset);
							DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %S", &dir_string->NameString[0]);
						}
						else
						{
							DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Id: %i", DataLangEntry->Id);
						}

						if (pData->Size == 0)
							continue;

						void* ResourceData = RvaToPointer(pData->OffsetToData, BaseAddress);
						if (ResourceData && (DWORD64)ResourceData == (DWORD64)-1)
							continue;	// empty or encrypted resource?

						DbgShout("[GetEmbeddedManifestResourceFromMemory] Resource Data: 0x%IX", ResourceData);

						*Resource = ResourceData;

						return pData->Size;
					}
				}
			}
		}
	}
	// Empty or no resource directory
	return NULL;
}

// Set custom exception handler to bypass SafeSEH under DEP 
NTSTATUS CRemoteLoader::EnableExceptions(PVOID BaseAddress, PVOID RemoteAddress, size_t ImageSize)
{
#ifdef _M_AMD64
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	size_t size = ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size;

	PIMAGE_RUNTIME_FUNCTION_ENTRY pExpTable = (PIMAGE_RUNTIME_FUNCTION_ENTRY)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress, BaseAddress);
	if (pExpTable)
	{
		size_t result = 0;

		size_t ExpTableAddr = (size_t)pExpTable - (size_t)BaseAddress + (size_t)RemoteAddress;

		BeginCall64();

		PushInt64(ExpTableAddr);
		PushInt64(size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY));
		PushInt64((size_t)RemoteAddress);
		PushCall(CCONV_WIN64, (FARPROC)RtlAddFunctionTable);

		SaveRetValAndSignalEvent();

		EndCall64();

		if (ExecuteInWorkerThread(m_CurrentRemoteThreadBuffer, result) != ERROR_SUCCESS)
			return false;

		return (CreateVEH((size_t)RemoteAddress, ImageSize) == ERROR_SUCCESS);
	}
	else
		return false;
#else
	return true;
#endif
}

DWORD CRemoteLoader::CreateVEH(size_t RemoteAddress /*= 0*/, size_t ImageSize /*= 0*/)
{
	return GetLastError();
}

// Calculate and set security cookie
bool CRemoteLoader::InitializeCookie(PVOID BaseAddress, PVOID RemoteAddress)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	PIMAGE_LOAD_CONFIG_DIRECTORY pLC = (PIMAGE_LOAD_CONFIG_DIRECTORY)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress, BaseAddress);

	//
	// Cookie generation based on MSVC++ compiler
	//
	if (pLC && pLC->SecurityCookie)
	{
		FILETIME systime = { 0 };
		LARGE_INTEGER PerformanceCount = { { 0 } };
		uintptr_t cookie = 0;

		GetSystemTimeAsFileTime(&systime);
		QueryPerformanceCounter(&PerformanceCount);

		cookie = m_dwProcessId ^ m_dwWorkerThreadId ^ reinterpret_cast<uintptr_t>(&cookie);

#ifdef _M_AMD64
		cookie ^= *reinterpret_cast<unsigned __int64*>(&systime);
		cookie ^= (PerformanceCount.QuadPart << 32) ^ PerformanceCount.QuadPart;
		cookie &= 0xFFFFFFFFFFFF;

		if (cookie == 0x2B992DDFA232)
			cookie++;
#else

		cookie ^= systime.dwHighDateTime ^ systime.dwLowDateTime;
		cookie ^= PerformanceCount.LowPart;
		cookie ^= PerformanceCount.HighPart;

		if (cookie == 0xBB40E64E)
			cookie++;
		else if (!(cookie & 0xFFFF0000))
			cookie |= (cookie | 0x4711) << 16;
#endif

		size_t RemoteCookieAddr = (size_t)pLC->SecurityCookie - (size_t)BaseAddress + (size_t)RemoteAddress;

		if (!WriteProcessMemory(m_hProcess, (void*)RemoteCookieAddr, (const void*)cookie, sizeof(uintptr_t), NULL))
		{
			DbgShout("[InitializeCookie] Failed to write generated security cookie!");
			return false;
		}

		return true;
	}

	DbgShout("[InitializeCookie] No security cookie for module. Continuing.");

	return true;
}

BOOL CRemoteLoader::ProcessImportTable(PVOID BaseAddress, PVOID RemoteAddress)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	if (ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		PIMAGE_IMPORT_DESCRIPTOR ImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, BaseAddress);
		if (ImageImportDescriptor)
		{
			for (; ImageImportDescriptor->Name; ImageImportDescriptor++)
			{
				char* ModuleName = (char*)RvaToPointer(ImageImportDescriptor->Name, BaseAddress);
				if (ModuleName == NULL)
				{
					DbgShout("[ProcessImportTable] Module name for entry NULL");
					continue;
				}

				std::string strDll = ModuleName;
				std::wstring strBaseDll = L"";
				std::wstring strBaseName = L"";

				strBaseDll.assign(strDll.begin(), strDll.end());
				strBaseName.assign(strDll.begin(), strDll.end());

				DbgShout("[ProcessImportTable] Module Name [%s]", strDll.c_str());

				HMODULE ModuleBase = GetRemoteModuleHandleW(strBaseDll.c_str());
				if (ModuleBase == NULL)
				{
					HMODULE ReturnModule = NULL;
					if (ResolvePath(strBaseDll, EnsureFullPath, strBaseName, &ReturnModule) == STATUS_IMAGE_ALREADY_LOADED)
					{
						ModuleBase = ReturnModule;
					}

					if (ModuleBase == NULL)
					{
						if (m_bIs64bit)
						{
							// Getting infinite recursive loop using LoadLibraryByPathIntoMemoryW??
							// So I have to use LdrpLoadDllW. Gonna research how file paths are resolved better later.
							ModuleBase = LdrpLoadDllW(strBaseDll.c_str());
						}
						else
						{
							ModuleBase = LoadLibraryByPathIntoMemoryW(strBaseDll.c_str(), TRUE);
						}
					}

					if (ModuleBase == NULL)
					{
						DbgShout("[ProcessImportTable] Failed to obtain module handle [%ls]", strBaseDll.c_str());
						continue;
					}
				}

				IMAGE_THUNK_DATA *ImageThunkData = NULL;
				IMAGE_THUNK_DATA *ImageFuncData = NULL;

				if (ImageImportDescriptor->OriginalFirstThunk)
				{
					ImageThunkData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageImportDescriptor->OriginalFirstThunk, BaseAddress);
					ImageFuncData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageImportDescriptor->FirstThunk, BaseAddress);
				}
				else
				{
					ImageThunkData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageImportDescriptor->FirstThunk, BaseAddress);
					ImageFuncData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageImportDescriptor->FirstThunk, BaseAddress);
				}

				if (ImageThunkData == NULL || ImageFuncData == NULL) {
					DbgShout("[ProcessImportTable] Image Thunk Data is NULL");
				}

				for (; ImageThunkData->u1.AddressOfData; ImageThunkData++, ImageFuncData++)
				{
					FARPROC FunctionAddress = NULL;

					bool bSnapByOrdinal = m_bIs64bit ? ((ImageThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG64) != 0) : ((ImageThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG32) != 0);
					if (bSnapByOrdinal)
					{
						SHORT Ordinal = (SHORT)(ImageThunkData->u1.Ordinal & 0xffff);

						FunctionAddress = (FARPROC)GetDependencyProcAddressA(ModuleBase, (const char*)Ordinal);

						// Failed to resolve import
						if (FunctionAddress == 0)
						{
							// TODO: Add error code
							DbgShout("[ProcessImportTable] Failed to get import [%d] from image [%ls]", Ordinal, strBaseDll.c_str());
							return FALSE;
						}

						DbgShout("[ProcessImportTable] Processed (%ls -> %i) -> (0x%IX)", strBaseDll.c_str(), Ordinal, FunctionAddress);
					}
					else
					{
						PIMAGE_IMPORT_BY_NAME ImageImportByName = (PIMAGE_IMPORT_BY_NAME)RvaToPointer(*(DWORD*)ImageThunkData, BaseAddress);
						char* NameOfImport = (char*)ImageImportByName->Name;

						FunctionAddress = (FARPROC)GetDependencyProcAddressA(ModuleBase, NameOfImport);
						// Failed to resolve import
						if (FunctionAddress == 0)
						{
							// TODO: Add error code				
							DbgShout("[ProcessImportTable] Failed to get import [%s] from image [%ls]", (PCHAR)ImageImportByName->Name, strBaseDll.c_str());
							return FALSE;
						}

						DbgShout("[ProcessImportTable] Processed (%ls -> %s) -> (0x%IX)", strBaseDll.c_str(), NameOfImport, FunctionAddress);
					}

					ImageFuncData->u1.Function = (size_t)FunctionAddress;
				}
			}
			return TRUE;
		}
		else
		{
			DbgShout("[ProcessImportTable] Size of table confirmed but pointer to data invalid!");
			return FALSE;
		}
	}
	else
	{
		DbgShout("[ProcessImportTable] No Imports");
		return TRUE;
	}

	return FALSE;
}

BOOL CRemoteLoader::ProcessDelayedImportTable(PVOID BaseAddress, PVOID RemoteAddress)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	if (ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size)
	{
		PIMAGE_IMPORT_DESCRIPTOR ImageDelayedImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress, BaseAddress);

		if (ImageDelayedImportDescriptor)
		{
			for (; ImageDelayedImportDescriptor->Name; ImageDelayedImportDescriptor++)
			{
				char* ModuleName = (char*)RvaToPointer(ImageDelayedImportDescriptor->Name, BaseAddress);
				if (ModuleName == NULL)
				{
					DbgShout("[ProcessDelayedImportTable] Module name for entry NULL");
					continue;
				}

				std::string strDll = ModuleName;
				std::wstring strBaseDll = L"";
				std::wstring strBaseName = L"";

				strBaseDll.assign(strDll.begin(), strDll.end());
				strBaseName.assign(strDll.begin(), strDll.end());

				DbgShout("[ProcessDelayedImportTable] Module Name [%s]", strDll.c_str());

				HMODULE ModuleBase = GetRemoteModuleHandleW(strBaseDll.c_str());
				if (ModuleBase == NULL)
				{
					HMODULE ReturnModule = NULL;
					if (ResolvePath(strBaseDll, EnsureFullPath, strBaseName, &ReturnModule) == STATUS_IMAGE_ALREADY_LOADED)
					{
						ModuleBase = ReturnModule;
					}

					if (ModuleBase == NULL)
					{
						if (m_bIs64bit)
						{
							// Getting infinite recursive loop using LoadLibraryByPathIntoMemoryW??
							// So I have to use LdrpLoadDllW. Gonna research how file paths are resolved better later.
							ModuleBase = LdrpLoadDllW(strBaseDll.c_str());
						}
						else
						{
							ModuleBase = LoadLibraryByPathIntoMemoryW(strBaseDll.c_str(), TRUE);
						}
					}

					if (ModuleBase == NULL)
					{
						DbgShout("[ProcessDelayedImportTable] Failed to obtain module handle [%s]", ModuleName);
						continue;
					}
				}

				IMAGE_THUNK_DATA *ImageThunkData = NULL;
				IMAGE_THUNK_DATA *ImageFuncData = NULL;

				if (ImageDelayedImportDescriptor->OriginalFirstThunk)
				{
					ImageThunkData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageDelayedImportDescriptor->OriginalFirstThunk, BaseAddress);
					ImageFuncData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageDelayedImportDescriptor->FirstThunk, BaseAddress);
				}
				else
				{
					ImageThunkData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageDelayedImportDescriptor->FirstThunk, BaseAddress);
					ImageFuncData = (IMAGE_THUNK_DATA*)RvaToPointer(ImageDelayedImportDescriptor->FirstThunk, BaseAddress);
				}

				if (ImageThunkData == NULL || ImageFuncData == NULL) {
					DbgShout("[ProcessDelayedImportTable] Image Thunk Data is NULL");
				}

				for (; ImageThunkData->u1.AddressOfData; ImageThunkData++, ImageFuncData++)
				{
					FARPROC FunctionAddress = NULL;

					bool bSnapByOrdinal = false;
					if (m_bIs64bit)
						bSnapByOrdinal = ((ImageThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG64) != 0);
					else
						bSnapByOrdinal = ((ImageThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG32) != 0);

					if (bSnapByOrdinal)
					{
						WORD Ordinal = (WORD)(ImageThunkData->u1.Ordinal & 0xffff);

						FunctionAddress = (FARPROC)GetDependencyProcAddressA(ModuleBase, (const char*)Ordinal); // Utils::GetProcAddress
																												// Failed to resolve import
						if (FunctionAddress == 0)
						{
							// TODO: Add error code			
							DbgShout("[ProcessDelayedImportTable] Failed to get import [%d] from image [%s]", Ordinal, ModuleName);
							return FALSE;
						}

						DbgShout("[ProcessDelayedImportTable] Processed (%s -> %i) -> (0x%IX)", ModuleName, Ordinal, FunctionAddress);
					}
					else
					{
						PIMAGE_IMPORT_BY_NAME ImageImportByName = (PIMAGE_IMPORT_BY_NAME)RvaToPointer(*(DWORD*)ImageThunkData, BaseAddress);

						FunctionAddress = (FARPROC)GetDependencyProcAddressA(ModuleBase, (LPCCH)ImageImportByName->Name); // Utils::GetProcAddress
																														  // Failed to resolve import
						if (FunctionAddress == 0)
						{
							// TODO: Add error code				
							DbgShout("[ProcessDelayedImportTable] Failed to get import [%s] from image [%s]", (PCHAR)ImageImportByName->Name, ModuleName);
							return FALSE;
						}

						DbgShout("[ProcessDelayedImportTable] Processed (%s -> %s) -> (0x%IX)", ModuleName, (PCHAR)ImageImportByName->Name, FunctionAddress);
					}

					ImageFuncData->u1.Function = (ULONGLONG)FunctionAddress;
				}
			}

			return TRUE;
		}
		else
		{
			DbgShout("[ProcessDelayedImportTable] Size of table confirmed but pointer to data invalid!");
			return FALSE;
		}
	}
	else
	{
		DbgShout("[ProcessDelayedImportTable] No Delayed Imports");
		return TRUE;
	}

	return FALSE;
}

FARPROC CRemoteLoader::GetRemoteProcAddressW(LPCWCH Module, LPCWCH procName)
{
	char ModuleAnsi[MAX_PATH] = { 0 };
	size_t charsConverted;
	wcstombs_s(&charsConverted, ModuleAnsi, Module, MAX_PATH);

	char ProcNameAnsi[MAX_PATH];
	wcstombs_s(&charsConverted, ProcNameAnsi, procName, MAX_PATH);

	return GetRemoteProcAddressA(ModuleAnsi, ProcNameAnsi);
}

FARPROC CRemoteLoader::GetDependencyProcAddressW(HMODULE ModuleBase, LPCWCH procName)
{
	char ProcAnsi[MAX_PATH];
	size_t charsConverted;
	wcstombs_s(&charsConverted, ProcAnsi, procName, MAX_PATH);
	return GetDependencyProcAddressA(ModuleBase, ProcAnsi);
}

FARPROC CRemoteLoader::GetDependencyProcAddressA(HMODULE ModuleBase, LPCCH proc_name)
{
	void* modb = (void*)ModuleBase;

	IMAGE_DOS_HEADER hdrDos = { 0 };
	IMAGE_NT_HEADERS hdrNt32 = { 0 };
	IMAGE_EXPORT_DIRECTORY* expData = { 0 };
	void* pFunc = NULL;

	SIZE_T dwRead = 0;
	ReadProcessMemory(m_hProcess, (BYTE*)modb, &hdrDos, sizeof(IMAGE_DOS_HEADER), &dwRead);
	if (hdrDos.e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	ReadProcessMemory(m_hProcess, (BYTE*)modb + hdrDos.e_lfanew, &hdrNt32, sizeof(IMAGE_NT_HEADERS), &dwRead);
	if (hdrNt32.Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	size_t expBase = hdrNt32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	// Exports are present
	if (expBase)
	{
		DWORD expSize = hdrNt32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		expData = (IMAGE_EXPORT_DIRECTORY*)malloc(expSize);
		ReadProcessMemory(m_hProcess, (BYTE*)modb + expBase, expData, expSize, &dwRead);

		WORD  *pAddressOfOrds = (WORD*)(expData->AddressOfNameOrdinals + (size_t)expData - expBase);
		DWORD *pAddressOfNames = (DWORD*)(expData->AddressOfNames + (size_t)expData - expBase);
		DWORD *pAddressOfFuncs = (DWORD*)(expData->AddressOfFunctions + (size_t)expData - expBase);

		for (DWORD i = 0; i < expData->NumberOfFunctions; ++i)
		{
			WORD OrdIndex = 0xFFFF;
			char *pName = NULL;
			// Find by index
			if ((size_t)proc_name <= 0xFFFF)
				OrdIndex = (WORD)i;
			// Find by name
			else if ((size_t)proc_name > 0xFFFF && i < expData->NumberOfNames)
			{
				pName = (char*)(pAddressOfNames[i] + (size_t)expData - expBase);
				OrdIndex = (WORD)pAddressOfOrds[i];
			}
			else
				return 0;

			if (((size_t)proc_name <= 0xFFFF && (WORD)proc_name == (OrdIndex + expData->Base)) || ((size_t)proc_name > 0xFFFF && strcmp(pName, proc_name) == 0))
			{
				pFunc = (void*)((size_t)modb + pAddressOfFuncs[OrdIndex]);
				// Check forwarded export
				if ((size_t)pFunc >= (size_t)modb + expBase && (size_t)pFunc <= (size_t)modb + expBase + expSize)
				{
					char forwardStr[255] = { 0 };
					ReadProcessMemory(m_hProcess, pFunc, forwardStr, sizeof(forwardStr), &dwRead);

					std::string chainExp(forwardStr);

					std::string strDll = chainExp.substr(0, chainExp.find(".")) + ".dll";
					std::string strName = chainExp.substr(chainExp.find(".") + 1, strName.npos);

					HMODULE hChainMod = GetRemoteModuleHandleA(strDll.c_str());
					if (hChainMod == NULL)
					{
						std::wstring strBaseDll = L"";
						strBaseDll.assign(strDll.begin(), strDll.end());

						//
						// Check the C:\\Windows\\System32\\downlevel folder for 'api-ms-' or 'ext-ms-' file
						//
						wchar_t tmpPath[4096] = { 0 };
						GetSystemDirectoryW(tmpPath, ARRAYSIZE(tmpPath));
						std::wstring completePath = std::wstring(tmpPath) + L"\\downlevel\\" + strBaseDll;
						if (Utils::FileExists(completePath))
						{
							strBaseDll = completePath;
						}

						if (hChainMod == NULL)
						{
							hChainMod = LdrpLoadDllW(strBaseDll.c_str());
						}
					}

					// Import by ordinal
					if (strName.find("#") == 0)
						return GetDependencyProcAddressA(hChainMod, (const char*)atoi(strName.c_str() + 1));
					else // Import by name
						return GetDependencyProcAddressA(hChainMod, strName.c_str());
				}

				break;
			}
		}
		// Free allocated data
		free(expData);
	}

	return (FARPROC)pFunc;
}

FARPROC	CRemoteLoader::GetRemoteProcAddressW(LPCWCH Module, SHORT procOrdinal)
{
	char ModuleAnsi[MAX_PATH];
	size_t charsConverted;
	wcstombs_s(&charsConverted, ModuleAnsi, Module, MAX_PATH);

	return GetRemoteProcAddressA(ModuleAnsi, procOrdinal);
}

FARPROC CRemoteLoader::GetRemoteProcAddressA(LPCCH Module, LPCCH procName)
{
	HMODULE hKernel32 = Utils::GetLocalModuleHandle("Kernel32.dll");
	if (hKernel32 == NULL)
		return NULL;

	size_t GetProcAddressOffset = (size_t)GetProcAddress - (size_t)hKernel32;

	HMODULE hRemoteKernel32 = GetRemoteModuleHandleA("Kernel32.dll");
	if (hRemoteKernel32 == NULL)
		return NULL;

	HMODULE hRemoteModule = GetRemoteModuleHandleA(Module);
	if (hRemoteModule == NULL)
	{
		DbgShout("[GetRemoteProcAddressA] Failed to obtain module handle [%s]", Module);
		return NULL;
	}

	PVOID ReturnPointerValue = RemoteAllocateMemory(sizeof(size_t));

	if (m_bIs64bit)
	{
		// Backup RCX, RDX, R8, and R9 on stack
		BeginCall64();

		PushInt64((unsigned __int64)hRemoteModule);
		PushANSIString((PCHAR)procName);
		PushCall(CCONV_WIN64, (FARPROC)((size_t)hRemoteKernel32 + (size_t)GetProcAddressOffset));

		// mov [ReturnPointerValue], rax
		AddByteToBuffer(0x48);
		AddByteToBuffer(0xA3);
		AddLong64ToBuffer((unsigned __int64)ReturnPointerValue);

		SaveRetValAndSignalEvent();

		// Restore RCX, RDX, R8, and R9 from stack and return
		EndCall64();
	}
	else
	{
		PushInt((unsigned int)hRemoteModule);
		PushANSIString((PCHAR)procName);
		PushCall(CCONV_STDCALL, (FARPROC)((size_t)hRemoteKernel32 + (size_t)GetProcAddressOffset));

		//mov ptr, eax
		AddByteToBuffer(0xA3);
		AddLongToBuffer((DWORD)ReturnPointerValue);

		//xor eax, eax
		AddByteToBuffer(0x33);
		AddByteToBuffer(0xC0);

		//retn 4
		AddByteToBuffer(0xC2);
		AddByteToBuffer(0x04);
		AddByteToBuffer(0x00);
	}

	if (m_bIs64bit)
	{
		size_t result;
		if (ExecuteInWorkerThread(m_CurrentRemoteThreadBuffer, result) != ERROR_SUCCESS)
		{
			RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
			return NULL;
		}
	}
	else
	{
		if (!ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer, true))
		{
			RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
			return NULL;
		}
	}

	size_t ProcAddressRemote = 0;
	if (ReadProcessMemory(m_hProcess, ReturnPointerValue, &ProcAddressRemote, sizeof(size_t), NULL) == TRUE)
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		return (FARPROC)ProcAddressRemote;
	}

	RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));

	return NULL;
}

FARPROC CRemoteLoader::GetRemoteProcAddressA(LPCCH Module, SHORT procOrdinal)
{
	HMODULE hKernel32 = Utils::GetLocalModuleHandle("Kernel32.dll");
	if (hKernel32 == NULL)
		return NULL;

	size_t GetProcAddressOffset = (size_t)GetProcAddress - (size_t)hKernel32;

	HMODULE hRemoteKernel32 = GetRemoteModuleHandleA("Kernel32.dll");
	if (hRemoteKernel32 == NULL)
		return NULL;

	HMODULE hRemoteModule = GetRemoteModuleHandleA(Module);
	if (hRemoteModule == NULL)
	{
		std::string strDll = Module;
		std::wstring strBaseDll = L"";
		std::wstring strBaseName = L"";

		strBaseDll.assign(strDll.begin(), strDll.end());
		ResolvePath(strBaseDll, EnsureFullPath, strBaseName);

		hRemoteModule = LdrLoadDllW(strBaseDll.c_str());
		if (hRemoteModule == NULL)
			return NULL;
	}

	PVOID ReturnPointerValue = RemoteAllocateMemory(sizeof(size_t));

	PushInt((INT)hRemoteModule);
	PushInt((INT)procOrdinal);
	PushCall(CCONV_STDCALL, (FARPROC)((size_t)hRemoteKernel32 + (size_t)GetProcAddressOffset));

	//mov ptr, eax
	AddByteToBuffer(0xA3);
	AddLongToBuffer((DWORD)ReturnPointerValue);

	//xor eax, eax
	AddByteToBuffer(0x33);
	AddByteToBuffer(0xC0);

	//retn 4
	AddByteToBuffer(0xC2);
	AddByteToBuffer(0x04);
	AddByteToBuffer(0x00);

	if (!ExecuteRemoteThreadBuffer(m_CurrentRemoteThreadBuffer, true))
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(DWORD));
		return NULL;
	}

	size_t ProcAddressRemote = 0;
	if (ReadProcessMemory(m_hProcess, ReturnPointerValue, &ProcAddressRemote, sizeof(DWORD), NULL) == TRUE)
	{
		RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));
		return (FARPROC)ProcAddressRemote;
	}

	RemoteFreeMemory(ReturnPointerValue, sizeof(size_t));

	return NULL;
}

BOOL CRemoteLoader::ProcessRelocation(size_t ImageBaseDelta, WORD Data, PBYTE RelocationBase)
{
	BOOL bReturn = TRUE;
	switch (IMR_RELTYPE(Data))
	{
	case IMAGE_REL_BASED_HIGH:
	{
		SHORT* Raw = (SHORT*)(RelocationBase + IMR_RELOFFSET(Data));
		SHORT Backup = *Raw;

		*Raw += (ULONG)HIWORD(ImageBaseDelta);

		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_HIGH (0x%IX) -> (0x%IX)", Backup, *Raw);
		break;
	}
	case IMAGE_REL_BASED_LOW:
	{
		SHORT* Raw = (SHORT*)(RelocationBase + IMR_RELOFFSET(Data));
		SHORT Backup = *Raw;

		*Raw += (ULONG)LOWORD(ImageBaseDelta);

		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_LOW (0x%IX) -> (0x%X)", Backup, *Raw);
		break;
	}
	case IMAGE_REL_BASED_HIGHLOW:
	{
		size_t* Raw = (size_t*)(RelocationBase + IMR_RELOFFSET(Data));
		size_t Backup = *Raw;

		*Raw += (size_t)ImageBaseDelta;

		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_HIGHLOW (0x%IX) -> (0x%X)", Backup, *Raw);
		break;
	}
	case IMAGE_REL_BASED_DIR64:
	{
		DWORD_PTR UNALIGNED* Raw = (DWORD_PTR UNALIGNED*)(RelocationBase + IMR_RELOFFSET(Data));
		DWORD_PTR UNALIGNED Backup = *Raw;

		*Raw += ImageBaseDelta;

		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_DIR64 (0x%IX) -> (0x%IX)", Backup, *Raw);
		break;
	}
	case IMAGE_REL_BASED_ABSOLUTE: // No action required
	{
		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_ABSOLUTE no need to process");
		break;
	}
	case IMAGE_REL_BASED_HIGHADJ: // no action required
	{
		DbgShout("[ProcessRelocation] IMAGE_REL_BASED_HIGHADJ no need to process");
		break;
	}
	default:
	{
		DbgShout("[ProcessRelocation] UNKNOWN RELOCATION (0x%IX)", IMR_RELTYPE(Data));
		bReturn = FALSE;
		break;
	}

	} // end of switch

	return bReturn;
}

BOOL CRemoteLoader::ProcessRelocations(PVOID BaseAddress, PVOID RemoteAddress)
{
	IMAGE_NT_HEADERS* ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	if (ImageNtHeaders->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
	{
		DbgShout("[ProcessRelocations] Relocations have been stripped from this executable, continuing..");
		return TRUE;
	}
	else
	{
		size_t ImageBaseDelta = MakeDelta(size_t, RemoteAddress, ImageNtHeaders->OptionalHeader.ImageBase);
		DbgShout("[ProcessRelocations] VirtualAddress (0x%IX)", ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

		DWORD RelocationSize = ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		DbgShout("[ProcessRelocations] Relocation Size [0x%IX]", RelocationSize);

		if (RelocationSize)
		{
			PIMAGE_BASE_RELOCATION RelocationDirectory = (PIMAGE_BASE_RELOCATION)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, BaseAddress);
			if (RelocationDirectory)
			{
				DbgShout("[ProcessRelocations] RelocationDirectory (0x%IX)", RelocationDirectory);

				PVOID RelocationEnd = reinterpret_cast<PBYTE>(RelocationDirectory) + RelocationSize;

				while (RelocationDirectory < RelocationEnd)
				{
					PBYTE RelocBase = static_cast<PBYTE>(RvaToPointer(RelocationDirectory->VirtualAddress, BaseAddress));
					DWORD NumRelocs = (RelocationDirectory->SizeOfBlock - 8) >> 1;
					PWORD RelocationData = reinterpret_cast<PWORD>(RelocationDirectory + 1);

					DbgShout("[ProcessRelocations] RelocationData (0x%IX)", RelocationData);

					for (DWORD i = 0; i < NumRelocs; ++i, ++RelocationData)
					{
						if (ProcessRelocation(ImageBaseDelta, *RelocationData, RelocBase) == FALSE) {
							DbgShout("[ProcessRelocations] Unable to process relocation (%i)", i);
						}
					}

					RelocationDirectory = reinterpret_cast<PIMAGE_BASE_RELOCATION>(RelocationData);
				}
			}
			else
			{
				DbgShout("[ProcessRelocations] Relocations have a size, but the pointer is invalid");
				return FALSE;
			}
		}
		else
		{
			DbgShout("[ProcessRelocations] Relocations have have not been found in this executable, continuing..");
			return TRUE;
		}
	}

	return TRUE;
}

ULONG CRemoteLoader::GetSectionProtection(ULONG Characteristics)
{
	ULONG Result = 0;
	if (Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
		Result |= PAGE_NOCACHE;

	if (Characteristics & IMAGE_SCN_MEM_EXECUTE)
	{
		if (Characteristics & IMAGE_SCN_MEM_READ)
		{
			if (Characteristics & IMAGE_SCN_MEM_WRITE)
				Result |= PAGE_EXECUTE_READWRITE;
			else
				Result |= PAGE_EXECUTE_READ;
		}
		else if (Characteristics & IMAGE_SCN_MEM_WRITE)
			Result |= PAGE_EXECUTE_WRITECOPY;
		else
			Result |= PAGE_EXECUTE;
	}
	else if (Characteristics & IMAGE_SCN_MEM_READ)
	{
		if (Characteristics & IMAGE_SCN_MEM_WRITE)
			Result |= PAGE_READWRITE;
		else
			Result |= PAGE_READONLY;
	}
	else if (Characteristics & IMAGE_SCN_MEM_WRITE)
		Result |= PAGE_WRITECOPY;
	else
		Result |= PAGE_NOACCESS;

	return Result;
}

BOOL CRemoteLoader::ProcessSection(BYTE* Name, PVOID BaseAddress, PVOID RemoteAddress, ULONGLONG RawData, ULONGLONG VirtualAddress, ULONGLONG RawSize, ULONGLONG VirtualSize, ULONG ProtectFlag)
{
	DbgShout("[ProcessSection] ProcessSection( %s, 0x%IX, 0x%IX, 0x%IX, 0x%IX, 0x%IX, 0x%IX, 0x%IX )", Name, BaseAddress, RemoteAddress, RawData, VirtualAddress, RawSize, VirtualSize, ProtectFlag);

	if (WriteProcessMemory(m_hProcess, MakePtr(PVOID, RemoteAddress, VirtualAddress), MakePtr(PVOID, BaseAddress, RawData), (SIZE_T)RawSize, NULL) == FALSE)
	{
		DbgShout("[ProcessSection] Failed to write memory for %s. Error: [%s]", Name, Utils::GetLastErrorAsString().c_str());
		return FALSE;
	}

	DWORD dwOldProtect = NULL;
	if (VirtualProtectEx(m_hProcess, MakePtr(PVOID, RemoteAddress, VirtualAddress), (SIZE_T)VirtualSize, ProtectFlag, &dwOldProtect) == FALSE)
	{
		DbgShout("[ProcessSection] Failed to protect memory for %s. Error: [%s]", Name, Utils::GetLastErrorAsString().c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL CRemoteLoader::ProcessSections(PVOID BaseAddress, PVOID RemoteAddress, BOOL MapPEHeader)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;

	// Writing the PE header
	if (MapPEHeader)
	{
		if (WriteProcessMemory(m_hProcess, RemoteAddress, BaseAddress, ImageNtHeaders->OptionalHeader.SizeOfHeaders, NULL) == FALSE)
			DbgShout("[ProcessSections] Failed to map PE header!");
		else
			DbgShout("[ProcessSections] Mapped PE Header successfully!");
	}
	else
	{
		DbgShout("[ProcessSections] PE Header mapping disabled, skipping.");
	}

	// Write individual sections
	PIMAGE_SECTION_HEADER ImageSectionHeader = (PIMAGE_SECTION_HEADER)((ULONG_PTR)&ImageNtHeaders->OptionalHeader + ImageNtHeaders->FileHeader.SizeOfOptionalHeader);
	for (DWORD i = 0; i < ImageNtHeaders->FileHeader.NumberOfSections; i++)
	{
		if (_stricmp(".reloc", (char*)ImageSectionHeader[i].Name) == 0)
		{
			DbgShout("[ProcessSections] Skipping \".reloc\" section.");
			continue; // NOPE, do not process the .reloc section
		}

		// Skip discardable sections
		if (ImageSectionHeader[i].Characteristics & (IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE))
		{
			ULONG Protection = GetSectionProtection(ImageSectionHeader[i].Characteristics);
			if (ProcessSection(ImageSectionHeader[i].Name, BaseAddress, RemoteAddress, ImageSectionHeader[i].PointerToRawData, ImageSectionHeader[i].VirtualAddress, ImageSectionHeader[i].SizeOfRawData, ImageSectionHeader[i].Misc.VirtualSize, Protection) == FALSE)
				DbgShout("[ProcessSections] Failed [%s]", ImageSectionHeader[i].Name);
			else
				DbgShout("[ProcessSections] Success [%s]", ImageSectionHeader[i].Name);
		}
	}

	return TRUE;
}

BOOL CRemoteLoader::ProcessTlsEntries(PVOID BaseAddress, PVOID RemoteAddress)
{
	IMAGE_NT_HEADERS* ImageNtHeaders = ToNts(BaseAddress);
	if (ImageNtHeaders == NULL)
		return FALSE;
	if (ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size == 0)
	{
		DbgShout("[ProcessTlsEntries] No Tls entries to process");
		return TRUE; // Success when there is no Tls Entries <--- always hits here
	}

	DbgShout("[ProcessTlsEntries] Tls Data detected!");

	PIMAGE_TLS_DIRECTORY TlsDirectory = (PIMAGE_TLS_DIRECTORY)RvaToPointer(ImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress, BaseAddress);
	if (TlsDirectory == NULL)
		return TRUE; // Success when there is no Tls entries / broken data?

	DbgShout("[ProcessTlsEntries] TlsDirectory (0x%IX)", TlsDirectory);

	if (TlsDirectory->AddressOfCallBacks == NULL)
		return TRUE; // Success when there is no Tls entries / broken data?

	DbgShout("[ProcessTlsEntries] TlsDirectory->AddressOfCallBacks (0x%IX)", TlsDirectory->AddressOfCallBacks);

	PIMAGE_TLS_CALLBACK TLSCallbacks[0xFF];
	if (ReadProcessMemory(m_hProcess, (void*)TlsDirectory->AddressOfCallBacks, TLSCallbacks, sizeof(TLSCallbacks), NULL) == FALSE)
	{
		DbgShout("[ProcessTlsEntries] ReadProcessMemory Failed");
		return FALSE;
	}

	BOOL SuccessValue = TRUE;
	for (int i = 0; TLSCallbacks[i]; i++)
	{
		DbgShout("[ProcessTlsEntries] TLSCallbacks[%i] = 0x%IX (0x%IX)", i, TLSCallbacks[i], RemoteAddress);

		// As a consequence of the relocation stuff mentioned above, pCallbacks[i] is already fixed
		if (CallEntryPoint(RemoteAddress, (FARPROC)TLSCallbacks[i]) == false)
			DbgShout("[ProcessTlsEntries] Failed to execute Tls Entry [%i]", i);
		else
			DbgShout("[ProcessTlsEntries] Called Tls Callback (0x%IX)", TLSCallbacks[i]);
	}

	return SuccessValue;
}

///////////////////////
// Private functions //
///////////////////////
ModuleFile CRemoteLoader::InitModuleFile(LPCCH FileName)
{
	ModuleFile r;

	r.Buffer = 0;
	r.Size = 0;

	HANDLE hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DbgShout("[InitModuleFile] CreateFile Failed");
		return r;
	}

	DbgShout("[InitModuleFile] File opened");

	if (GetFileAttributesA(FileName) & FILE_ATTRIBUTE_COMPRESSED)
	{
		r.Size = GetCompressedFileSizeA(FileName, NULL);
		DbgShout("[InitModuleFile] File is compressed!");
	}
	else
	{
		r.Size = GetFileSize(hFile, NULL);
	}

	DbgShout("[InitModuleFile] Size [0x%IX]", r.Size);

	if (r.Size == 0)
	{
		CloseHandle(hFile);
		return r;
	}

	unsigned char* AllocatedFile = (unsigned char*)VirtualAlloc(NULL, r.Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (AllocatedFile == NULL)
	{
		DbgShout("[InitModuleFile] Failed to allocate buffer!");
		r.Size = 0;
		CloseHandle(hFile);
		return r;
	}

	DbgShout("[InitModuleFile] Buffer allocated!");

	DWORD NumberOfBytesRead = 0;
	if (ReadFile(hFile, AllocatedFile, r.Size, &NumberOfBytesRead, FALSE) == FALSE)
	{
		DbgShout("[InitModuleFile] Read file failed");
		r.Buffer = 0;
		r.Size = 0;
	}
	else
	{
		DbgShout("[InitModuleFile] Read file complete [0x%IX]", NumberOfBytesRead);
		r.Buffer = AllocatedFile;
	}

	DbgShout("[InitModuleFile] Buffer [0x%IX]", r.Buffer);

	CloseHandle(hFile);

	return r;
}

BOOL CRemoteLoader::FreeModuleFile(ModuleFile Handle)
{
	if (Handle.Buffer)
	{
		VirtualFree(Handle.Buffer, Handle.Size, MEM_RELEASE);
		Handle.Buffer = 0;
	}

	Handle.Size = 0;

	return (Handle.Buffer == 0 && Handle.Size == 0);
}