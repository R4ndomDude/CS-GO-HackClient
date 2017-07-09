#include "SDK.h"
#include "NetVars.h"
#include "Drawings.h"
#include "Misc.h"
#include "Player.h"
#include "Aimbot.h"
#include "CVars.h"

bool __fastcall Hooked_CreateMove(void* ecx, void* edx, float frametime, CUserCmd* cmd)
{
	if(cmd->command_number == 0)
		return false;

	CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
	if(!pLocal)
		return false;

	if(pLocal->GetLifeState() != LIFE_ALIVE)
		return false;

	if(Hack.CVars->aimbot_active)
		Hack.Aimbot->Main(cmd, frametime);

	if(Hack.CVars->misc_bunnyhop)
	{
		static bool bLastJumped = false;
		static bool bShouldFake = false;

		if(!bLastJumped && bShouldFake)
		{
			bShouldFake = false;
			cmd->buttons |= IN_JUMP;
			//cmd->buttons |= IN_DUCK; //mhm
		}
		else if(cmd->buttons & IN_JUMP)
		{
			if(pLocal->GetFlags() & FL_ONGROUND)
			{
				bLastJumped = true;
				bShouldFake = true;
			}
			else
			{
				cmd->buttons &= ~IN_JUMP;
				bLastJumped = false;
			}
		}
		else
		{
			bLastJumped = false;
			bShouldFake = false;
		}
	}

	if(Hack.CVars->misc_autostrafer)
	{
		static float move = 400.f;
		float s_move = move * (pLocal->GetVelocity().Length() / 1000.f);

		if(GetAsyncKeyState(VK_SPACE) && !(pLocal->GetMoveType() & MOVETYPE_NOCLIP || pLocal->GetMoveType() & MOVETYPE_LADDER)) //(cmd->buttons & IN_JUMP || !pLocal->GetFlags() & FL_ONGROUND)  && !(cmd->buttons & IN_ATTACK)
		{
			cmd->forwardmove = move * 0.015f;
			cmd->sidemove += (float)(((cmd->tick_count % 2) * 2) - 1) * s_move;

			if(cmd->mousedx)
				cmd->sidemove = (float)Hack.Misc->Clamp(cmd->mousedx, -1, 1) * s_move;

			static float strafe = cmd->viewangles.y;

			float rt = cmd->viewangles.y, rotation;
			rotation = strafe - rt;

			if(rotation < -Interfaces.Globals->interval_per_tick)
				cmd->sidemove = -s_move;

			if(rotation > Interfaces.Globals->interval_per_tick)
				cmd->sidemove = s_move;

			strafe = rt;
		}
	}

	return false;
}

void __fastcall Hooked_PaintTraverse(void* ecx, void* edx, unsigned int VGUIPanel, bool forcerepaint, bool allowforce)
{
	Hack.PanelHook->GetMethod<PaintTraverseFn>(41)(ecx, VGUIPanel, forcerepaint, allowforce);

	if(GetAsyncKeyState(VK_F4) & 0x1)
		Hack.CVars->Load();

	static unsigned int panel = 0;
	if(!panel)
	{
		std::string szPanelName = Interfaces.Panel->GetName(VGUIPanel);
		if(szPanelName.find("FocusOverlayPanel") != std::string::npos) //FocusOverlayPanel - MatSystemTopPanel
			panel = VGUIPanel;
	}

	if(panel && VGUIPanel == panel)
	{
		Hack.Drawings->DrawString(5, 5, Color::White(), false, "CSGO Internal");

		if(Hack.CVars->esp_active)
		{
			if(Interfaces.Engine->IsInGame() && Interfaces.Engine->IsConnected())
			{
				CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
				if(!pLocal)
					return;

				for(int i = 1; i <= Interfaces.Engine->GetMaxClients(); i++)
				{
					CPlayer* pPlayer = Interfaces.EntList->GetClientEntity<CPlayer>(i);
					if(!pPlayer->IsValid(pLocal))
						continue;

					Color clrTeam = Color(255, 255, 255, 255);
					if(Hack.Misc->IsVisible(pLocal->GetEyePosition(), pPlayer->GetEyePosition(), pLocal, pPlayer)) //visible
					{
						if(pPlayer->GetTeam() == 2)
							clrTeam = Color::Yellow();
						else if(pPlayer->GetTeam() == 3)
							clrTeam = Color::Green();
					}
					else //invisible
					{
						if(pPlayer->GetTeam() == 2)
							clrTeam = Color::Red();
						else if(pPlayer->GetTeam() == 3)
							clrTeam = Color::Blue();
					}

					player_info_t info;
					if(!Interfaces.Engine->GetPlayerInfo(i, &info))
						continue;

					Hack.Drawings->DynamicBox(pPlayer, info.name, clrTeam);
				}
			}
		}
	}
}

void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	setup.fovViewmodel = 90.0f;

	CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
	
	if(!pLocal->IsScoped())
		setup.fov = 120.0f;

	//setup.origin = pLocal->GetHitboxPosition(1); //lawl

	Hack.ViewRenderHook->GetMethod<RenderViewFn>(6)(ecx, setup, hudViewSetup, nClearFlags, whatToDraw);
}

void __fastcall Hooked_DrawModelExecute(void* ecx, void* edx, IMatRenderContext* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
{
	Hack.ModelRenderHook->Unhook();

	if(Hack.CVars->chams_active)
	{
		IMaterial* mat_ignorez = Hack.CVars->chams_flat ? Hack.ChamsMat_Flat_IgnoreZ : Hack.ChamsMat_IgnoreZ;
		IMaterial* mat = Hack.CVars->chams_flat ? Hack.ChamsMat_Flat : Hack.ChamsMat;

		if(pInfo.pModel)
		{
			std::string szModelName = Interfaces.ModelInfo->GetModelName(pInfo.pModel);

			if(szModelName.find("models/player/") != std::string::npos)
			{
				CPlayer* pModelPlayer = Interfaces.EntList->GetClientEntity<CPlayer>(pInfo.entity_index);
				if(pModelPlayer->IsValid(Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer())))
				{
					if(pModelPlayer->GetTeam() == 2)
					{
						if(!Hack.CVars->chams_visibleonly)
						{
							Hack.Misc->ForceMaterial(mat_ignorez, fColor::Chams_Red());
							Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);
						}

						Hack.Misc->ForceMaterial(mat, fColor::Chams_Yellow());
						Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);
					}
					else if(pModelPlayer->GetTeam() == 3)
					{
						if(!Hack.CVars->chams_visibleonly)
						{
							Hack.Misc->ForceMaterial(mat_ignorez, fColor::Chams_Blue());
							Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);
						}

						Hack.Misc->ForceMaterial(mat, fColor::Chams_Green());
						Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);
					}
				}
			}

			/*if(szModelName.find("models/weapons/w_") != std::string::npos)
			{
				Hack.Misc->ForceMaterial(Hack.ChamsMat_IgnoreZ, fColor::White());
				Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);

				Hack.Misc->ForceMaterial(Hack.ChamsMat, fColor::White());
				Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);
			}*/
		}
	}

	Interfaces.ModelRender->DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);

	Interfaces.ModelRender->ForcedMaterialOverride(nullptr);

	Hack.ModelRenderHook->Rehook();
}

void Hooked_nModelIndex(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	int* hz = reinterpret_cast<int*>(reinterpret_cast<DWORD>(pData) + 0x8);

	if(*hz == Interfaces.ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl") || *hz == Interfaces.ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl"))
		*hz = Interfaces.ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");

	Hack.oModelIndex(pData, pStruct, pOut);
}

void Hooked_iItemDefinitionIndex(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	int* hz = reinterpret_cast<int*>(reinterpret_cast<DWORD>(pData) + 0x8);

	if(*hz == WEAPON_KNIFE_T || *hz == WEAPON_KNIFE)
		*hz = WEAPON_KNIFE_M9_BAYONET;

	Hack.oItemDefinitionIndex(pData, pStruct, pOut);
}

void Init(HMODULE hModule)
{
	//AllocConsole();
	//AttachConsole(GetCurrentProcessId());
	//freopen("CON", "w", stdout);

	CreateInterfaceFn ClientFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface"));
	CreateInterfaceFn EngineFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface"));
	CreateInterfaceFn ServerFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("server.dll"), "CreateInterface"));
	CreateInterfaceFn VGUI2Factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vgui2.dll"), "CreateInterface"));
	CreateInterfaceFn MatSurfaceFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vguimatsurface.dll"), "CreateInterface"));
	CreateInterfaceFn MatSystemFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("materialsystem.dll"), "CreateInterface"));

	Interfaces.Client = static_cast<IBaseClientDLL*>(ClientFactory("VClient017", nullptr));
	Interfaces.EntList = static_cast<IClientEntityList*>(ClientFactory("VClientEntityList003", nullptr));
	Interfaces.GameMovement = static_cast<IGameMovement*>(ClientFactory("GameMovement001", nullptr));
	Interfaces.Engine = static_cast<IVEngineClient*>(EngineFactory("VEngineClient013", nullptr));
	Interfaces.EngineTrace = static_cast<IEngineTrace*>(EngineFactory("EngineTraceClient004", nullptr));
	Interfaces.ModelInfo = static_cast<IVModelInfoClient*>(EngineFactory("VModelInfoClient004", nullptr));
	Interfaces.ModelRender = static_cast<IVModelRender*>(EngineFactory("VEngineModel016", nullptr));
	//Interfaces.RenderView = static_cast<IVRenderView*>(EngineFactory("VEngineRenderView014", nullptr));
	Interfaces.Panel = static_cast<IPanel*>(VGUI2Factory("VGUI_Panel009", nullptr));
	Interfaces.Surface = static_cast<ISurface*>(MatSurfaceFactory("VGUI_Surface031", nullptr));
	Interfaces.MatSystem = static_cast<IMaterialSystem*>(MatSystemFactory("VMaterialSystem080", nullptr));
	Interfaces.PlayerInfoManager = static_cast<IPlayerInfoManager*>(ServerFactory("PlayerInfoManager002", nullptr));

	void** pClientTable = *reinterpret_cast<void***>(Interfaces.Client);
	Interfaces.ClientMode = **reinterpret_cast<IClientMode***>(reinterpret_cast<DWORD>(pClientTable[10]) + 5);

	Interfaces.ViewRender = **reinterpret_cast<IViewRender***>(Hack.Misc->FindSignature("client.dll", "FF 50 14 E8 ? ? ? ? 5D") - 7);

	Interfaces.Globals = Interfaces.PlayerInfoManager->GetGlobalVars();

	//-------------------------------------------------------------------------------------------------------------------//

	Hack.NetVars = std::shared_ptr<CNetVars>(new CNetVars());
	Hack.Drawings = std::shared_ptr<CDrawings>(new CDrawings());
	Hack.Misc = std::shared_ptr<CMisc>(new CMisc());
	Hack.Aimbot = std::shared_ptr<CAimbot>(new CAimbot());
	Hack.CVars = std::shared_ptr<CVars>(new CVars(hModule));

	//-------------------------------------------------------------------------------------------------------------------//

	Hack.Misc->InitNetVars();

	Hack.Misc->InitMaterials();

	Hack.CVars->Load();

	//-------------------------------------------------------------------------------------------------------------------//

	Hack.oModelIndex = Hack.NetVars->GetProxyFunction("DT_BaseViewModel", "m_nModelIndex");
	Hack.oItemDefinitionIndex = Hack.NetVars->GetProxyFunction("DT_BaseAttributableItem", "m_iItemDefinitionIndex");

	Hack.NetVars->HookProp("DT_BaseViewModel", "m_nModelIndex", Hooked_nModelIndex);
	Hack.NetVars->HookProp("DT_BaseAttributableItem", "m_iItemDefinitionIndex", Hooked_iItemDefinitionIndex);

	//-------------------------------------------------------------------------------------------------------------------//

	Hack.ClientModeHook = std::shared_ptr<CHook>(new CHook(Interfaces.ClientMode));
	Hack.PanelHook = std::shared_ptr<CHook>(new CHook(Interfaces.Panel));
	Hack.ModelRenderHook = std::shared_ptr<CHook>(new CHook(Interfaces.ModelRender));
	Hack.ViewRenderHook = std::shared_ptr<CHook>(new CHook(Interfaces.ViewRender));

	Hack.ClientModeHook->HookMethod(&Hooked_CreateMove, 24);
	Hack.PanelHook->HookMethod(&Hooked_PaintTraverse, 41);
	Hack.ModelRenderHook->HookMethod(&Hooked_DrawModelExecute, 21);
	Hack.ViewRenderHook->HookMethod(&Hooked_RenderView, 6);

	//-------------------------------------------------------------------------------------------------------------------//

	while(!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(101));

	Hack.NetVars->HookProp("DT_BaseViewModel", "m_nModelIndex", Hack.oModelIndex);
	Hack.NetVars->HookProp("DT_BaseAttributableItem", "m_iItemDefinitionIndex", Hack.oItemDefinitionIndex);

	SAFE_RESET(Hack.NetVars);
	SAFE_RESET(Hack.Drawings);
	SAFE_RESET(Hack.Misc);
	SAFE_RESET(Hack.Aimbot);
	SAFE_RESET(Hack.CVars);

	SAFE_RESET(Hack.ClientModeHook);
	SAFE_RESET(Hack.PanelHook);
	SAFE_RESET(Hack.ModelRenderHook);
	SAFE_RESET(Hack.ViewRenderHook);

	//FreeConsole();

	std::this_thread::sleep_for(std::chrono::seconds(1));
	FreeLibraryAndExitThread(hModule, 0);
}

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), hInstance, 0, nullptr);
	}

	return true;
}