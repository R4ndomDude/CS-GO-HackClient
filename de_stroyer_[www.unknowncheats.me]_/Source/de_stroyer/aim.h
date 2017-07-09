#pragma once

#include "sdk.h"

namespace Aim
{
	extern bool AimStep(Angle angSource, Angle angDestination, Angle &angOut);
	extern int ClosestAngle(Angle& angDestination);
	extern void CalcAngle(Vector& vecSource, Vector& vecDestination, Angle& angAngles);
	extern Angle Smooth(Angle pOriginal, Angle angDestination, float fSmoothness);
	extern bool IsValidTarget(CBaseEntity* pEntity);
	extern bool Trace(Vector vecSource, Vector vecDestination);
	extern bool Trace(Vector vecSource, Vector vecDestination, CBaseEntity* pSkipEntity);
	extern bool CanWallbang(CBaseEntity* pEntity, int iBone);
	extern bool EdgeAA(float flWall, float flCorner);
}