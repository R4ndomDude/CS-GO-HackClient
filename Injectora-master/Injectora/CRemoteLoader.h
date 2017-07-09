#include "CRemoteCode.h"

#include "ApiSet.h"

#include <map>

#ifdef UNICODE
#undef UNICODE
#endif

#ifndef _CREMOTELOAD_H_
#define _CREMOTELOAD_H_

#ifndef IMR_RELTYPE
#define IMR_RELTYPE(x)				((x >> 12) & 0xF)
#endif

#ifndef IMR_RELOFFSET
#define IMR_RELOFFSET(x)			(x & 0xFFF)
#endif

#define MakePtr(cast, ptr, addValue) (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
#define MakeDelta(cast, x, y) (cast)((DWORD_PTR)(x) - (DWORD_PTR)(y))

struct ModuleFile
{
	PVOID							Buffer;
	int								Size;

	bool IsValid() { return ( Buffer && Size ); }
};

// Relocation block information
struct RelocData
{
	ULONG VirtualAddress;
	ULONG SizeOfBlock;
	struct
	{
		WORD Offset : 12;
		WORD Type : 4;
	}Item[1];
};

class CRemoteLoader : public CRemoteCode
{
public:
	CRemoteLoader() : 
		m_hActx(0)
	{ 
		InitializeApiSchema(); 
	}

	void							SetProcess(HANDLE hProcess, DWORD dwProcessId);

	HMODULE							LdrpLoadDllA(LPCCH Path);
	HMODULE							LdrpLoadDllW(LPCWCH Path);
	HMODULE							LdrLoadDllA(LPCCH Path);
	HMODULE							LdrLoadDllW(LPCWCH Path);
	HMODULE							LoadLibraryByPathA(LPCCH Path, ULONG Flags = NULL);
	HMODULE							LoadLibraryByPathW(LPCWCH Path, ULONG Flags = NULL);
	HMODULE							LoadLibraryByPathIntoMemoryA(LPCCH Path, BOOL PEHeader);
	HMODULE							LoadLibraryByPathIntoMemoryW(LPCWCH Path, BOOL PEHeader);
	HMODULE							LoadLibraryFromMemory(PVOID BaseAddress, DWORD SizeOfModule, BOOL PEHeader);

private:
	HMODULE							GetRemoteModuleHandleA(LPCCH Module, ULONG* ModuleSize = NULL);
	HMODULE							GetRemoteModuleHandleW(LPCWCH Module, ULONG* ModuleSize = NULL);

public:
	FARPROC							GetDependencyProcAddressA(HMODULE Module, LPCCH procName);
	FARPROC							GetDependencyProcAddressW(HMODULE Module, LPCWCH procName);
	
	FARPROC							GetRemoteProcAddressA(LPCCH module, SHORT procOrdinal);
	FARPROC							GetRemoteProcAddressW(LPCWCH module, SHORT procOrdinal);

	FARPROC							GetRemoteProcAddressA(LPCCH module, LPCCH procName);
	FARPROC							GetRemoteProcAddressW(LPCWCH module, LPCWCH procName);

protected:
	IMAGE_DOS_HEADER*				ToDos(PVOID BaseAddress);
	IMAGE_NT_HEADERS*				ToNts(PVOID BaseAddress);
	PVOID							RvaToPointer( ULONG RVA, PVOID BaseAddress );

	BOOL							CallEntryPoint( PVOID BaseAddress, FARPROC Entrypoint );

	bool							CreateActx(PVOID BaseAddress);

	bool							CreateTempManifestFileFromMemory(PVOID BaseAddress, DWORD ResourceId);
	DWORD							GetEmbeddedManifestResourceFromMemory(PVOID BaseAddress, DWORD ResourceId, void** Resource);

	NTSTATUS						EnableExceptions(PVOID BaseAddress, PVOID RemoteAddress, size_t ImageSize);
	DWORD							CreateVEH(size_t RemoteAddress /*= 0*/, size_t ImageSize /*= 0*/);

	bool							InitializeCookie(PVOID BaseAddress, PVOID RemoteAddress);

	BOOL							ProcessDelayedImportTable(PVOID BaseAddress, PVOID RemoteAddress );
	BOOL							ProcessImportTable( PVOID BaseAddress, PVOID RemoteAddress );
	BOOL							ProcessRelocation( size_t ImageBaseDelta, WORD Data, PBYTE RelocationBase );
	BOOL							ProcessRelocations( PVOID BaseAddress, PVOID RemoteAddress );
	BOOL							ProcessTlsEntries( PVOID BaseAddress, PVOID RemoteAddress );
	ULONG							GetSectionProtection( ULONG Characteristics );
	BOOL							ProcessSection( BYTE* Name, PVOID BaseAddress, PVOID RemoteAddress, ULONGLONG RawData, ULONGLONG VirtualAddress, ULONGLONG RawSize, ULONGLONG VirtualSize, ULONG ProtectFlag );
	BOOL							ProcessSections( PVOID BaseAddress, PVOID RemoteAddress, BOOL MapPEHeader );

private:
	ModuleFile						InitModuleFile(LPCCH FileName);
	BOOL							FreeModuleFile(ModuleFile Handle);

private:
	typedef std::map<std::wstring, std::vector<std::wstring>> MapApiSchema;
	MapApiSchema m_ApiSchemaMap;

	HANDLE m_hActx;

	// All credits go to DarthTon for his BlackBone project!
	//
	// Initialize api set map
	bool InitializeApiSchema()
	{
		if (Utils::IsWindowsVersionOrLater(Utils::Windows10))
			return InitializeP<PAPI_SET_NAMESPACE_ARRAY_10, PAPI_SET_NAMESPACE_ENTRY_10, PAPI_SET_VALUE_ARRAY_10, PAPI_SET_VALUE_ENTRY_10>();
		else if (Utils::IsWindowsVersionOrLater(Utils::Windows8))
			return InitializeP<PAPI_SET_NAMESPACE_ARRAY, PAPI_SET_NAMESPACE_ENTRY, PAPI_SET_VALUE_ARRAY, PAPI_SET_VALUE_ENTRY>();
		else if (Utils::IsWindowsVersionOrLater(Utils::Windows7))
			return InitializeP<PAPI_SET_NAMESPACE_ARRAY_V2, PAPI_SET_NAMESPACE_ENTRY_V2, PAPI_SET_VALUE_ARRAY_V2, PAPI_SET_VALUE_ENTRY_V2>();
		else
			return true;
	}

	// OS dependent api set initialization
	template<typename T1, typename T2, typename T3, typename T4>
	bool InitializeP()
	{
		if (!m_ApiSchemaMap.empty())
			return true;

		PEB_T *ppeb = reinterpret_cast<PEB_T*>(reinterpret_cast<TEB_T*>(NtCurrentTeb())->ProcessEnvironmentBlock);
		T1 pSetMap = reinterpret_cast<T1>(ppeb->ApiSetMap);
 
		for (DWORD i = 0; i < pSetMap->Count; i++)
		{
			T2 pDescriptor = pSetMap->entry(i);

			std::vector<std::wstring> vhosts;
			wchar_t dllName[MAX_PATH] = { 0 };

			pSetMap->apiName(pDescriptor, dllName);
			std::transform(dllName, dllName + MAX_PATH, dllName, ::tolower);

			T3 pHostData = pSetMap->valArray(pDescriptor);

			for (DWORD j = 0; j < pHostData->Count; j++)
			{
				T4 pHost = pHostData->entry(pSetMap, j);
				std::wstring hostName((wchar_t*)((BYTE*)pSetMap + pHost->ValueOffset), pHost->ValueLength / sizeof(wchar_t));

				if (!hostName.empty())
					vhosts.push_back(hostName);
			}

			m_ApiSchemaMap.insert(std::make_pair(dllName, vhosts));
		}

		return true;
	}

	// Resolve path flags
	enum eResolveFlag
	{
		Default = 0,
		ApiSchemaOnly = 1,
		EnsureFullPath = 2,
	};

	// From DarthTon's BlackBone
	// Resolve dll path
	DWORD ResolvePath(std::wstring& path, eResolveFlag flags, std::wstring& baseName, HMODULE* ppReturnModule = NULL)
	{
		wchar_t tmpPath[4096] = { 0 };
		std::wstring completePath;

		std::transform(path.begin(), path.end(), path.begin(), ::tolower);

		std::wstring filename = Utils::StripPath(path);

		//
		// ApiSchema redirection
		//
		auto iter = m_ApiSchemaMap.end();
		for (auto it = m_ApiSchemaMap.begin(); it != m_ApiSchemaMap.end(); ++it)
		{
			std::wstring strApiSchemaFile = it->first.c_str();
			// 'ext-ms-' and 'api-ms-' are resolved the same way
			strApiSchemaFile.erase(0, 4);

			//DbgShout("%ls == %ls", it->first.c_str(), filename.c_str());
			if (wcsstr(filename.c_str(), strApiSchemaFile.c_str()) != 0)
			{
				iter = it;
				break;
			}		
		}

		//auto iter = m_ApiSchemaMap.find(filename);
		if (iter != m_ApiSchemaMap.end())
		{
			//
			// 1) Select appropriate api host and find SxS redirect
			//
			path = iter->second.front() != baseName ? iter->second.front() : iter->second.back();
			if (ProbeSxSRedirect(path) == STATUS_SUCCESS)
			{
				return STATUS_SUCCESS;
			}

			//
			// 2) Ensure there is a full path by returning the filename in the system path
			//	  Usually resolved to kernelbase.dll or kernel32.dll, so return STATUS_IMAGE_ALREADY_LOADED
			else if (flags & EnsureFullPath)
			{
				baseName.assign(path.begin(), path.end());
				HMODULE mod = GetRemoteModuleHandleW(baseName.c_str());
				if (mod)
				{
					if (ppReturnModule)
						*ppReturnModule = mod;
					return STATUS_IMAGE_ALREADY_LOADED;	
				}

				wchar_t sys_path[255] = { 0 };
				GetSystemDirectoryW(sys_path, 255);
				std::wstring strSystemPath(sys_path); 
				strSystemPath += L'\\';
				path = strSystemPath + path;			
			}

			return STATUS_SUCCESS;
		}

		if (flags & ApiSchemaOnly)
		{
			SetLastError(ERROR_NOT_FOUND);
			return ERROR_NOT_FOUND;
		}

		// SxS redirection
		if (ProbeSxSRedirect(path) == ERROR_SUCCESS)
			return ERROR_SUCCESS;

		//
		// Perform search accordingly to Windows Image loader search order 
		// 1. KnownDlls
		//
		HKEY hKey = NULL;
		LRESULT res = 0;
		res = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\KnownDLLs", &hKey);

		if (res == 0)
		{
			for (int i = 0; i < 0x1000 && res == ERROR_SUCCESS; i++)
			{
				wchar_t value_name[255] = { 0 };
				wchar_t value_data[255] = { 0 };

				DWORD dwSize = 255;
				DWORD dwType = 0;

				res = RegEnumValueW(hKey, i, value_name, &dwSize, NULL, &dwType, (LPBYTE)value_data, &dwSize);

				if (_wcsicmp(value_data, filename.c_str()) == 0)
				{
					wchar_t sys_path[255] = { 0 };
					dwSize = 255;
	
					// In Win10 DllDirectory value got screwed, so less reliable method is used
					GetSystemDirectoryW(sys_path, dwSize);

					if (res == ERROR_SUCCESS)
					{
						path = std::wstring(sys_path) + L"\\" + value_data;

						RegCloseKey(hKey);
						return ERROR_SUCCESS;
					}
				}
			}

			RegCloseKey(hKey);
		}

		//
		// 2. The directory from which the application loaded.
		//
		completePath = Utils::GetProcessDirectory(m_hProcess) + L"\\" + filename;
		if (Utils::FileExists(completePath))
		{
			path = completePath;
			return ERROR_SUCCESS;
		}

		//
		// 3. The system directory
		//
		GetSystemDirectoryW(tmpPath, ARRAYSIZE(tmpPath));
		completePath = std::wstring(tmpPath) + L"\\" + filename;
		if (Utils::FileExists(completePath))
		{
			path = completePath;
			return ERROR_SUCCESS;
		}

		//
		// 4. The Windows directory
		//
		GetWindowsDirectoryW(tmpPath, ARRAYSIZE(tmpPath));
		completePath = std::wstring(tmpPath) + L"\\" + filename;
		if (Utils::FileExists(completePath))
		{
			path = completePath;
			return ERROR_SUCCESS;
		}

		//
		// 5. The current directory
		//
		GetCurrentDirectoryW(ARRAYSIZE(tmpPath), tmpPath);
		completePath = std::wstring(tmpPath) + L"\\" + filename;
		if (Utils::FileExists(completePath))
		{
			path = completePath;
			return ERROR_SUCCESS;
		}

		//
		// 6. The directories listed in the PATH environment variable
		//
		GetEnvironmentVariableW(L"PATH", tmpPath, ARRAYSIZE(tmpPath));
		wchar_t *pContext;

		for (wchar_t *pDir = wcstok_s(tmpPath, L";", &pContext); pDir; pDir = wcstok_s(pContext, L";", &pContext))
		{
			completePath = std::wstring(pDir) + L"\\" + filename;
			if (Utils::FileExists(completePath))
			{
				path = completePath;
				return ERROR_SUCCESS;
			}
		}

		SetLastError(ERROR_NOT_FOUND);
		return ERROR_NOT_FOUND;
	}

	/*
	Try SxS redirection
	*/
	DWORD ProbeSxSRedirect(std::wstring& path)
	{
		UNICODE_STRING OriginalName = { 0 };
		UNICODE_STRING DllName1 = { 0 };
		UNICODE_STRING DllName2 = { 0 };
		PUNICODE_STRING pPath = nullptr;
		ULONG_PTR cookie = 0;
		wchar_t wBuf[255];

		static HMODULE hNtdll = Utils::GetLocalModuleHandle("ntdll.dll");
		static tRtlInitUnicodeString RtlInitUnicodeString = (tRtlInitUnicodeString)Utils::GetProcAddress(hNtdll, "RtlInitUnicodeString");
		static tRtlFreeUnicodeString RtlFreeUnicodeString = (tRtlFreeUnicodeString)Utils::GetProcAddress(hNtdll, "RtlFreeUnicodeString");
		static tRtlNtStatusToDosError RtlNtStatusToDosError = (tRtlNtStatusToDosError)Utils::GetProcAddress(hNtdll, "RtlNtStatusToDosError");
		static tRtlDosApplyFileIsolationRedirection_Ustr RtlDosApplyFileIsolationRedirection_Ustr = (tRtlDosApplyFileIsolationRedirection_Ustr)Utils::GetProcAddress(hNtdll, "RtlDosApplyFileIsolationRedirection_Ustr");

		RtlInitUnicodeString(&OriginalName, path.c_str());

		DllName1.Buffer = wBuf;
		DllName1.Length = NULL;
		DllName1.MaximumLength = sizeof(wBuf);

		// Use activation context
		if (m_hActx != INVALID_HANDLE_VALUE)
			ActivateActCtx(m_hActx, &cookie);

		// SxS resolve
		NTSTATUS status = RtlDosApplyFileIsolationRedirection_Ustr( TRUE, &OriginalName, (PUNICODE_STRING)NULL,
			&DllName1, &DllName2, &pPath,
			nullptr, nullptr, nullptr
		);

		if (cookie != 0 && m_hActx != INVALID_HANDLE_VALUE)
			DeactivateActCtx(0, cookie);

		if (status == STATUS_SUCCESS)
		{
			path = pPath->Buffer;
		}
		else
		{
			if (DllName2.Buffer)
				RtlFreeUnicodeString(&DllName2);
		}

		SetLastError(RtlNtStatusToDosError(status));
		return status;
	}

};



#endif