#include "sdk.h"

void m_flPostponeFireReadyTime( const CRecvProxyData *data, void *pstruct, void *out )
{
	if(!G::LocalPlayer)
		return;

	*(float*)out = 0.f;
}

CreateMoveFn oCreateMove;
bool __stdcall Hooks::CreateMove( float flInputSampleTime, CUserCmd* cmd )
{
	static bool first = true;
	G::Return = true;
	G::LocalPlayer = Util::GetLocalPlayer( );
	G::UserCmd = cmd;

	if(cmd->command_number == 0)
		return false;

	ConVar* name = I::Cvar->FindVar( "name" );
	*(int*)((DWORD)&name->fnChangeCallback + 0xC) = NULL;

	if(G::LocalPlayer)
	{
		Cheat::Run_AirStuck( );
		Cheat::Run_Bhop( );
		Cheat::Run_Spinbot( );
		if(!G::Aimbotting)
		{
			G::LastAngle = cmd->viewangles;
		}
		Cheat::Run_Aimbot( );
		Cheat::Run_AchievementSpam( );
		Cheat::Run_LocationSpam( );
		Cheat::Run_NameSteal( );
		Cheat::Run_RCS( );

		if(G::LocalPlayer->GetWeapon( ))
		{
			*(float*)((DWORD)G::LocalPlayer->GetWeapon( ) + 0x32C4) = 0.f;
			Util::NetVars->HookProp( "DT_WeaponCSBase", "m_flPostponeFireReadyTime", m_flPostponeFireReadyTime );
			Util::NetVars->HookProp( "DT_WeaponDEagle", "m_flPostponeFireReadyTime", m_flPostponeFireReadyTime );
		}
	}

	static auto frame = Menu->GetFrame( );
	if(frame->GetVisible( ))
	{
		cmd->forwardmove = 0.f;
		cmd->sidemove = 0.f;
		cmd->upmove = 0.f;
		cmd->buttons = NULL;
	}

	if(G::Aimbotting && Config->GetValue( "Aimbot", "Silent" ).m_bValue)
		G::Return = false;

	//*(float*)((DWORD)G::LocalPlayer->GetWeapon() + 0x32C4) = 0.f;

	cmd->viewangles.Normalize( );

	first = false;

	return G::Return;
}