#pragma once

#include "sdk.h"

void Util::PrintMessage(const char* szMsg, ...)
{
	typedef void(__cdecl* MsgFn)(const char* szMsg, va_list);
	static MsgFn Msg = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");

	char buf[989];
	va_list vlist;
	va_start(vlist, szMsg);
	_vsnprintf(buf, sizeof(buf), szMsg, vlist);
	va_end(vlist);
	sprintf(buf, "%s", buf);

	Msg(buf, vlist);
}

void Util::CaptureInterfaces()
{
	I::Client = Util::CaptureInterface<IBaseClientDll>("client.dll", "VClient017");
	I::ClientMode = **(IClientModeShared***)((*(DWORD**)I::Client)[10] + 0x5);
	I::ClientEntList = Util::CaptureInterface<IClientEntityList>("client.dll", "VClientEntityList003");
	I::Cvar = Util::CaptureInterface<ICVar>("vstdlib.dll", "VEngineCvar007");
	I::Engine = Util::CaptureInterface<IEngineClient>("engine.dll", "VEngineClient013");
	I::EngineTrace = Util::CaptureInterface<IEngineTrace>("engine.dll", "EngineTraceClient004");
	I::InputSystem = Util::CaptureInterface<IInputSystem>("inputsystem.dll", "InputSystemVersion001");
	I::Globals = **(IGlobalVarsBase***)((*(DWORD**)I::Client)[0] + 0x53);
	I::Surface = Util::CaptureInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface031");
	I::VPanel = Util::CaptureInterface<IVPanel>("vgui2.dll", "VGUI_Panel009");
}

void* Util::Hook(void* pInstance, int iIndex, void* pHookFunc)
{
	DWORD dwVTable = *(DWORD*)pInstance;
	DWORD dwVFunc = dwVTable + sizeof(DWORD) * iIndex;
	intptr_t pOrigFunc = *((DWORD*)dwVFunc);
	DWORD dwOldProtect;
	VirtualProtect((LPVOID)dwVFunc, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*((DWORD*)dwVFunc) = (DWORD)pHookFunc;
	VirtualProtect((LPVOID)dwVFunc, sizeof(DWORD), dwOldProtect, &dwOldProtect);

	return (void*)pOrigFunc;
}

void Util::Unhook(void* pInstance, int iIndex, void* pOriginal)
{
	DWORD dwVTable = *(DWORD*)pInstance;
	DWORD dwVFunc = dwVTable + sizeof(DWORD)* iIndex;
	intptr_t pOrigFunc = *((DWORD*)dwVFunc);
	DWORD dwOldProtect;
	VirtualProtect((LPVOID)dwVFunc, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*((DWORD*)dwVFunc) = (DWORD)pOriginal;
	VirtualProtect((LPVOID)dwVFunc, sizeof(DWORD), dwOldProtect, &dwOldProtect);
}

bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
{
	for (; *szMask; ++szMask, ++Mask, ++Data)
	{
		if (*szMask == 'x' && *Mask != *Data)
		{
			return false;
		}
	}
	return (*szMask) == 0;
}

DWORD Util::FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
{
	DWORD dwAddress = (DWORD)GetModuleHandleA(moduleName.c_str());

	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)dwAddress, &miModInfo, sizeof(MODULEINFO));
	DWORD dwLength = miModInfo.SizeOfImage;
	for (DWORD i = 0; i < dwLength; i++)
	{
		if (bCompare((BYTE*)(dwAddress + i), Mask, szMask))
		{
			return (DWORD)(dwAddress + i);
		}
	}
	return 0;
}

CBaseEntity* Util::GetLocalPlayer()
{
	return I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());
}

CNetVarManager* Util::NetVars = new CNetVarManager;

void Util::SpoofCvar(ConVar* pCvar)
{
	
}

unsigned short UTIL_GetAchievementEventMask()
{
	static uintptr_t GetAchievementEventMask_func = Util::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????xx????x????");
	typedef unsigned short(__fastcall* UTIL_GetAchievementEventMask_t)();
	static UTIL_GetAchievementEventMask_t pUTIL_GetAchievementEventMask = 
		reinterpret_cast<UTIL_GetAchievementEventMask_t>(GetAchievementEventMask_func);
	return pUTIL_GetAchievementEventMask();
}