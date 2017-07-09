#include "SDK.h"

class CPlayer;
class CMisc
{
public:
	CMisc();
	~CMisc();

	bool IsVisible(Vector3& vecAbsStart, Vector3& vecAbsEnd, IClientEntity* pLocal, IClientEntity* pBaseEnt);
	bool WorldToScreen(Vector3 &vOrigin, Vector3 &vScreen);
	void InitNetVars();

	void ForceMaterial(IMaterial* material, fColor color);
	std::string GetWorkingPath();
	bool IsFileExists(const std::string& name);
	IMaterial* CreateMat(bool bShouldIgnoreZ, bool bShouldFlat);
	void InitMaterials();

	void ConsolePrint(ConsolePrint_t type, char* msg, ...);
	DWORD FindSignature(const char* szModuleName, char* szPattern);

	float Clamp(float v, float mmin, float mmax);
	float Deg2Rad(float x);
	float Rad2Deg(float x);
	void SinCos(float x, float* s, float* c);
	void SinCos(float x, float &s, float &c);
	void AngleVectors(const Vector3& angles, Vector3* forward);
	void AngleVectors(const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up);
	void VectorAngles(const Vector3& forward, Vector3& angles);
	float VectorNormalize(Vector3& v);
	void AngleNormalize(Vector3& v);
	void VectorTransform(const Vector3& vSome, const matrix3x4& vMatrix, Vector3& vOut);
	void ClampAngles(Vector3& v);

	//pls
	bool	IsRcsWeapon(int weaponid);
	bool	IsNonAimWeapon(int weaponid);
private:
	bool	IsWeaponPistol(int weaponid);
	bool	IsWeaponSniper(int weaponid);
	bool	IsWeaponKnife(int weaponid);
	bool	IsWeaponGrenade(int weaponid);
	bool	IsWeaponBomb(int weaponid);
	bool	IsWeaponTaser(int weaponid);
	bool	IsWeaponShotgun(int weaponid);
};