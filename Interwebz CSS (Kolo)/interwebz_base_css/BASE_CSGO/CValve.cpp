#include "DllMain.h"

ValveSDK::CBaseCombatWeapon* CBaseEntity::GetActiveBaseCombatWeapon()
{
	static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0x8F,12,0xB341DDC2>("\xCB\xC4\xCE\xD1\xC0\xC4\xF9\xF7\xEE\xFD\xEB"+0xB341DDC2).s,/*m_hActiveWeapon*/XorStr<0xA4,16,0x547EFF2E>("\xC9\xFA\xCE\xE6\xCB\xDD\xC3\xDD\xC9\xFA\xCB\xCE\xC0\xDE\xDC"+0x547EFF2E).s);
	ULONG pWeepEhandle = *(PULONG)((DWORD)this + iOffset);
	return (ValveSDK::CBaseCombatWeapon*)(g_Valve.pEntList->GetClientEntityFromHandle(pWeepEhandle));
}

void* GetInterfacePtr(const char* interfaceName, const char* ptrName, CreateInterface_t pInterface)
{
#ifdef DEBUGMODE
	char szDebugString[1024];
#endif
	std::string sinterface = "";
	std::string interfaceVersion = /*0*/XorStr<0xBF,2,0xE204B62B>("\x8F"+0xE204B62B).s;

	for (int i = 0; i <= 99; i++)
	{
		sinterface = interfaceName;
		sinterface += interfaceVersion;
		sinterface += std::to_string(i);

		void* funcPtr = pInterface(sinterface.c_str(), NULL);

		if ((DWORD)funcPtr != 0x0)
		{
#ifdef DEBUGMODE
			sprintf(szDebugString, "%s: 0x%x (%s%s%i)", ptrName, (DWORD)funcPtr, interfaceName, interfaceVersion.c_str(), i);
			Base::Debug::LOG(szDebugString);
#endif
			return funcPtr;
		}
		if (i >= 99 && interfaceVersion == /*0*/XorStr<0xFE,2,0x8291FF95>("\xCE"+0x8291FF95).s)
		{
			interfaceVersion = /*00*/XorStr<0x79,3,0x0494E7DB>("\x49\x4A"+0x0494E7DB).s;
			i = 0;
		}
		else if (i >= 99 && interfaceVersion == /*00*/XorStr<0x79,3,0x0494E7DB>("\x49\x4A"+0x0494E7DB).s)
		{
#ifdef DEBUGMODE
			sprintf(szDebugString, "%s: 0x%x (error)", ptrName, (DWORD)funcPtr);
			Base::Debug::LOG(szDebugString);
#endif
		}
	}
	return 0;
}

VOID CValve::initSDK()
{
#ifdef DEBUGMODE
	Base::Debug::LOG("Initiate Thread");
	char szDebugString[1024];
#endif

	//DWORD dwInitAddress = Base::Utils::PatternSearch(/*engine.dll*/XorStr<0x3F,11,0x78446C3F>("\x5A\x2E\x26\x2B\x2D\x21\x6B\x22\x2B\x24"+0x78446C3F).s,(PBYTE)"\x56\xE8\x00\x00\x00\x00\x83\x3D\x00\x00\x00\x00\x00",/*xx????xx?????*/XorStr<0xD8,14,0x0449C8E4>("\xA0\xA1\xE5\xE4\xE3\xE2\xA6\xA7\xDF\xDE\xDD\xDC\xDB"+0x0449C8E4).s,NULL,NULL);

#ifdef DEBUGMODE
	//sprintf(szDebugString, "dwInitAddress: 0x%x", dwInitAddress);
	//Base::Debug::LOG(szDebugString);
#endif

	//CreateInterfaceFn AppSystemFactory = (CreateInterfaceFn)**(PDWORD*)(dwInitAddress + APPSYSTEMFACTORYOFFSET);	
				
	CreateInterfaceFn ClientFactory = (CreateInterfaceFn)**(PDWORD*)(dwInitAddress + CLIENTFACTORYOFFSET);	

	//DWORD dwCLMoveAddress = Base::Utils::PatternSearch( /*engine.dll*/XorStr<0xA1,11,0xE75EB804>("\xC4\xCC\xC4\xCD\xCB\xC3\x89\xCC\xC5\xC6"+0xE75EB804).s,(PBYTE)"\x55\x8B\xEC\x83\xEC\x4C\x53\x56\x57\x8B\x3D",/*xxxxxxxxxxx*/XorStr<0xEF,12,0xC7505732>("\x97\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x80\x81"+0xC7505732).s,NULL,NULL);

	pClient = (ValveSDK::HLCLient*)GetInterfacePtr(/*VClient*/XorStr<0xAF,8,0x968D4BC3>("\xF9\xF3\xDD\xDB\xD6\xDA\xC1"+0x968D4BC3).s, /*g_pClient*/XorStr<0x0C,10,0xD5C0FAD9>("\x6B\x52\x7E\x4C\x7C\x78\x77\x7D\x60"+0xD5C0FAD9).s, ClientFactory);
	//pEntList = (ValveSDK::CEntityList*)GetInterfacePtr(/*VClientEntityList*/XorStr<0xD9,18,0x06C34A7E>("\x8F\x99\xB7\xB5\xB8\xB0\xAB\xA5\x8F\x96\x8A\x90\x9C\xAA\x8E\x9B\x9D"+0x06C34A7E).s, /*g_pEntList*/XorStr<0x4E,11,0x4F329B8F>("\x29\x10\x20\x14\x3C\x27\x18\x3C\x25\x23"+0x4F329B8F).s, ClientFactory);
	//pPred = (ValveSDK::CPrediction*)GetInterfacePtr(/*VClientPrediction*/XorStr<0x08,18,0x5594458A>("\x5E\x4A\x66\x62\x69\x63\x7A\x5F\x62\x74\x76\x7A\x77\x61\x7F\x78\x76"+0x5594458A).s, /*g_pPred*/XorStr<0xC8,8,0x048281FC>("\xAF\x96\xBA\x9B\xBE\xA8\xAA"+0x048281FC).s, ClientFactory);
	//pGameMovement = (ValveSDK::CGameMovement*)GetInterfacePtr(/*GameMovement*/XorStr<0x6C,13,0xBA6B4994>("\x2B\x0C\x03\x0A\x3D\x1E\x04\x16\x19\x10\x18\x03"+0xBA6B4994).s, /*g_pGameMovement*/XorStr<0xE3,16,0xA149CDDD>("\x84\xBB\x95\xA1\x86\x85\x8C\xA7\x84\x9A\x88\x83\x8A\x9E\x85"+0xA149CDDD).s, ClientFactory);
	//pEngine = (ValveSDK::CEngineClient*)GetInterfacePtr(/*VEngineClient*/XorStr<0x0F,14,0xECD749D0>("\x59\x55\x7F\x75\x7A\x7A\x70\x55\x7B\x71\x7C\x74\x6F"+0xECD749D0).s, /*g_pEngine*/XorStr<0xB8,10,0x06F99669>("\xDF\xE6\xCA\xFE\xD2\xDA\xD7\xD1\xA5"+0x06F99669).s, AppSystemFactory);
	//pModel = (ValveSDK::CModelInfo*)GetInterfacePtr(/*VModelInfoClient*/XorStr<0x45,17,0x10AE6F86>("\x13\x0B\x28\x2C\x2C\x26\x02\x22\x2B\x21\x0C\x3C\x38\x37\x3D\x20"+0x10AE6F86).s, /*g_pModel*/XorStr<0x62,9,0x7A9B9A92>("\x05\x3C\x14\x28\x09\x03\x0D\x05"+0x7A9B9A92).s, AppSystemFactory);
	//pDebugOverlay = (ValveSDK::CDebugOverlay*)GetInterfacePtr(/*VDebugOverlay*/XorStr<0x7C,14,0x84B69AE6>("\x2A\x39\x1B\x1D\xF5\xE6\xCD\xF5\xE1\xF7\xEA\xE6\xF1"+0x84B69AE6).s, /*g_pDebugOverlay*/XorStr<0xA4,16,0xE3165656>("\xC3\xFA\xD6\xE3\xCD\xCB\xDF\xCC\xE3\xDB\xCB\xDD\xDC\xD0\xCB"+0xE3165656).s, AppSystemFactory);
	//pEngineTrace = (ValveSDK::CTrace*)GetInterfacePtr(/*EngineTraceClient*/XorStr<0x63,18,0x9F57E38B>("\x26\x0A\x02\x0F\x09\x0D\x3D\x18\x0A\x0F\x08\x2D\x03\x19\x14\x1C\x07"+0x9F57E38B).s, /*g_pEngineTrace*/XorStr<0x9B,15,0x979D2673>("\xFC\xC3\xED\xDB\xF1\xC7\xC8\xCC\xC6\xF0\xD7\xC7\xC4\xCD"+0x979D2673).s, AppSystemFactory);	
	//pSurface = (ValveSDK::ISurface*)GetInterfacePtr(/*VGUI_Surface*/XorStr<0xD8,13,0xA16C3D37>("\x8E\x9E\x8F\x92\x83\x8E\xAB\xAD\x86\x80\x81\x86"+0xA16C3D37).s, /*g_pSurface*/XorStr<0x1A,11,0x8765E1C4>("\x7D\x44\x6C\x4E\x6B\x6D\x46\x40\x41\x46"+0x8765E1C4).s, AppSystemFactory);
	//pPanel = (ValveSDK::IPanel*)GetInterfacePtr(/*VGUI_Panel*/XorStr<0x11,11,0x7A40E632>("\x47\x55\x46\x5D\x4A\x46\x76\x76\x7C\x76"+0x7A40E632).s, /*g_pPanel*/XorStr<0xB4,9,0x01C7FA4D>("\xD3\xEA\xC6\xE7\xD9\xD7\xDF\xD7"+0x01C7FA4D).s, AppSystemFactory);
	//pPhysics = (ValveSDK::IPhysicsSurfaceProps*)GetInterfacePtr(/*VPhysicsSurfaceProps*/XorStr<0x7A,21,0x2D8FF92E>("\x2C\x2B\x14\x04\x0D\x16\xE3\xF2\xD1\xF6\xF6\xE3\xE7\xE4\xED\xD9\xF8\xE4\xFC\xFE"+0x2D8FF92E).s,/*g_pPhysics*/XorStr<0x36,11,0xEC4BCCBD>("\x51\x68\x48\x69\x52\x42\x4F\x54\x5D\x4C"+0xEC4BCCBD).s, AppSystemFactory);
	//pModelRender  = (ValveSDK::IVModelRender*)GetInterfacePtr(/*VEngineModel*/XorStr<0xCB,13,0x8FEE9BDA>("\x9D\x89\xA3\xA9\xA6\xBE\xB4\x9F\xBC\xB0\xB0\xBA"+0x8FEE9BDA).s,/*g_pModelRender*/XorStr<0x28,15,0x592E98CF>("\x4F\x76\x5A\x66\x43\x49\x4B\x43\x62\x54\x5C\x57\x51\x47"+0x592E98CF).s,AppSystemFactory);
	//pRenderView = (ValveSDK::IVRenderView*)GetInterfacePtr(/*VEngineRenderView*/XorStr<0xB4,18,0xAD1665A7>("\xE2\xF0\xD8\xD0\xD1\xD7\xDF\xE9\xD9\xD3\xDA\xDA\xB2\x97\xAB\xA6\xB3"+0xAD1665A7).s,/*g_pRenderView*/XorStr<0x0D,14,0xBA1E3FE5>("\x6A\x51\x7F\x42\x74\x7C\x77\x71\x67\x40\x7E\x7D\x6E"+0xBA1E3FE5).s,AppSystemFactory);
	//pMaterialSystem = (ValveSDK::IMaterialSystem*)GetInterfacePtr(/*VMaterialSystem*/XorStr<0x07,16,0x9D711B90>("\x51\x45\x68\x7E\x6E\x7E\x64\x6F\x63\x43\x68\x61\x67\x71\x78"+0x9D711B90).s,/*g_pMaterialSystem*/XorStr<0xD7,18,0xC2404C57>("\xB0\x87\xA9\x97\xBA\xA8\xB8\xAC\xB6\x81\x8D\xB1\x9A\x97\x91\x83\x8A"+0xC2404C57).s,AppSystemFactory);
	//pConVar = (ValveSDK::ICvar*)GetInterfacePtr(/*VEngineCvar*/XorStr<0x0F,12,0xCAB92F1C>("\x59\x55\x7F\x75\x7A\x7A\x70\x55\x61\x79\x6B"+0xCAB92F1C).s,/*g_pConVar*/XorStr<0xB1,10,0xFC444677>("\xD6\xED\xC3\xF7\xDA\xD8\xE1\xD9\xCB"+0xFC444677).s,AppSystemFactory);
	//pGameEventManager = (ValveSDK::IGameEventManager*)AppSystemFactory(/*GAMEEVENTSMANAGER002*/XorStr<0x75,21,0xAAE7DB93>("\x32\x37\x3A\x3D\x3C\x2C\x3E\x32\x29\x2D\x32\xC1\xCF\xC3\xC4\xC1\xD7\xB6\xB7\xBA"+0xAAE7DB93).s,NULL);*/
	
#ifdef DEBUGMODE
	sprintf(szDebugString, "g_pGameEventManager: 0x%x", (DWORD)pGameEventManager);
	Base::Debug::LOG(szDebugString);
#endif

	pGlobalVars = *(ValveSDK::CGlobalVars**)(((*(PDWORD*)g_Valve.pClient)[0]) + GLOBALSOFFSET);		
			
	pGlobalVars = (ValveSDK::CGlobalVars*)*(PDWORD)pGlobalVars;

#ifdef DEBUGMODE
	sprintf(szDebugString, "g_pGlobalVars: 0x%x", (DWORD)pGlobalVars);
	Base::Debug::LOG(szDebugString);
#endif

	m_bInitiated = TRUE;
}

BOOL CValve::isInitiated()
{
	return this->m_bInitiated;
}

bool CValve::WorldToScreen(const Vector &vOrigin, Vector &vScreen)
{
	return (pDebugOverlay->ScreenPosition(vOrigin, vScreen) != 1);
}