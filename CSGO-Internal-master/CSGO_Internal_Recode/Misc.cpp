#include "Misc.h"
#include "NetVars.h"
#include "Player.h"

CMisc::CMisc()
{

}

CMisc::~CMisc()
{

}

bool CMisc::IsVisible(Vector3& vecAbsStart, Vector3& vecAbsEnd, IClientEntity* pLocal, IClientEntity* pBaseEnt)
{
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = pLocal;

	ray.Init(vecAbsStart, vecAbsEnd);

	Interfaces.EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

	return (tr.m_pEnt == pBaseEnt || tr.fraction > 0.99f);
}

bool CMisc::WorldToScreen(Vector3 &vOrigin, Vector3 &vScreen)
{
	const matrix3x4& worldToScreen = Interfaces.Engine->WorldToScreenMatrix();

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3];
	vScreen.z = 0;
	if(w > 0.01)
	{
		float inverseWidth = 1 / w;
		vScreen.x = (ScreenSize.Width / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * inverseWidth) * ScreenSize.Width + 0.5);
		vScreen.y = (ScreenSize.Height / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * inverseWidth) * ScreenSize.Height + 0.5);
		return true;
	}
	return false;
}

void CMisc::InitNetVars()
{
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_CollisionGroup", &NetVars.m_rgflCoordinateFrame);
	NetVars.m_rgflCoordinateFrame -= 0x30;

	Hack.NetVars->GetOffset("DT_BaseEntity", "m_Collision", &NetVars.m_Collision);
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_vecOrigin", &NetVars.m_vecOrigin);

	Hack.NetVars->GetOffset("DT_BasePlayer", "m_Local", &NetVars.m_vecPunchAngles);
	NetVars.m_vecPunchAngles += 0x70;

	Hack.NetVars->GetOffset("DT_BasePlayer", "m_Local", &NetVars.m_vecViewPunchAngles);
	NetVars.m_vecViewPunchAngles += 0x64;

	Hack.NetVars->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]", &NetVars.m_vecViewOffset);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_vecVelocity[0]", &NetVars.m_vecVelocity);
	Hack.NetVars->GetOffset("DT_CSPlayer", "m_bIsScoped", &NetVars.m_bIsScoped);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_lifeState", &NetVars.m_lifeState);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_fFlags", &NetVars.m_fFlags);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_iHealth", &NetVars.m_iHealth);
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_iTeamNum", &NetVars.m_iTeamNum);
	Hack.NetVars->GetOffset("DT_CSPlayer", "m_iGlowIndex", &NetVars.m_iGlowIndex);
	Hack.NetVars->GetOffset("DT_CSPlayer", "m_iShotsFired", &NetVars.m_iShotsFired);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_hActiveWeapon", &NetVars.m_hActiveWeapon);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_hMyWeapons", &NetVars.m_hMyWeapons);
	Hack.NetVars->GetOffset("DT_BaseAttributableItem", "m_iItemDefinitionIndex", &NetVars.m_iItemDefinitionIndex);
	Hack.NetVars->GetOffset("DT_BaseAnimating", "m_nHitboxSet", &NetVars.m_nHitboxSet);
	Hack.NetVars->GetOffset("DT_BaseViewModel", "m_nModelIndex", &NetVars.m_nModelIndex);
	Hack.NetVars->GetOffset("DT_BaseViewModel", "m_hOwner", &NetVars.m_hOwner);
	Hack.NetVars->GetOffset("DT_BaseViewModel", "m_hWeapon", &NetVars.m_hWeapon);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_hViewModel", &NetVars.m_hViewModel);
	Hack.NetVars->GetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit", &NetVars.m_nFallbackPaintKit);
	Hack.NetVars->GetOffset("DT_BaseAttributableItem", "m_iItemIDHigh", &NetVars.m_iItemIDHigh);
}

void CMisc::ForceMaterial(IMaterial* material, fColor color)
{
	if(material)
	{
		//float temp[3] = { color.r(), color.g(), color.b() };

		//Interfaces.RenderView->SetBlend(color.a());
		//Interfaces.RenderView->SetColorModulation(temp);

		material->AlphaModulate(color.a());
		material->ColorModulate(color.r(), color.g(), color.b());

		Interfaces.ModelRender->ForcedMaterialOverride(material);
	}
}

std::string CMisc::GetWorkingPath()
{
	char szPath[MAX_PATH];

	GetModuleFileNameA(0, szPath, MAX_PATH);

	std::string path(szPath);

	return path.substr(0, path.find_last_of("\\") + 1);
}

bool CMisc::IsFileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

IMaterial* CMisc::CreateMat(bool bShouldIgnoreZ, bool bShouldFlat)
{
	IMaterial* createdMaterial;

	if(bShouldIgnoreZ)
	{
		if(bShouldFlat)
			createdMaterial = Interfaces.MatSystem->FindMaterial("chamsmat_flat_ignorez");
		else
			createdMaterial = Interfaces.MatSystem->FindMaterial("chamsmat_ignorez");
	}
	else
	{
		if(bShouldFlat)
			createdMaterial = Interfaces.MatSystem->FindMaterial("chamsmat_flat");
		else
			createdMaterial = Interfaces.MatSystem->FindMaterial("chamsmat");
	}

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void CMisc::InitMaterials()
{
	if(!IsFileExists(GetWorkingPath().append("csgo\\materials\\chamsmat.vmt")))
	{
		std::stringstream ss;

		ss << "\"VertexLitGeneric\"" << std::endl;
		ss << "{" << std::endl;
		ss << "\t\"$basetexture\" \"VGUI/white_additive\"" << std::endl;
		ss << "\t\"$ignorez\" \"0\"" << std::endl;
		ss << "\t\"$nofog\" \"1\"" << std::endl;
		ss << "\t\"$model\" \"1\"" << std::endl;
		ss << "\t\"$nocull\" \"1\"" << std::endl;
		ss << "\t\"$halflambert\" \"1\"" << std::endl;
		ss << "}" << std::endl;

		std::ofstream(GetWorkingPath().append("csgo\\materials\\chamsmat.vmt").c_str()) << ss.str();
	}

	if(!IsFileExists(GetWorkingPath().append("csgo\\materials\\chamsmat_ignorez.vmt")))
	{
		std::stringstream ss;

		ss << "\"VertexLitGeneric\"" << std::endl;
		ss << "{" << std::endl;
		ss << "\t\"$basetexture\" \"VGUI/white_additive\"" << std::endl;
		ss << "\t\"$ignorez\" \"1\"" << std::endl;
		ss << "\t\"$nofog\" \"1\"" << std::endl;
		ss << "\t\"$model\" \"1\"" << std::endl;
		ss << "\t\"$nocull\" \"1\"" << std::endl;
		ss << "\t\"$halflambert\" \"1\"" << std::endl;
		ss << "}" << std::endl;

		std::ofstream(GetWorkingPath().append("csgo\\materials\\chamsmat_ignorez.vmt").c_str()) << ss.str();
	}

	if(!IsFileExists(GetWorkingPath().append("csgo\\materials\\chamsmat_flat.vmt")))
	{
		std::stringstream ss;

		ss << "\"UnlitGeneric\"" << std::endl;
		ss << "{" << std::endl;
		ss << "\t\"$basetexture\" \"VGUI/white_additive\"" << std::endl;
		ss << "\t\"$ignorez\" \"0\"" << std::endl;
		ss << "\t\"$nofog\" \"1\"" << std::endl;
		ss << "\t\"$model\" \"1\"" << std::endl;
		ss << "\t\"$nocull\" \"1\"" << std::endl;
		ss << "\t\"$halflambert\" \"1\"" << std::endl;
		ss << "}" << std::endl;

		std::ofstream(GetWorkingPath().append("csgo\\materials\\chamsmat_flat.vmt").c_str()) << ss.str();
	}

	if(!IsFileExists(GetWorkingPath().append("csgo\\materials\\chamsmat_flat_ignorez.vmt")))
	{
		std::stringstream ss;

		ss << "\"UnlitGeneric\"" << std::endl;
		ss << "{" << std::endl;
		ss << "\t\"$basetexture\" \"VGUI/white_additive\"" << std::endl;
		ss << "\t\"$ignorez\" \"1\"" << std::endl;
		ss << "\t\"$nofog\" \"1\"" << std::endl;
		ss << "\t\"$model\" \"1\"" << std::endl;
		ss << "\t\"$nocull\" \"1\"" << std::endl;
		ss << "\t\"$halflambert\" \"1\"" << std::endl;
		ss << "}" << std::endl;

		std::ofstream(GetWorkingPath().append("csgo\\materials\\chamsmat_flat_ignorez.vmt").c_str()) << ss.str();
	}

	Hack.ChamsMat = CreateMat(false, false);
	Hack.ChamsMat_IgnoreZ = CreateMat(true, false);
	Hack.ChamsMat_Flat = CreateMat(false, true);
	Hack.ChamsMat_Flat_IgnoreZ = CreateMat(true, true);
}

void CMisc::ConsolePrint(ConsolePrint_t type, char* msg, ...)
{
	if(!msg)
		return;

	static MsgFn oMsg = nullptr;

	if(type == PRINT_MSG)
		oMsg = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");
	else if(type == PRINT_WARNING)
		oMsg = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Warning");
	else if(type == PRINT_ERROR)
		oMsg = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Error");
	else
		return;

	if(!oMsg)
		return;

	char buffer[989];
	va_list list;

	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);

	oMsg(buffer, list);
}

DWORD FindSig(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	if(!dwAddress || !dwLength || !szPattern)
		return 0;

	const char* pat = szPattern;
	DWORD firstMatch = NULL;

	for(DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if(!*pat)
			return firstMatch;

		if(*(PBYTE)pat == '\?' || *(BYTE*)pCur == GetByte(pat))
		{
			if(!firstMatch)
				firstMatch = pCur;

			if(!pat[2])
				return firstMatch;

			if(*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else pat += 2;
		}
		else
		{
			pat = szPattern;
			firstMatch = 0;
		}
	}

	return 0;
}

DWORD CMisc::FindSignature(const char* szModuleName, char* szPattern)
{
	HMODULE hModule = GetModuleHandleA(szModuleName);
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDOSHeader->e_lfanew);

	return FindSig(((DWORD)hModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hModule) + pNTHeaders->OptionalHeader.SizeOfCode, szPattern);
}

float CMisc::Clamp(float v, float mmin, float mmax)
{
	if(v > mmax) return mmax;
	if(v < mmin) return mmin;

	return v;
}

float CMisc::Rad2Deg(float x)
{
	return (x * (180.0f / PI));
}

float CMisc::Deg2Rad(float x)
{
	return (x * (PI / 180.0f));
}

void CMisc::SinCos(float x, float* s, float* c)
{
	//*s = sin(x);
	//*c = cos(x);

	__asm
	{
		fld dword ptr[x]
		fsincos
		mov edx, dword ptr[c]
		mov eax, dword ptr[s]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void CMisc::SinCos(float x, float &s, float &c)
{
	//s = sin(x);
	//c = cos(x);

	__asm
	{
		fld dword ptr[x]
		fsincos
		mov edx, dword ptr[c]
		mov eax, dword ptr[s]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void CMisc::AngleVectors(const Vector3& angles, Vector3* forward)
{
	float sp, sy, cp, cy;

	SinCos(Deg2Rad(angles.x), &sp, &cp);
	SinCos(Deg2Rad(angles.y), &sy, &cy);

	if(forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
}

void CMisc::AngleVectors(const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(Deg2Rad(angles.x), &sp, &cp);
	SinCos(Deg2Rad(angles.y), &sy, &cy);
	SinCos(Deg2Rad(angles.z), &sr, &cr);

	if(forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if(right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if(up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void CMisc::VectorAngles(const Vector3& forward, Vector3& angles)
{
	float tmp, yaw, pitch;

	if(forward.y == 0 && forward.x == 0)
	{
		yaw = 0;

		if(forward.z > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = Rad2Deg(atan2f(forward.y, forward.x));

		if(yaw < 0)
			yaw += 360;

		tmp = forward.Length2d();
		pitch = Rad2Deg(atan2f(-forward.z, tmp));

		if(pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float CMisc::VectorNormalize(Vector3& v)
{
	float flLength = v.Length();

	if(!flLength)
		v.Set();
	else
		v /= flLength;

	return flLength;
}

void CMisc::AngleNormalize(Vector3& v)
{
	for(auto i = 0; i < 3; i++)
	{
		if(v[i] < -180.0f) v[i] += 360.0f;
		if(v[i] >  180.0f) v[i] -= 360.0f;
	}
}

void CMisc::ClampAngles(Vector3& v)
{
	if(v.x > 89.0f && v.x <= 180.0f)
		v.x = 89.0f;

	if(v.x > 180.0f)
		v.x = v.x - 360.0f;

	if(v.x < -89.0f)
		v.x = -89.0f;

	v.y = fmodf(v.y + 180, 360) - 180;

	v.z = 0;
}

void CMisc::VectorTransform(const Vector3& vSome, const matrix3x4& vMatrix, Vector3& vOut)
{
	for(auto i = 0; i < 3; i++)
		vOut[i] = vSome.Dot((Vector3&)vMatrix[i]) + vMatrix[i][3];
}

bool CMisc::IsRcsWeapon(int weaponid)
{
	//pls

	if(IsWeaponPistol(weaponid) || IsWeaponSniper(weaponid) || IsWeaponShotgun(weaponid))
		return false;

	return true;
}

bool CMisc::IsNonAimWeapon(int weaponid)
{
	//pls

	if(IsWeaponKnife(weaponid) || IsWeaponGrenade(weaponid) || IsWeaponBomb(weaponid) || IsWeaponTaser(weaponid))
		return true;

	return false;
}

bool CMisc::IsWeaponPistol(int weaponid)
{
	//pls

	if(weaponid == WEAPON_DEAGLE || weaponid == WEAPON_ELITE || weaponid == WEAPON_FIVESEVEN || weaponid == WEAPON_GLOCK || weaponid == WEAPON_HKP2000 || weaponid == WEAPON_P250 || weaponid == WEAPON_TEC9 || weaponid == WEAPON_USP_SILENCER)
		return true;

	return false;
}

bool CMisc::IsWeaponSniper(int weaponid)
{
	//pls

	if(weaponid == WEAPON_AWP || weaponid == WEAPON_G3SG1 || weaponid == WEAPON_SCAR20 || weaponid == WEAPON_SSG08)
		return true;

	return false;
}

bool CMisc::IsWeaponKnife(int weaponid)
{
	//pls

	if(weaponid == WEAPON_KNIFE || weaponid == WEAPON_KNIFE_T || weaponid == WEAPON_KNIFEGG || weaponid == WEAPON_KNIFE_BAYONET || weaponid == WEAPON_KNIFE_BUTTERFLY || weaponid == WEAPON_KNIFE_FALCHION || weaponid == WEAPON_KNIFE_FLIP || weaponid == WEAPON_KNIFE_GUT || weaponid == WEAPON_KNIFE_KARAMBIT || weaponid == WEAPON_KNIFE_M9_BAYONET || weaponid == WEAPON_KNIFE_PUSH || weaponid == WEAPON_KNIFE_TACTICAL)
		return true;

	return false;
}

bool CMisc::IsWeaponGrenade(int weaponid)
{
	//pls

	if(weaponid == WEAPON_FLASHBANG || weaponid == WEAPON_HEGRENADE || weaponid == WEAPON_SMOKEGRENADE || weaponid == WEAPON_MOLOTOV || weaponid == WEAPON_INCGRENADE || weaponid == WEAPON_DECOY)
		return true;

	return false;
}

bool CMisc::IsWeaponBomb(int weaponid)
{
	//pls

	if(weaponid == WEAPON_C4)
		return true;

	return false;
}

bool CMisc::IsWeaponTaser(int weaponid)
{
	//pls

	if(weaponid == WEAPON_TASER)
		return true;

	return false;
}

bool CMisc::IsWeaponShotgun(int weaponid)
{
	//pls

	if(weaponid == WEAPON_NOVA || weaponid == WEAPON_SAWEDOFF || weaponid == WEAPON_XM1014 || weaponid == WEAPON_MAG7)
		return true;

	return false;
}