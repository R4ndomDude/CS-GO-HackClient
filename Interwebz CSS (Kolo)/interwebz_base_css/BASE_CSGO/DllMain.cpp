////////////////////////////////
/*
SOURCE Base by rifk kolo & gt3x
CREDITS: floesen,outline,inurface
Init - 22.08-2014
*/
///////////////////////////////
#include "DllMain.h"

CValve g_Valve;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

static HMODULE DllBaseAddress() // works with manually mapped dlls (will get us our HMODULE)
{
	MEMORY_BASIC_INFORMATION info;
	size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)DllBaseAddress, &info, sizeof(info));
	Assert(len == sizeof(info));
	return len ? (HMODULE)info.AllocationBase : NULL;
}

void InitDllThread(void* ptr)
{	
	g_Valve.initSDK();

	g_NetworkedVariableManager.Init();

#ifdef DEBUGMODE
	Base::Debug::LOG("Netvars initialized");
#endif

	g_pClientVMT.bInitialize((PDWORD*)g_Valve.pClient);
	dwOriginCreateMove = g_pClientVMT.dwGetMethodAddress(23);
	g_pClientVMT.dwHookMethod((DWORD)hkdCreateMove, 23);

	g_Valve.pInput = (ValveSDK::CInput*)**(PDWORD*)(g_pClientVMT.dwGetMethodAddress(21) + INPUTOFFSET);

#ifdef DEBUGMODE
	char szDebugString[1024];
	sprintf(szDebugString, "g_pInput: 0x%08X ", (DWORD)g_Valve.pInput);
	Base::Debug::LOG(szDebugString);
#endif

	g_pInputVMT.bInitialize((PDWORD*)g_Valve.pInput);
	g_pInputVMT.dwHookMethod((DWORD)hkdGetUserCmd, 8);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked GetUserCmd...");
#endif

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked CreateMove...");
#endif

	g_pClientVMT.dwHookMethod((DWORD)hkdFrameStageNotify, 35);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked FrameStageNotify");
#endif

	g_pClientVMT.dwHookMethod((DWORD)hkdIN_KeyEvent, 20);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked IN_KeyEvent");
#endif

	g_pPredictionVMT.bInitialize((PDWORD*)g_Valve.pPred);
	g_pPredictionVMT.dwHookMethod((DWORD)hkdRunCommand, 17);

#ifdef DEBUGMODE
	sprintf(szDebugString,"runcommand: 0x%X",g_pPredictionVMT.dwGetMethodAddress(17));
	Base::Debug::LOG(szDebugString);
#endif

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked RunCommand");
#endif

	g_pPredictionVMT.dwHookMethod((DWORD)hkdFinishMove, 19);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked FinishMove");
#endif

	g_pModelRenderVMT.bInitialize((PDWORD*)g_Valve.pModelRender);
	g_pModelRenderVMT.dwHookMethod((DWORD)hkdDrawModelExecute,19);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked DrawModelExecute");
#endif

	g_pPanelVMT.bInitialize((PDWORD*)g_Valve.pPanel);
	g_pPanelVMT.dwHookMethod((DWORD)hkdPaintTraverse, 41);

#ifdef DEBUGMODE
	Base::Debug::LOG("hooked PaintTraverse");
#endif

	g_NetworkedVariableManager.HookProp(/*DT_CSPlayer*/XorStr<0xAA,12,0x6F938DD5>("\xEE\xFF\xF3\xEE\xFD\xFF\xDC\xD0\xCB\xD6\xC6"+0x6F938DD5).s, /*m_angEyeAngles[0]*/XorStr<0x7E,18,0x0D7A3609>("\x13\x20\xE1\xEF\xE5\xC6\xFD\xE0\xC7\xE9\xEF\xE5\xEF\xF8\xD7\xBD\xD3"+0x0D7A3609).s,PitchEyeAngleProxy);
	g_NetworkedVariableManager.HookProp(/*DT_CSPlayer*/XorStr<0xAA,12,0x6F938DD5>("\xEE\xFF\xF3\xEE\xFD\xFF\xDC\xD0\xCB\xD6\xC6"+0x6F938DD5).s, /*m_angEyeAngles[1]*/XorStr<0x1B,18,0x97DB0871>("\x76\x43\x7C\x70\x78\x65\x58\x47\x62\x4A\x42\x4A\x42\x5B\x72\x1B\x76"+0x97DB0871).s,YawEyeAngleProxy);

	//gGameEventManager.RegisterSelf();

	/*
	char path[260];
	HMODULE hm = NULL;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)&DllBaseAddress,
		&hm))
	{
		int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle returned %d\n", ret);
	}

	char strDLLPath1[MAX_PATH];
	//::GetModuleFileNameA(hm, strDLLPath1, _MAX_PATH);
	//LoadConfig(strDLLPath1);
	*/
}

/*void unhookThread(LPARAM lpParam)
{
	HMODULE hModule = (HMODULE)lpParam;
	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD9) & 1)
		{
			Sleep(1000);

			g_pClientVMT.UnHook();
			g_pPredictionVMT.UnHook();
			g_pPanelVMT.UnHook();

			Sleep(2000);

			FreeConsole();
			SendMessage(FindWindow(0, "DebugMessages"), WM_CLOSE, NULL, NULL);

			FreeLibraryAndExitThread(hModule, 0);
		}
	}
}*/

#include "ReflectiveLoader.h"
#include "ReflectiveDLLInjection.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
#ifdef DEBUGMODE
		Base::Debug::AttachDebugConsole();
#endif
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitDllThread, hinstDLL, NULL, NULL);

		g_CVARS.Init(hinstDLL);

		g_CVARS.SetDefaultConfig();

		//g_CVARS.CvarList[Triggerbot] = 0;

		/*HW_PROFILE_INFO hwProfileInfo;

		GetCurrentHwProfile(&hwProfileInfo);

		char szFile[2048];
		sprintf(szFile,"%s\\hwid.txt",g_CVARS.szIniFilePath);

		std::ofstream fNew(szFile,std::ios::app);
		fNew.write(hwProfileInfo.szHwProfileGuid,strlen(hwProfileInfo.szHwProfileGuid));
		fNew.close();*/

		//HandleConfig("flick.ini",LoadConfig);
	}
	else if(dwReason == DLL_QUERY_HMODULE && lpReserved != NULL)
		*(HMODULE*)lpReserved = hinstDLL;

	return TRUE;
}