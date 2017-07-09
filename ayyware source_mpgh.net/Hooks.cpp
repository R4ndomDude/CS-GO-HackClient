/*
Syn's AyyWare Framework 2015
*/

#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"

#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"

// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);

// Function Pointers to the originals
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;

// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __fastcall CreateMoveClient_Hooked(void* self, int edx, float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
};

// Initialise all our hooks
void Hooks::Initialise()
{
	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);
	//Utilities::Log("Paint Traverse Hooked");

	// No Visual Recoil
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);
	//Utilities::Log("InPrediction Hooked");

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);
	//Utilities::Log("DrawModelExecute Hooked");

	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)&CreateMoveClient_Hooked, 24);
	//oFrameStageNotify = (FrameStageNotifyFn)VMTClientMode.HookMethod((DWORD)Hooked_FrameStageNotify, 36);
	Utilities::Log("CreateMove");

	// Setup client hooks
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod( (DWORD)&Hooked_FrameStageNotify, 36);
	Utilities::Log("FrameStage Hey whats up hello?");
}

// Undo our hooks
void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
	//VMTClient.RestoreOriginal();
}

void MovementCorrection(CUserCmd* pCmd)
{

}

//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		//Render::GradientV(8, 8, 160, 18, Color(0, 0, 0, 0), Color(7, 39, 17, 255));
		//Render::Text(10, 10, Color(255, 255, 255, 220), Render::Fonts::Menu, "AyyWare New Meme of 2015");
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();

		// Update and draw the menu
		Menu::DoUIFrame();
	}
}

// InPrediction Hooked Function
bool __stdcall Hooked_InPrediction()
{
	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);

	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{
		IClientEntity* pLocalEntity = NULL;

		float* m_LocalViewAngles = NULL;

		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

// DrawModelExec for chams and shit
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	bool DontDraw = false;

	const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
	IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Menu::Window.VisualsTab.Active.GetState())
	{
		// Player Chams
		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();
		if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersPlayers.GetState() && strstr(ModelName, "models/player"))
		{
			if (pLocal && (!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() ||
				pModelEntity->GetTeamNum() != pLocal->GetTeamNum()))
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

				IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
				if (pModelEntity)
				{
					IClientEntity *local = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
					if (local)
					{
						if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
						{
							float alpha = 1.f;

							if (pModelEntity->HasGunGameImmunity())
								alpha = 0.5f;

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 240.f / 255.f;
								flColor[1] = 30.f / 255.f;
								flColor[2] = 35.f / 255.f;
							}
							else
							{
								flColor[0] = 63.f / 255.f;
								flColor[1] = 72.f / 255.f;
								flColor[2] = 205.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 247.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 20.f / 255.f;
							}
							else
							{
								flColor[0] = 32.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 57.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(open);
						}
						else
						{
							color.SetColor(255, 255, 255, 255);
							ForceMaterial(color, open);
						}
					}
				}
			}
		}
		else if (HandsStyle != 0 && strstr(ModelName, "arms"))
		{
			if (HandsStyle == 1)
			{
				DontDraw = true;
			}
			else if (HandsStyle == 2)
			{
				Interfaces::RenderView->SetBlend(0.3);
			}
			else if (HandsStyle == 3)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;
				if (pLocal)
				{
					if (pLocal->IsAlive())
					{
						int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(240, 30, 35, alpha);
						else
							color.SetColor(63, 72, 205, alpha);

						ForceMaterial(color, covered);
						oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(247, 180, 20, alpha);
						else
							color.SetColor(32, 180, 57, alpha);
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
					}

					ForceMaterial(color, open);
				}
			}
			else
			{
				static int counter = 0;
				static float colors[3] = { 1.f, 0.f, 0.f };

				if (colors[counter] >= 1.0f)
				{
					colors[counter] = 1.0f;
					counter += 1;
					if (counter > 2)
						counter = 0;
				}
				else
				{
					int prev = counter - 1;
					if (prev < 0) prev = 2;
					colors[prev] -= 0.05f;
					colors[counter] += 0.05f;
				}

				Interfaces::RenderView->SetColorModulation(colors);
				Interfaces::RenderView->SetBlend(0.3);
				Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
			}
		}
		else if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersWeapons.GetState() && strstr(ModelName, "_dropped.mdl"))
		{
			IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
			color.SetColor(255, 255, 255, 255);
			ForceMaterial(color, covered);
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}

// ClientMode CreateMove
bool __fastcall CreateMoveClient_Hooked(void* self, int edx, float frametime, CUserCmd* pCmd)
{
	// Backup for safety
	Vector origView = pCmd->viewangles;
	Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
	Vector qAimAngles;
	qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
	AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

	// Do da hacks
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
		Hacks::MoveHacks(pCmd);

	//Movement Fix
	//GameUtils::CL_FixMove(pCmd, origView);
	qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f);
	AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
	qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
	AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
	Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
	Vector vRightNorm;			Normalize(viewright, vRightNorm);
	Vector vUpNorm;				Normalize(viewup, vUpNorm);

	// Original shit for movement correction
	float forward = pCmd->forwardmove;
	float right = pCmd->sidemove;
	float up = pCmd->upmove;
	if (forward > 450) forward = 450;
	if (right > 450) right = 450;
	if (up > 450) up = 450;
	if (forward < -450) forward = -450;
	if (right < -450) right = -450;
	if (up < -450) up = -450;
	pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
	pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
	pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

	// Angle normalisation
	if (Menu::Window.VisualsTab.OtherSafeMode.GetState())
	{
		GameUtils::NormaliseViewAngle(pCmd->viewangles);

		if (pCmd->viewangles.z != 0.0f)
		{
			pCmd->viewangles.z = 0.00;
		}

		if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
		{
			Utilities::Log("Having to re-normalise!");
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
			Beep(750, 800);
			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				pCmd->viewangles = origView;
				pCmd->sidemove = right;
				pCmd->forwardmove = forward;
			}
		}
	}

	return false;
}

/*void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	oFrameStageNotify(curStage);

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (Menu::Window.MiscTab.KnifeEnable.GetState() && pLocal)
		{
			IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
			CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)WeaponEnt;
			if (Weapon)
			{
				//Utilities::Log("NIGGA WE CLOSE!");
				static bool LastItemWasKnife = false;
				if (WeaponEnt->GetClientClass()->m_ClassID == (int)CSGOClassID::CKnife)
				{
					*Weapon->FallbackStatTrak() = 1337;
					int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();
					if (Skin == 0) *Weapon->FallbackPaintKit() = 416;
					if (Skin == 1) *Weapon->FallbackPaintKit() = 415;
					if (Skin == 3) *Weapon->FallbackPaintKit() = 409;
					if (Skin == 4) *Weapon->FallbackPaintKit() = 0;
					*Weapon->FallbackWear() = 0.0001;

					*Weapon->m_AttributeManager()->m_Item()->ItemIDLow() = 7;
					int Model = Menu::Window.MiscTab.KnifeModel.GetIndex();
					*Weapon->m_AttributeManager()->m_Item()->ItemIDHigh() = (Model == 0) ? 507 : 500;
					*Weapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = (Model == 0) ? 507 : 500;

					if (GUI.GetKeyState(VK_END))
						ForceUpdate();
				}
			}
		}
	}
}*/


void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	oFrameStageNotify(curStage);

	//Utilities::Log("plsplspls");

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		//Utilities::Log("APPLY SKIN APPLY SKIN");
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
		int iGunGame = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");
		int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
		int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
		int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");

		int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");

		for (int i = Interfaces::EntList->GetHighestEntityIndex(); i >= 0; --i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			if (pEntity)
			{
				ULONG hOwnerEntity = *(PULONG)((DWORD)pEntity + 0x148);

				IClientEntity* pOwner = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)hOwnerEntity);

				if (pOwner)
				{
					if (pOwner == pLocal)
					{
						std::string sWeapon = Interfaces::ModelInfo->GetModelName(pEntity->GetModel());

						if (!(sWeapon.find("models/weapons", 0) != std::string::npos))
							continue;

						if (sWeapon.find("c4_planted", 0) != std::string::npos)
							continue;

						if (sWeapon.find("thrown", 0) != std::string::npos)
							continue;

						if (sWeapon.find("smokegrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("flashbang", 0) != std::string::npos)
							continue;

						if (sWeapon.find("fraggrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("molotov", 0) != std::string::npos)
							continue;

						if (sWeapon.find("decoy", 0) != std::string::npos)
							continue;

						if (sWeapon.find("incendiarygrenade", 0) != std::string::npos)
							continue;

						if (sWeapon.find("ied", 0) != std::string::npos)
							continue;

						if (sWeapon.find("w_eq_", 0) != std::string::npos)
							continue;

						CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pEntity;

						ClientClass *pClass = Interfaces::Client->GetAllClasses();

						if (Menu::Window.MiscTab.KnifeEnable.GetState())
						{
							int Model = Menu::Window.MiscTab.KnifeModel.GetIndex();

							int M41S = Menu::Window.MiscTab.M41SSkin.GetIndex();
							int M4A4 = Menu::Window.MiscTab.M4A4Skin.GetIndex();
							int AK47 = Menu::Window.MiscTab.AK47Skin.GetIndex();
							int AWP = Menu::Window.MiscTab.AWPSkin.GetIndex();
							int Glock = Menu::Window.MiscTab.GLOCKSkin.GetIndex();
							int USPS = Menu::Window.MiscTab.USPSSkin.GetIndex();
							int Deagle = Menu::Window.MiscTab.DEAGLESkin.GetIndex();
							int Magnum = Menu::Window.MiscTab.DEAGLESkin.GetIndex();

							if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 26) // M41S
							{
								if (M41S == 0)
								{
									*pWeapon->FallbackPaintKit() = 321;
								}
								else if (M41S == 1)
								{
									*pWeapon->FallbackPaintKit() = 430;
								}
								else if (M41S == 2)
								{
									*pWeapon->FallbackPaintKit() = 360;
								}
								else if (M41S == 3)
								{
									*pWeapon->FallbackPaintKit() = 445;
								}
							}
							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 60) // M4A4
							{
								if (M4A4 == 0)
								{
									*pWeapon->FallbackPaintKit() = 512;
								}
								else if (M4A4 == 1)
								{
									*pWeapon->FallbackPaintKit() = 309;
								}
								else if (M4A4 == 2)
								{
									*pWeapon->FallbackPaintKit() = 155;
								}
								else if (M4A4 == 3)
								{
									*pWeapon->FallbackPaintKit() = 400;
								}
								else if (M4A4 == 4)
								{
									*pWeapon->FallbackPaintKit() = 255;
								}
							}
							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 7) // AK47
							{
								if (AK47 == 0)
								{
									*pWeapon->FallbackPaintKit() = 506;
								}
								else if (AK47 == 1)
								{
									*pWeapon->FallbackPaintKit() = 474;
								}
								else if (AK47 == 2)
								{
									*pWeapon->FallbackPaintKit() = 302;
								}
								else if (AK47 == 3)
								{
									*pWeapon->FallbackPaintKit() = 180;
								}
								else if (AK47 == 4)
								{
									*pWeapon->FallbackPaintKit() = 316;
								}
							}
							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 9) // AWP
							{
								if (AWP == 0)
								{
									*pWeapon->FallbackPaintKit() = 475;
								}
								else if (AWP == 1)
								{
									*pWeapon->FallbackPaintKit() = 279;
								}
								else if (AWP == 2)
								{
									*pWeapon->FallbackPaintKit() = 344;
								}
								else if (AWP == 3)
								{
									*pWeapon->FallbackPaintKit() = 51;
								}
							}
							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 4) // GLOCK
							{
								if (Glock == 0)
								{
									*pWeapon->FallbackPaintKit() = 353;
								}
								else if (Glock == 1)
								{
									*pWeapon->FallbackPaintKit() = 38;
								}
								else if (Glock == 2)
								{
									*pWeapon->FallbackPaintKit() = 437;
								}
								else if (Glock == 3)
								{
									*pWeapon->FallbackPaintKit() = 479;
								}
							}
							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 61) // USPS
							{
								if (USPS == 0)
								{
									*pWeapon->FallbackPaintKit() = 504;
								}
								else if (USPS == 1)
								{
									*pWeapon->FallbackPaintKit() = 339;
								}
								else if (USPS == 2)
								{
									*pWeapon->FallbackPaintKit() = 313;
								}
								else if (USPS == 3)
								{
									*pWeapon->FallbackPaintKit() = 290;
								}
							}

							else if (*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64) // USPS
							{
								if (Magnum == 0)
								{
									*pWeapon->FallbackPaintKit() = 12;
								}
								else if (Magnum == 1)
								{
									*pWeapon->FallbackPaintKit() = 522;
								}
								else if (Magnum == 2)
								{
									*pWeapon->FallbackPaintKit() = 523;
								}
							}

							if (pEntity->GetClientClass()->m_ClassID == (int)CSGOClassID::CKnife)
							{
								if (Model == 0) // Karambit
								{
									*pWeapon->ViewModelIndex() = iKarambit;
									*pWeapon->WorldModelIndex() = iKarambit + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 507;

									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 1) // Bayonet
								{

									*pWeapon->ViewModelIndex() = iBayonet;
									*pWeapon->WorldModelIndex() = iBayonet + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 500;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 2) // Huntsman Knife
								{

									*pWeapon->ViewModelIndex() = iHuntsman;
									*pWeapon->WorldModelIndex() = iHuntsman + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 509;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 3) // Gut Knife
								{

									*pWeapon->ViewModelIndex() = iGut;
									*pWeapon->WorldModelIndex() = iGut + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 506;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 4) // Walmart Knife
								{

									*pWeapon->ViewModelIndex() = iFalchion;
									*pWeapon->WorldModelIndex() = iFalchion + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 512;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 5) // Shadow Daggers Knife
								{

									*pWeapon->ViewModelIndex() = iDagger;
									*pWeapon->WorldModelIndex() = iDagger + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 516;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 6) // Butterfly Knife
								{

									*pWeapon->ViewModelIndex() = iButterfly;
									*pWeapon->WorldModelIndex() = iButterfly + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 515;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 7) // Flip Knife
								{

									*pWeapon->ViewModelIndex() = iFlip;
									*pWeapon->WorldModelIndex() = iFlip + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 505;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
								else if (Model == 8) // M9 Bayonet
								{

									*pWeapon->ViewModelIndex() = iM9Bayonet;
									*pWeapon->WorldModelIndex() = iM9Bayonet + 1;
									*pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 508;
									int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

									if (Skin == 0)
									{
										*pWeapon->FallbackPaintKit() = 38; // Fade
									}
									else if (Skin == 1)
									{
										*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
									}
									else if (Skin == 2)
									{
										*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
									}
									else if (Skin == 3)
									{
										*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
									}
									else if (Skin == 4)
									{
										*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
									}
									else if (Skin == 5)
									{
										*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
									}
									else if (Skin == 6)
									{
										*pWeapon->FallbackPaintKit() = 415; // Doppler (Ruby)
									}
									else if (Skin == 7)
									{
										*pWeapon->FallbackPaintKit() = 416; // Doppler (Sapphire)
									}
									else if (Skin == 8)
									{
										*pWeapon->FallbackPaintKit() = 417; // Doppler (Blackpearl)
									}
									else if (Skin == 9)
									{
										*pWeapon->FallbackPaintKit() = 413; // Marble Fade
									}
									else if (Skin == 10)
									{
										*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
									}
									else if (Skin == 11)
									{
										*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
									}
									else if (Skin == 12)
									{
										*pWeapon->FallbackPaintKit() = 12; // Crimson Web
									}
									else if (Skin == 13)
									{
										*pWeapon->FallbackPaintKit() = 59; // Slaughter
									}
									else if (Skin == 14)
									{
										*pWeapon->FallbackPaintKit() = 40; // Night
									}
								}
							}
							*pWeapon->OwnerXuidLow() = 0;
							*pWeapon->OwnerXuidHigh() = 0;
							*pWeapon->FallbackWear() = 0.001f;
							*pWeapon->m_AttributeManager()->m_Item()->ItemIDHigh() = 1;
						}
					}
				}

			}
		}
	}
}
