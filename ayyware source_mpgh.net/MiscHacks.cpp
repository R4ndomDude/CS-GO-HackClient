/*
Syn's AyyWare Framework 2015
*/

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include <time.h>

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	// Any init
}

void CMiscHacks::Draw()
{
	// Any drawing	
}

void CMiscHacks::Move(CUserCmd *pCmd)
{
	// Any Move Stuff

	// Bhop
	if (Menu::Window.VisualsTab.OtherAutoJump.GetState())
		AutoJump(pCmd);

	// Strafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	if (Menu::Window.VisualsTab.OtherAutoStrafe.GetState())
		AutoStrafe(pCmd);

	// Spams
	switch (Menu::Window.VisualsTab.OtherChatSpam.GetIndex())
	{
	case 0:
		// No Chat Spam
		break;
	case 1:
		// Round Say
		break;
	case 2:
		// Regular
		ChatSpamRegular();
		break;
	case 3:
		// Report Spam
		ChatSpamReports();
		break;
	}

}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}
	}
}

void CMiscHacks::AutoStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	static bool bDirection = true;

	bool bKeysPressed = true;
	if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44)) bKeysPressed = false;
	if (pCmd->buttons & IN_ATTACK) bKeysPressed = false;

	float flYawBhop = 0.f;
	if (pLocal->GetVelocity().Length() > 50.f)
	{
		float x = 30.f, y = pLocal->GetVelocity().Length(), z = 0.f, a = 0.f;

		z = x / y;
		z = fabsf(z);

		a = x * z;

		flYawBhop = a;
	}

	if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		
		if (bDirection)
		{
			AutoStrafeView -= flYawBhop;
			GameUtils::NormaliseViewAngle(AutoStrafeView);
			pCmd->sidemove = -400.f;
			bDirection = false;
		}
		else
		{
			AutoStrafeView += flYawBhop;
			GameUtils::NormaliseViewAngle(AutoStrafeView);
			pCmd->sidemove = 400.f;
			bDirection = true;
		}

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.f;
		}

		if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.f;
		}
	}
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

void CMiscHacks::ChatSpamReports()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.5)
		return;

	// Loop through all active entitys
	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() != entity->GetTeamNum())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	int randomIndex = rand() % Names.size();
	char buffer[128];
	static unsigned long int meme = 3091961887844204720;
	sprintf_s(buffer, "Report for %s submitted, report id %lu.", Names[randomIndex].c_str(), meme);
	meme += 1;
	SayInChat(buffer);
	start_t = clock();
}

void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.5)
		return;

	SayInChat("Aliens exist. #AliensInCSGOConfirmed");

	start_t = clock();
}
