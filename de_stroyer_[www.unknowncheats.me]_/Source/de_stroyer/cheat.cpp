#include "sdk.h"
#include "mathematics.h"

void Cheat::Run_Aimbot( )
{
	G::Aimbotting = false;

	if(!G::LocalPlayer->GetAlive() || G::LocalPlayer->GetHealth() <= 0) return;

	if(!Config->GetValue( "Aimbot", "Enabled" ).m_bValue) return;
	bool bUseKey = Config->GetValue( "Aimbot", "Use Key" ).m_bValue;
	bool bSmooth = Config->GetValue( "Aimbot", "Smoothing" ).m_bValue;
	bool bFOVCheck = Config->GetValue( "Aimbot", "FOV Check" ).m_bValue;
	bool bAimStep = Config->GetValue( "Aimbot", "Aim Step" ).m_bValue;
	bool bSilent = Config->GetValue( "Aimbot", "Silent" ).m_bValue;
	bool bHitChance = Config->GetValue( "Aimbot", "Hit Chance" ).m_bValue;
	bool bAutoShoot = Config->GetValue( "Aimbot", "Auto Shoot" ).m_bValue;
	bool bAutoStop = Config->GetValue( "Aimbot", "Auto Stop" ).m_bValue;
	bool bAutoCrouch = Config->GetValue( "Aimbot", "Auto Crouch" ).m_bValue;
	float flSmoothness = Config->GetValue( "Aimbot", "Smoothness" ).m_flValue;
	float flHitChanceAmt = Config->GetValue( "Aimbot", "Hit Chance Amount" ).m_flValue;
	int iKey = Config->GetValue( "Aimbot", "Key" ).m_iValue;

	if(bUseKey && !GetAsyncKeyState( iKey )) return;

	static CBaseCombatWeapon* pWeapon;
	pWeapon = G::LocalPlayer->GetWeapon( );

	if(bSilent && !bAimStep)	
	{
		auto pWeapon = G::LocalPlayer->GetWeapon( );
		float flServerTime = G::LocalPlayer->GetTickBase( ) * I::Globals->interval_per_tick;
		if(pWeapon && pWeapon->GetNextPrimaryAttack( ) > flServerTime) return;
	}

	static Angle angAimAngles;
	static int iClosestIndex;

	iClosestIndex = Aim::ClosestAngle( angAimAngles );

	if(iClosestIndex != -1)
	{
		G::Aimbotting = true;

		angAimAngles.x -= (G::LocalPlayer->GetPunch( ).x * 2.f);
		angAimAngles.y -= (G::LocalPlayer->GetPunch( ).y * 2.f);

		bool bShouldAutoShoot = false;;

		if(bSilent) FixMove->Start( );

		if(bSmooth)
			G::UserCmd->viewangles = (Aim::Smooth( G::UserCmd->viewangles, angAimAngles, flSmoothness ));
		else
		{
			if(true)//bAimStep)
			{
				Angle angNextAngle;
				bool bFinished = Aim::AimStep( G::LastAngle, angAimAngles, angNextAngle );
				G::UserCmd->viewangles = angNextAngle;
				G::LastAngle = angNextAngle;
				if(bFinished) 
					bShouldAutoShoot = true;
			}
			else
			{
				G::UserCmd->viewangles = angAimAngles;
				bShouldAutoShoot = true;
			}
		}

		if(bSilent) FixMove->End( );

		if(bShouldAutoShoot && bAutoShoot && (!bHitChance || (pWeapon && (1.0f - pWeapon->GetAccuracyPenalty( )) * 100.f >= flHitChanceAmt)))
		{
			G::UserCmd->buttons |= IN_ATTACK;
		}

		if(bAutoStop)
		{
			G::UserCmd->forwardmove = 0;
			G::UserCmd->sidemove = 0;
			G::UserCmd->upmove = 0;
		}

		if(bAutoCrouch)
		{
			G::UserCmd->buttons |= IN_DUCK;
		}
	}
}

void Cheat::Run_AirStuck( )
{
	if(!Config->GetValue( "Misc", "Air Stuck" ).m_bValue) return;
	if(!GetAsyncKeyState( Config->GetValue( "Misc", "Air Stuck Key" ).m_iValue )) return;

	G::UserCmd->tick_count = 16777216;
}

void Cheat::Run_AchievementSpam( )
{
	if(!Config->GetValue( "Misc", "Achievement Spam" ).m_bValue) return;
	if(GetAsyncKeyState( 'G' )) return;
	if(GetAsyncKeyState( VK_LBUTTON )) return;

	static char cmd[256];
	static int iPlayerID, currentachievement;
	static player_info_t info;
	static unsigned short mask;
	I::Engine->GetPlayerInfo( G::LocalPlayer->index, &info );
	iPlayerID = info.userid;
	mask = UTIL_GetAchievementEventMask( );

	int achievements[12] = { 0x0FCC, 0x0FCB, 0x0FCA, 0x0FCA, 0x0FC9, 0x0FC8, 0x0FC7, 0x0FC6, 0x0FC5, 0x0FC4, 0x0FC3, 0x0C2 };
	sprintf( cmd, "achievement_earned %d %d", achievements[currentachievement % 12] ^ mask, (iPlayerID ^ achievements[currentachievement % 12]) ^ mask );
	I::Engine->ClientCmd_Unrestricted( cmd );

	++currentachievement;
}

void Cheat::Run_Bhop( )
{
	if(G::UserCmd->buttons & IN_JUMP && !(G::LocalPlayer->GetFlags( ) & FL_ONGROUND))
	{
		if(Config->GetValue( "Misc", "Bhop" ).m_bValue)
			G::UserCmd->buttons &= ~IN_JUMP;

		if(!Config->GetValue( "Misc", "Auto Strafe" ).m_bValue) return;
		bool backwards = false;// G::UserCmd->forwardmove < 0;
		if(G::UserCmd->mousedx < 0) G::UserCmd->sidemove = backwards ? 650.f : -650.f;
		if(G::UserCmd->mousedx > 0) G::UserCmd->sidemove = backwards ? -650.f : 650.f;
	}
}

void Cheat::Run_ESP( )
{
	if(!Config->GetValue( "Visuals", "Enabled" ).m_bValue) return;

	int iScreenWidth, iScreenHeight;
	I::Engine->GetScreenSize( iScreenWidth, iScreenHeight );

	if(I::Engine->IsInGame( ))
	{
		for(int iPlayerIndex = 0; iPlayerIndex < 64; iPlayerIndex++)
		{
			auto pEntity = I::ClientEntList->GetClientEntity( iPlayerIndex );

			if(!G::LocalPlayer || !pEntity || pEntity == G::LocalPlayer || iPlayerIndex == I::Engine->GetLocalPlayer( ) || 0 >= pEntity->GetHealth( ) || pEntity->GetDormant( )/* ||(Vars::Visuals::EnemyOnly && (pEntity->team() == G::LocalPlayer->team())) || (Vars::Visuals::VisCheck && !Aim->TraceBone(pEntity))*/)
				continue;

			Vector vecMax = pEntity->GetCollideable( )->OBBMaxs( );
			Vector vecPos, vecPos3D;
			Vector vecTop, vecTop3D;
			vecPos3D = pEntity->GetOrigin( );
			vecTop3D = vecPos3D + Vector( 0, 0, vecMax.z );
			if(!Draw::WorldToScreen( vecPos3D, vecPos ) || !Draw::WorldToScreen( vecTop3D, vecTop ))
				continue;

			int iMiddle = (vecPos.y - vecTop.y) + 4;
			int iWidth = iMiddle / 3.5;
			int iCornerWidth = iWidth / 4;
			float flTeamColor = Config->GetValue( "Visuals", "Team Color" ).m_flValue;
			float flEnemyColor = Config->GetValue( "Visuals", "Enemy Color" ).m_flValue;
			static Color colTeam;

			if(pEntity->GetTeam( ) == G::LocalPlayer->GetTeam( ))
				colTeam = Color( ).FromHSB( flTeamColor, 1.f, 1.f );
			else
				colTeam = Color( ).FromHSB( flEnemyColor, 1.f, 1.f );

			if(Config->GetValue( "Visuals", "Name" ).m_bValue)
			{
				Draw::DrawString( 5, vecTop.x + iWidth + (iWidth / 8) + 8, vecTop.y + 6, colTeam, false, "%s", pEntity->GetName( ).c_str( ) );
			}

			if(Config->GetValue( "Visuals", "Box" ).m_bValue)
			{
				Draw::DrawOutlinedRect( vecTop.x - iWidth, vecTop.y, iWidth * 2, iMiddle, Color( 0, 0, 0 ) );
				Draw::DrawOutlinedRect( vecTop.x - iWidth + 1, vecTop.y + 1, iWidth * 2 - 2, iMiddle - 2, colTeam );
				Draw::DrawOutlinedRect( vecTop.x - iWidth + 2, vecTop.y + 2, iWidth * 2 - 4, iMiddle - 4, Color( 0, 0, 0 ) );
			}

			if(Config->GetValue( "Visuals", "Bone Box" ).m_bValue && pEntity->GetTeam( ) != G::LocalPlayer->GetTeam( ))
			{
				Vector vecBonePos;
				Vector vecAimPos = pEntity->GetBonePosition( Config->GetValue( "Aimbot", "Bone" ).m_iValue );
				int iTWidth = iWidth / 6;

				if(Draw::WorldToScreen( vecAimPos, vecBonePos ))
				{
					Draw::DrawOutlinedRect( vecBonePos.x - iTWidth - 1, vecBonePos.y - iTWidth - 1, iTWidth * 2 + 2, iTWidth * 2 + 2, Color( 0, 0, 0 ) );
					Draw::DrawOutlinedRect( vecBonePos.x - iTWidth, vecBonePos.y - iTWidth, iTWidth * 2, iTWidth * 2, colTeam );
					Draw::DrawOutlinedRect( vecBonePos.x - iTWidth + 1, vecBonePos.y - iTWidth + 1, iTWidth * 2 - 2, iTWidth * 2 - 2, Color( 0, 0, 0 ) );
				}
			}

			if(Config->GetValue( "Visuals", "Line" ).m_bValue && pEntity->GetTeam( ) != G::LocalPlayer->GetTeam( ))
			{
				int ox, oy = 0;
				Math::SnapRotate( iScreenWidth / 2, iScreenHeight, vecTop.x, vecTop.y + iMiddle - 1, ox, oy );
				Draw::DrawLine( iScreenWidth / 2 - ox, iScreenHeight - oy, vecTop.x - ox, vecTop.y + iMiddle - 1 - oy, Color( 0, 0, 0 ) );
				Draw::DrawLine( iScreenWidth / 2, iScreenHeight, vecTop.x, vecTop.y + iMiddle - 1, colTeam );
				Draw::DrawLine( iScreenWidth / 2 + ox, iScreenHeight + oy, vecTop.x + ox, vecTop.y + iMiddle - 1 + oy, Color( 0, 0, 0 ) );
			}

			int iClampedHealth = pEntity->GetHealth( );
			if(iClampedHealth >= 101)
				iClampedHealth = 100;

			int iHealthG = iClampedHealth * 2.55;
			int iHealthR = 255 - iHealthG;

			Color colHealth = Color( iHealthR, iHealthG, 0, 255 );

			if(Config->GetValue( "Visuals", "Health Bar" ).m_bValue)
			{
				Draw::DrawRect( vecTop.x + iWidth + 3, vecTop.y + 1, iWidth / 8, iMiddle * 0.01 * iClampedHealth - 2, colHealth );
				Draw::DrawOutlinedRect( vecTop.x + iWidth + 2, vecTop.y, (iWidth / 8) + 2, iMiddle, Color( 0, 0, 0, 255 ) );
			}

			if(Config->GetValue( "Visuals", "Health Text" ).m_bValue)
			{
				Draw::DrawString( 5, vecTop.x + iWidth + (iWidth / 8) + 8, vecTop.y + 16, colHealth, false, "%i", pEntity->GetHealth( ) );
			}
		}
	}
}

void Cheat::Run_LocationSpam( )
{
	if(!Config->GetValue( "Misc", "Location Spam" ).m_bValue) return;

	static std::string name;
	CBaseEntity* pEntity = NULL;

	while(!pEntity)
	{
		pEntity = I::ClientEntList->GetClientEntity( RandomInt( 0, I::ClientEntList->GetHighestEntityIndex( ) ) );

		if(!pEntity) return;
		if(!pEntity->GetAlive( )) return;
		name = pEntity->GetName( );
		if(pEntity->GetName( ) == "GOTV" || pEntity->GetName( ) == "") return;

		char szCmd[256];
		sprintf( szCmd, "say \"%s is at %s with %d health!\"", name.c_str( ), pEntity->GetLastPlace( ).c_str( ), pEntity->GetHealth( ) );
		I::Engine->ClientCmd_Unrestricted( szCmd );
		return;
	}
}

void Cheat::Run_NameSteal( )
{
	static ConVar* name = I::Cvar->FindVar( "name" );

	if(!Config->GetValue( "Misc", "Name Steal" ).m_bValue) return;

	char* szName = NULL;

	while(!szName)
	{
		player_info_t pInfo;
		I::Engine->GetPlayerInfo( RandomInt( 0, I::ClientEntList->GetHighestEntityIndex( ) ), &pInfo );

		if(strcmp( pInfo.name, "" ) != 0 && !(strcmp( pInfo.name, "GOTV" ) == 0)) szName = pInfo.name;
	}

	char nameChar[64];
	sprintf( nameChar, "%s ", szName );
	name->SetValue( nameChar );
}

void Cheat::Run_RCS( )
{
	if(!Config->GetValue( "Aimbot", "Enabled" ).m_bValue) return;

	if(G::LocalPlayer->GetHealth( ) <= 0)
		return;

	if(!Config->GetValue( "Aimbot", "RCS" ).m_bValue)
		return;

	static int iRCSAmount;
	iRCSAmount = Config->GetValue( "Aimbot", "RCS Amount" ).m_flValue;

	/*if (Config->GetValue("Visuals", "No Visual Recoil").m_bValue && !(G::Aimbotting && Config->GetValue("Aimbot", "Silent").m_bValue))
	{
	if (G::LocalPlayer->GetShotsFired() > 1 && !G::Aimbotting)
	{
	G::UserCmd->viewangles.x -= G::LocalPlayer->GetPunch().x * 2.f;
	G::UserCmd->viewangles.y -= G::LocalPlayer->GetPunch().y * 2.f;

	G::Return = false;
	}
	}
	else
	{*/
	static Angle angle;
	static Angle viewAngle;
	static Angle oldAngle;
	static Vector punchAngle;
	static int shotsFired = 0;

	viewAngle = G::UserCmd->viewangles;
	punchAngle = G::LocalPlayer->GetPunch( );

	punchAngle.x *= (iRCSAmount / 100.f);
	punchAngle.y *= (iRCSAmount / 100.f);

	viewAngle.x += oldAngle.x;
	viewAngle.y += oldAngle.y;

	angle.x = viewAngle.x - punchAngle.x * 2;
	angle.y = viewAngle.y - punchAngle.y * 2;
	G::UserCmd->viewangles = angle;

	oldAngle.x = punchAngle.x * 2;
	oldAngle.y = punchAngle.y * 2;
	//}*/
}

void Cheat::Run_SkinChanger( )
{
	CBaseEntity* localent = G::LocalPlayer;

	if(!localent)
		return;

	player_info_t info;
	I::Engine->GetPlayerInfo( localent->index, &info );

	CBaseCombatWeapon* weapon = localent->GetWeapon( );
	if(!weapon)
		return;

	int weaponid = weapon->GetWeaponID( );

	for(auto& skin : SkinsUtil::data)
	{
		std::string weaponstr;
		switch(weaponid)
		{
		case WEAPON_DEAGLE:
			weaponstr.assign( "WEAPON_DEAGLE" );
			break;
		case WEAPON_DUAL:
			weaponstr.assign( "WEAPON_DUAL" );
			break;
		case WEAPON_FIVE7:
			weaponstr.assign( "WEAPON_FIVE7" );
			break;
		case WEAPON_GLOCK:
			weaponstr.assign( "WEAPON_GLOCK" );
			break;
		case WEAPON_AK47:
			weaponstr.assign( "WEAPON_AK47" );
			break;
		case WEAPON_AUG:
			weaponstr.assign( "WEAPON_AUG" );
			break;
		case WEAPON_AWP:
			weaponstr.assign( "WEAPON_AWP" );
			break;
		case WEAPON_FAMAS:
			weaponstr.assign( "WEAPON_FAMAS" );
			break;
		case WEAPON_G3SG1:
			weaponstr.assign( "WEAPON_G3SG1" );
			break;
		case WEAPON_GALIL:
			weaponstr.assign( "WEAPON_GALILAR" );
			break;
		case WEAPON_M249:
			weaponstr.assign( "WEAPON_M249" );
			break;
		case WEAPON_M4A4:
			weaponstr.assign( "WEAPON_M4A4" );
			break;
		case WEAPON_MAC10:
			weaponstr.assign( "WEAPON_MAC10" );
			break;
		case WEAPON_P90:
			weaponstr.assign( "WEAPON_P90" );
			break;
		case WEAPON_UMP:
			weaponstr.assign( "WEAPON_UMP" );
			break;
		case WEAPON_XM1014:
			weaponstr.assign( "WEAPON_XM1014" );
			break;
		case WEAPON_BIZON:
			weaponstr.assign( "WEAPON_BIZON" );
			break;
		case WEAPON_MAG7:
			weaponstr.assign( "WEAPON_MAG7" );
			break;
		case WEAPON_NEGEV:
			weaponstr.assign( "WEAPON_NEGEV" );
			break;
		case WEAPON_SAWEDOFF:
			weaponstr.assign( "WEAPON_SAWEDOFF" );
			break;
		case WEAPON_TEC9:
			weaponstr.assign( "WEAPON_TEC9" );
			break;
		case WEAPON_HKP2000:
			weaponstr.assign( "WEAPON_HKP2000" );
			break;
		case WEAPON_MP7:
			weaponstr.assign( "WEAPON_MP7" );
			break;
		case WEAPON_MP9:
			weaponstr.assign( "WEAPON_MP9" );
			break;
		case WEAPON_NOVA:
			weaponstr.assign( "WEAPON_NOVA" );
			break;
		case WEAPON_P250:
			weaponstr.assign( "WEAPON_P250" );
			break;
		case WEAPON_SCAR20:
			weaponstr.assign( "WEAPON_SCAR20" );
			break;
		case WEAPON_SG553:
			weaponstr.assign( "WEAPON_SG553" );
			break;
		case WEAPON_SSG08:
			weaponstr.assign( "WEAPON_SSG08" );
			break;
		case WEAPON_KNIFET:
			//weaponstr.assign("weapon_hkp2000");
			break;
		case WEAPON_KNIFECT:
			//weaponstr.assign("weapon_hkp2000");
			break;
		}

		std::transform( weaponstr.begin( ), weaponstr.end( ), weaponstr.begin( ), ::tolower );

		if(weaponstr == skin.first)
		{
			bool bStatTrak = Config->GetValue( "Skins", "StatTrak" ).m_bValue;
			//weapon->GetEntityQuality() = bStatTrak ? 4 : 0;
			//weapon->GetFallbackPaintKit() = SkinsUtil::data.find(weaponstr)->second;
			//weapon->GetFallbackStatTrak() = bStatTrak ? 1 : 0;
			//weapon->GetFallbackWear() = .0000001f;
			//weapon->GetAccountID() = info.xuidlow;
			//weapon->GetItemIDHigh() = 1;
			Util::PrintMessage( "m_iEntityQuality [0x%X]\n", offsets.m_iEntityQuality );
			Util::PrintMessage( "m_nFallbackPaintKit [0x%X]\n", offsets.m_nFallbackPaintKit );
			Util::PrintMessage( "m_nFallbackStatTrak [0x%X]\n", offsets.m_nFallbackStatTrak );
			Util::PrintMessage( "m_flFallbackWear [0x%X]\n", offsets.m_flFallbackWear );
			Util::PrintMessage( "m_iAccountID [0x%X]\n", offsets.m_iAccountID );
			Util::PrintMessage( "m_iItemIDHigh [0x%X]\n", offsets.m_iItemIDHigh );
			*(int*)((DWORD)weapon + offsets.m_iEntityQuality) = 4;
			*(int*)((DWORD)weapon + offsets.m_nFallbackPaintKit) = 474;
			*(int*)((DWORD)weapon + offsets.m_nFallbackStatTrak) = 1;
			*(float*)((DWORD)weapon + offsets.m_flFallbackWear) = .0000001f;
			*(int*)((DWORD)weapon + offsets.m_iAccountID) = info.xuidlow;
			*(int*)((DWORD)weapon + offsets.m_iItemIDHigh) = 1;
			Util::PrintMessage( "[%s] == [%s] : [%d]\n", weaponstr.c_str( ), skin.first.c_str( ), skin.second );
		}
		else
		{
			//Util::PrintMessage("[%s] != [%s]\n", weaponstr.c_str(), skin.first.c_str());
		}
	}

	/*if (weaponid == WEAPON_KNIFECT || weaponid == WEAPON_KNIFET)
	{
	bool bStatTrak = Config->GetValue("Skins", "StatTrak").m_bValue;
	weapon->GetItemDefinitionIndex() = KNIFE_KARAMBIT;
	weapon->GetEntityQuality() = bStatTrak ? 4 : 0;
	weapon->GetFallbackPaintKit() = SkinsUtil::data.find("weapon_knife_karambit")->second;
	weapon->GetAccountID() = info.xuidlow;
	weapon->GetFallbackStatTrak() = bStatTrak ? 1337 : 0;
	weapon->GetFallbackWear() = .0000001f;
	weapon->GetItemIDHigh() = 1;
	}*/
}

void Cheat::Run_Spinbot( )
{
	if(!Config->GetValue( "Aimbot", "Enabled" ).m_bValue) return;

	static int state = 0;

	if(!Config->GetValue( "Aimbot", "Spinbot" ).m_bValue || (G::Aimbotting/* || (G::UserCmd->buttons & IN_ATTACK)*/))
	{
		state = 0;
		return;
	}

	if(G::LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER)
		return;

	if(G::UserCmd->buttons & IN_USE)
		return;

	/*
	G::UserCmd->viewangles = G::LastAngle;
	G::UserCmd->viewangles.x = 0;
	G::UserCmd->viewangles.y -= 45.f * (state % 8);
	G::UserCmd->viewangles.Normalize();
	*/

	static Angle LastLastAngle = G::LastAngle;

	if((LastLastAngle.x != G::LastAngle.x) || (LastLastAngle.y != G::LastAngle.y))
		state = 0;

	FixMove->Start( );
	G::UserCmd->viewangles = G::LastAngle;
	G::UserCmd->viewangles.x = -89 + (rand( ) % 178);
	G::UserCmd->viewangles.y = -179 + (rand( ) % 358);
	G::UserCmd->viewangles.Normalize( );
	FixMove->End( );

	LastLastAngle = G::LastAngle;

	G::Return = false;

	++state;
}