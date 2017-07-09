#include "sdk.h"

template< class T, class Y >
inline T clamp(T const &val, Y const &minVal, Y const &maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else return val;
}

bool Aim::AimStep(Angle angSource, Angle angDestination, Angle &angOut)
{
	angDestination.Normalize();
	Angle angDelta = (angDestination - angSource).Normalize();;
	int iStepAmount = 15.f;
	bool bXFinished = false;
	bool bYFinished = false;

	if(angDelta.x > iStepAmount)
		angSource.x += iStepAmount;
	else if(angDelta.x < -iStepAmount)
		angSource.x -= iStepAmount;
	else
	{
		bXFinished = true;
		angSource.x = angDestination.x;
	}

	G::UserCmd->viewangles.Normalize();

	if(angDelta.y > iStepAmount)
		angSource.y += iStepAmount;
	else if(angDelta.y < -iStepAmount)
		angSource.y -= iStepAmount;
	else
	{
		bYFinished = true;
		angSource.y = angDestination.y;
	}

	angSource.Normalize();

	angOut = angSource;

	return bXFinished && bYFinished;

	/*angDestination.Normalize();
	Angle angDelta = (angDestination - angSource).Normalize();;
	static float stepamount = 10.f;
	bool bXFinished = true;
	bool bYFinished = true;

	angSource.x += Util::Clamp(angDelta.x, -stepamount, stepamount);
	angSource.y += Util::Clamp(angDelta.y, -stepamount, stepamount);

	if ((angDelta.x < -stepamount) || (angDelta.x > stepamount))
		bXFinished = false;

	if ((angDelta.y < -stepamount) || (angDelta.y > stepamount))
		bXFinished = false;

	angSource.Normalize();

	angOut = angSource;

	return bXFinished && bYFinished;*/
}

int Aim::ClosestAngle(Angle& angDestination)
{
	bool bFOVCheck = Config->GetValue("Aimbot", "FOV Check").m_bValue;
	float flFOV = Config->GetValue("Aimbot", "FOV").m_flValue;
	int iBone = Config->GetValue("Aimbot", "Bone").m_iValue;

	static Vector vecLocalPos;
	static Vector vecEntityPos;
	Angle angAimAngles[64];

	int iClosestIndex = -1;
	float flClosestFOV = bFOVCheck ? flFOV : 360.f;
	
	for (int i = 0; i < 64; i++)
	{
		auto pEntity = I::ClientEntList->GetClientEntity(i);

		if (!Aim::IsValidTarget(pEntity)) continue;

		vecLocalPos = G::LocalPlayer->GetEyePosition();
		vecEntityPos = pEntity->GetBonePosition(iBone);

		Aim::CalcAngle(vecLocalPos, vecEntityPos, angAimAngles[i]);
		angAimAngles[i].Normalize();

		float flDifference = G::UserCmd->viewangles.Difference(angAimAngles[i]);
		if (flDifference < flClosestFOV)
		{
			iClosestIndex = i;
			flClosestFOV = flDifference;
		}
	}

	angDestination = angAimAngles[iClosestIndex];
	return iClosestIndex;
}

void Aim::CalcAngle(Vector& vecSource, Vector& vecDestination, Angle& angAngles)
{
	Vector vDelta = vecSource - vecDestination;
	float hyp = sqrt(vDelta.x * vDelta.x + vDelta.y * vDelta.y);
	angAngles.x = atan(vDelta.z / hyp) * 57.295779513082f;
	angAngles.y = atan(vDelta.y / vDelta.x) * 57.295779513082f;
	angAngles.z = 0.f;
	if (vDelta.x >= 0.f) angAngles.y += 180.f;
}

Angle Aim::Smooth(Angle pOriginal, Angle angDestination, float fSmoothness)
{
	Angle angSmooth;
	angSmooth.Init();

	angSmooth.x = angDestination.x - pOriginal.x;
	angSmooth.y = angDestination.y - pOriginal.y;

	angSmooth.Normalize();

	angSmooth.x = pOriginal.x + angSmooth.x / 100 * fSmoothness;
	angSmooth.y = pOriginal.y + angSmooth.y / 100 * fSmoothness;

	angSmooth.Normalize();

	return angSmooth;
}

bool Aim::IsValidTarget(CBaseEntity* pEntity)
{
	bool bIgnoreTeam = Config->GetValue("Aimbot", "Ignore Team").m_bValue;
	bool bVisCheck = Config->GetValue("Aimbot", "Visibility Check").m_bValue;
	bool bAutoWall = Config->GetValue("Aimbot", "Autowall").m_bValue;

	if (!pEntity) return false;
	if (pEntity == G::LocalPlayer) return false;
	if (!pEntity->GetAlive()) return false;
	if (pEntity->GetHealth() <= 0 || pEntity->GetHealth() > 100) return false;
	if (pEntity->GetDormant()) return false;
	if (pEntity->GetImmune()) return false;
	if ((pEntity->GetTeam() == G::LocalPlayer->GetTeam()) && bIgnoreTeam) return false;
	if (bVisCheck && !bAutoWall && !Aim::Trace(G::LocalPlayer->GetEyePosition(), pEntity->GetBonePosition(6))) return false;
	if (bAutoWall && !Aim::CanWallbang(pEntity, 6)) return false;

	return true;
}

bool Aim::Trace(Vector vecSource, Vector vecDestination)
{
	Ray_t ray;
	trace_t tr;
	CTraceFilter filter;
	filter.pSkip = G::LocalPlayer;
	ray.Init(vecSource, vecDestination);
	I::EngineTrace->TraceRay(ray, 0x4602400B, &filter, &tr);

	return tr.fraction > 0.97f;
}

bool Aim::Trace(Vector vecSource, Vector vecDestination, CBaseEntity* pSkipEntity)
{
	Ray_t ray;
	trace_t tr;
	CTraceFilterSkipTwoEntities filter(G::LocalPlayer, pSkipEntity);
	ray.Init(vecSource, vecDestination);
	I::EngineTrace->TraceRay(ray, 0x4602400B, &filter, &tr);

	return tr.m_pEnt == pSkipEntity || tr.fraction > 0.97f;
}

bool Aim::CanWallbang(CBaseEntity* pEntity, int iBone)
{
	trace_t tr;
	Ray_t ray;
	Vector vStart, vEnd, vEndPos[3];
	vStart = G::LocalPlayer->GetEyePosition();
	vEnd = pEntity->GetBonePosition(iBone);
	CTraceFilter filter;

	if (Aim::Trace(vStart, vEnd, pEntity))
		return true;

	filter.pSkip = Util::GetLocalPlayer();
	ray.Init(vStart, vEnd);

	I::EngineTrace->TraceRay(ray, 0x4602400B, &filter, &tr);

	vEndPos[0] = tr.endpos;

	ray.Init(vEnd, vStart);
	filter.pSkip = pEntity;
	I::EngineTrace->TraceRay(ray, 0x4602400B, &filter, &tr);

	vEndPos[1] = tr.endpos;

	vEndPos[2] = vEndPos[0] - vEndPos[1];

	float flLength = vEndPos[2].Length();

	WeaponInfo_t* info = G::LocalPlayer->GetWeapon()->GetCSWpnData();
	float flMaxLength = 0.f;

	if(info->m_flPenetration == 2.5f)
		flMaxLength = 24.5f;
	else if(info->m_flPenetration == 2.f)
		flMaxLength = 17.5f;
	else if(info->m_flPenetration == 1.f)
		flMaxLength = 7.5f;

	if (tr.fraction != 0)
		if(flLength < flMaxLength)
			return true;

	return false;
}

bool EdgeAA(float flWall, float flCorner)
{
	/*Ray_t ray;
	trace_t tr;

	CTraceFilter traceFilter;
	traceFilter.pSkip = G::LocalPlayer;

	auto bRetVal = false;
	auto vecCurPos = G::LocalPlayer->GetEyePosition();

	for (float i = 0; i < 360; i++)
	{
		Angle vecDummy(10.f, G::UserCmd->viewangles.y, 0.f);
		vecDummy.y += i;

		vecDummy.Normalize();

		Vector vecForward;
		g_pTools->AngleVectors(vecDummy, vecForward);

		auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
		vecForward *= flLength;

		ray.Init(vecCurPos, (vecCurPos + vecForward));
		g_pEngineTrace->TraceRay(ray, MASK_SHOT, (ValveSDK::IEngineTrace::CTraceFilter *)&traceFilter, &tr);

		if (tr.fraction != 1.0f)
		{
			ValveSDK::QAngle qAngles;
			auto vecNegate = tr.plane.normal;

			vecNegate *= -1.f;
			g_pTools->VectorAngles(vecNegate, qAngles);

			vecDummy.y = qAngles.y;

			g_pTools->NormalizeVector(vecDummy);
			ValveSDK::IEngineTrace::trace_t leftTrace, rightTrace;

			ValveSDK::Vector vecLeft;
			g_pTools->AngleVectors(vecDummy + ValveSDK::Vector(0.f, 30.f, 0.f), vecLeft);

			ValveSDK::Vector vecRight;
			g_pTools->AngleVectors(vecDummy - ValveSDK::Vector(0.f, 30.f, 0.f), vecRight);

			vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
			vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

			ray.Init(vecCurPos, (vecCurPos + vecLeft));
			g_pEngineTrace->TraceRay(ray, MASK_SHOT, (ValveSDK::IEngineTrace::CTraceFilter*)&traceFilter, &leftTrace);

			ray.Init(vecCurPos, (vecCurPos + vecRight));
			g_pEngineTrace->TraceRay(ray, MASK_SHOT, (ValveSDK::IEngineTrace::CTraceFilter*)&traceFilter, &rightTrace);

			if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				vecDummy.y -= flCornor; // left
			else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				vecDummy.y += flCornor; // right			

			cmd->viewangles.y = vecDummy.y;
			cmd->viewangles.y -= flWall;
			bRetVal = true;
		}
	}*/
}