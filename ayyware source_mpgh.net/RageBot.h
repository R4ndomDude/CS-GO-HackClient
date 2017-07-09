/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"

class CRageBot : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd);
private:
	// Targetting
	int GetTargetCrosshair();
	int GetTargetDistance();
	int GetTargetHealth();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	int HitScan(IClientEntity* pEntity);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd);

	// Functionality
	void DoAimbot(CUserCmd *pCmd);
	void DoNoSpread(CUserCmd *pCmd);
	void DoNoRecoil(CUserCmd *pCmd);

	// AntiAim
	void DoAntiAim(CUserCmd *pCmd);

	// AimStep
	bool IsAimStepping;
	Vector LastAimstepAngle;
	Vector LastAngle;

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
};