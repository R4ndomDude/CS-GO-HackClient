#pragma once

#include "config.h"
#include "netvars.h"
#include "color.h"
#include "offsets.h"
#include "aim.h"
#include "draw.h"
#include "input.h"
#include "fixmove.h"
#include "autowall.h"
#include "skinsutil.h"

#define RandomInt(min, max) (rand() % (max - min + 1) + min)

namespace Util
{
	extern void PrintMessage(const char* szMsg, ...);
	template <typename T>
	T* CaptureInterface(std::string strModule, std::string strInterface)
	{
		typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule.c_str()), "CreateInterface");

		return CreateInterface(strInterface.c_str(), 0);
	}
	extern void CaptureInterfaces();
	extern void* Hook(void* pInstance, int iIndex, void* pHookFunc);
	extern void Unhook(void* pInstance, int iIndex, void* pHookFunc);
	extern DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);
	template <typename T> 
	T GetVFunc(void *vTable, int iIndex) {
		return (*(T**)vTable)[iIndex];
	}
	extern CBaseEntity* GetLocalPlayer();
	extern CNetVarManager* NetVars;
	template< class T, class Y >
	T Clamp(T const &val, Y const &minVal, Y const &maxVal)
	{
		if (val < minVal)
			return minVal;
		else if (val > maxVal)
			return maxVal;
		else
			return val;
	}
	extern void SpoofCvar(ConVar* pCvar);
}

extern unsigned short UTIL_GetAchievementEventMask();