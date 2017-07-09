/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"

class CLegitBot : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd);
private:
	// Targetting
	int GetTargetCrosshair();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd);

	void SyncWeaponSettings();

	// Functionality
	void DoAimbot(CUserCmd *pCmd);
	void DoTrigger(CUserCmd *pCmd);

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;

	float Speed;
	float FoV;
	bool RecoilControl;

};