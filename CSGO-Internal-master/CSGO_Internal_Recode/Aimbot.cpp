#include "Aimbot.h"
#include "Misc.h"
#include "Player.h"
#include "CVars.h"

CAimbot::CAimbot()
{
	m_pCmd = nullptr;

	m_pLocal = nullptr;

	m_iBestIndex = -1;
	m_flBestTarget = 0.0f;

	m_iTimeCounter = 0;
}

CAimbot::~CAimbot()
{
}

void MakeVector(const Vector3& vIn, Vector3& vOut)
{
	float pitch = Hack.Misc->Deg2Rad(vIn.x);
	float yaw = Hack.Misc->Deg2Rad(vIn.y);
	float temp = cos(pitch);

	vOut.x = -temp * -cos(yaw);
	vOut.y = sin(yaw) * temp;
	vOut.z = -sin(pitch);
}

float GetFOV(const Vector3& viewangles, const Vector3& vStart, const Vector3& vEnd)
{
	Vector3 vAng, vAim;

	Vector3 vDir = vEnd - vStart;

	Hack.Misc->VectorNormalize(vDir);

	Hack.Misc->VectorAngles(vDir, vAng);

	MakeVector(viewangles, vAim);
	MakeVector(vAng, vAng);

	return Hack.Misc->Rad2Deg(acos(vAim.Dot(vAng)) / vAim.LengthSqr());
}

void CAimbot::Main(CUserCmd* pCmd, float frametime)
{
	if(Hack.CVars->aimbot_type == 0)
		return;

	m_pCmd = pCmd;
	if(!m_pCmd)
		return;

	m_pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
	if(!m_pLocal)
		return;

	m_iWeaponId = m_pLocal->GetWeap()->GetItemDefinitionIndex();

	if(Hack.CVars->aimbot_key)
	{
		if(!GetAsyncKeyState(Hack.CVars->aimbot_key) || m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}
	else
	{
		if(m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}

	DropTarget();

	m_iTimeCounter++;
	m_iTimeCounter += frametime;

	if(Hack.CVars->aimbot_delay)
	{
		if(m_iTimeCounter < Hack.CVars->aimbot_delay)
			return;
	}

	if(Hack.CVars->aimbot_time)
	{
		if(m_iTimeCounter > Hack.CVars->aimbot_time)
			return;
	}

	if(m_iBestIndex != -1)
	{
		Vector3 vDirection = m_vEnd - m_pLocal->GetEyePosition();

		Hack.Misc->VectorNormalize(vDirection);

		Vector3 vAim;

		Hack.Misc->VectorAngles(vDirection, vAim);

		if(Hack.Misc->IsNonAimWeapon(m_iWeaponId))
			return;

		if(Hack.CVars->aimbot_rcs && Hack.Misc->IsRcsWeapon(m_iWeaponId))
		{
			if(m_pLocal->GetShotsFired() >= (int)Hack.CVars->aimbot_rcs_delay)
				vAim -= m_pLocal->GetPunchAngles() * 2.0f;
		}
		
		if(Hack.CVars->aimbot_smooth > 0.0f)
		{
			Vector3 vDelta(m_pCmd->viewangles - vAim);

			Hack.Misc->AngleNormalize(vDelta);

			vAim = m_pCmd->viewangles - vDelta / Hack.CVars->aimbot_smooth;
		}

		vAim.z = 0.0f;

		Hack.Misc->ClampAngles(vAim);

		m_pCmd->viewangles = vAim;

		if(!Hack.CVars->aimbot_silent)
			Interfaces.Engine->SetViewAngles(m_pCmd->viewangles);

		if(Hack.CVars->aimbot_autoshoot)
		{
			m_pCmd->buttons |= IN_ATTACK;
		}
	}
}

void CAimbot::DropTarget()
{
	if(Hack.CVars->aimbot_key)
	{
		if(!Valid(m_iBestIndex) || !GetAsyncKeyState(Hack.CVars->aimbot_key))
			m_iBestIndex = -1;
	}
	else
	{
		if(!Valid(m_iBestIndex))
			m_iBestIndex = -1;
	}
}

void CAimbot::GetBestTarget()
{
	m_flBestTarget = Hack.CVars->aimbot_fov;

	if(!m_pLocal)
		return;

	if(m_pLocal->GetLifeState() != LIFE_ALIVE)
		return;

	for(auto i = 1; i <= Interfaces.Engine->GetMaxClients(); i++)
	{
		if(Valid(i))
		{
			float flFOV = GetFOV(m_pCmd->viewangles, m_pLocal->GetEyePosition(), m_vEnd);

			if(flFOV < m_flBestTarget)
			{
				m_flBestTarget = flFOV;
				m_iBestIndex = i;
			}
		}
	}
}

bool CAimbot::Valid(int index)
{
	if(!m_pLocal)
		return false;

	CPlayer* pEntity = Interfaces.EntList->GetClientEntity<CPlayer>(index);
	if(!pEntity)
		return false;

	if(!pEntity)
		return false;

	if(pEntity->IsDormant())
		return false;

	if(pEntity->GetLifeState() != 0)
		return false;

	if(pEntity->GetHealth() > 500)
		return false;

	if(pEntity->GetTeam() == m_pLocal->GetTeam())
		return false;

	if(Hack.CVars->aimbot_type == 1)
		m_vEnd = pEntity->GetBonePosition(Hack.CVars->aimbot_bone);
	else if(Hack.CVars->aimbot_type == 2)
		m_vEnd = pEntity->GetHitboxPosition(Hack.CVars->aimbot_hitbox);
	else
		m_vEnd.Set();

	if(m_vEnd.IsZero())
		return false;

	if(!Hack.Misc->IsVisible(m_pLocal->GetEyePosition(), m_vEnd, m_pLocal, pEntity))
		return false;

	return true;
}