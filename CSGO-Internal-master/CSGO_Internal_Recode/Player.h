#include "SDK.h"

class CBaseWeapon : public IClientEntity
{
public:
	int GetItemDefinitionIndex();
};

class CPlayer : public IClientEntity
{
public:
	bool IsValid();
	bool IsValid(CPlayer* pPlayer);

	Vector3 GetOrigin();
	Vector3 GetEyePosition();
	Vector3 GetPunchAngles();
	Vector3 GetViewPunchAngles();
	Vector3 GetVelocity();
	Vector3 GetBonePosition(int bone);
	Vector3 GetHitboxPosition(int hitbox);
	bool IsScoped();
	bool GetDormant();
	MoveType_t GetMoveType();
	byte GetLifeState();
	int GetFlags();
	int GetHealth();
	int GetTeam();
	int GetGlowIndex();
	int GetShotsFired();
	int GetHitboxSet();
	int GetUserId();
	UINT* GetActiveWeapon();
	UINT* GetWeapons();

	CBaseWeapon* GetWeap();
};