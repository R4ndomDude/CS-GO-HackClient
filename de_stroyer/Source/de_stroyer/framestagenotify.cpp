#include "sdk.h"

FrameStageNotifyFn oFrameStageNotify;
void __stdcall Hooks::FrameStageNotify( ClientFrameStage_t stage ) {
	if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && Config->GetValue( "Skins", "Enabled" ).m_bValue)
	{
		//Cheat::Run_SkinChanger();
		//UpdateSkins();
	}

	/*Vector vecAimPunch;
	Vector vecViewPunch;

	Vector* pAimPunch = nullptr;
	Vector* pViewPunch = nullptr;

	if (I::Engine->IsInGame() && stage == FRAME_RENDER_START)
	{
	CBaseEntity* pLocal = I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());

	if (pLocal && pLocal->GetAlive())
	{
	pAimPunch = (Vector*)((DWORD)pLocal + offsets.m_aimPunchAngle);
	pViewPunch = (Vector*)((DWORD)pLocal + offsets.m_viewPunchAngle);

	vecAimPunch = *pAimPunch;
	vecViewPunch = *pViewPunch;

	*pAimPunch = Vector(0, 0, 0);
	*pViewPunch = Vector(0, 0, 0);
	}
	}*/

	oFrameStageNotify( stage );

	/*if (pAimPunch && pViewPunch)
	{
	*pAimPunch = vecAimPunch;
	*pViewPunch = vecViewPunch;
	}*/
}