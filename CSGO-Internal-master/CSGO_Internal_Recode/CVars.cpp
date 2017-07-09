#include "CVars.h"

float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
	char szData[32];

	GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

	return (float)atof(szData);
}

void WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flValue, LPCSTR lpFileName)
{
	WritePrivateProfileStringA(lpAppName, lpKeyName, std::to_string((int)flValue).c_str(), lpFileName);
}

CVars::CVars(HMODULE hModule)
{
	m_hModule = hModule;
}

CVars::~CVars()
{

}

void CVars::Load()
{
	char szPath[MAX_PATH];
	GetModuleFileNameA(m_hModule, szPath, MAX_PATH);
	std::string path(szPath);

	path = path.substr(0, path.find_last_of("\\") + 1);
	path.append("autoexec.cfg");

	aimbot_active = GetPrivateProfileFloat("main", "aimbot.active", 0, path.c_str());
	aimbot_key = GetPrivateProfileFloat("main", "aimbot.key", 0, path.c_str());
	aimbot_type = GetPrivateProfileFloat("main", "aimbot.type", 0, path.c_str());
	aimbot_autoshoot = GetPrivateProfileFloat("main", "aimbot.autoshoot", 0, path.c_str());
	aimbot_bone = GetPrivateProfileFloat("main", "aimbot.bone", 0, path.c_str());
	aimbot_hitbox = GetPrivateProfileFloat("main", "aimbot.hitbox", 0, path.c_str());
	aimbot_fov = GetPrivateProfileFloat("main", "aimbot.fov", 0, path.c_str());
	aimbot_smooth = GetPrivateProfileFloat("main", "aimbot.smooth", 0, path.c_str());
	aimbot_delay = GetPrivateProfileFloat("main", "aimbot.delay", 0, path.c_str());
	aimbot_time = GetPrivateProfileFloat("main", "aimbot.time", 0, path.c_str());
	aimbot_rcs = GetPrivateProfileFloat("main", "aimbot.rcs", 0, path.c_str());
	aimbot_rcs_delay = GetPrivateProfileFloat("main", "aimbot.rcs.delay", 0, path.c_str());
	aimbot_silent = GetPrivateProfileFloat("main", "aimbot.silent", 0, path.c_str());

	esp_active = GetPrivateProfileFloat("main", "esp.active", 0, path.c_str());
	esp_box = GetPrivateProfileFloat("main", "esp.box", 0, path.c_str());
	esp_health = GetPrivateProfileFloat("main", "esp.health", 0, path.c_str());
	esp_name = GetPrivateProfileFloat("main", "esp.name", 0, path.c_str());

	chams_active = GetPrivateProfileFloat("main", "chams.active", 0, path.c_str());
	chams_visibleonly = GetPrivateProfileFloat("main", "chams.visibleonly", 0, path.c_str());
	chams_flat = GetPrivateProfileFloat("main", "chams.flat", 0, path.c_str());

	misc_bunnyhop = GetPrivateProfileFloat("main", "misc.bunnyhop", 0, path.c_str());
	misc_autostrafer = GetPrivateProfileFloat("main", "misc.autostrafer", 0, path.c_str());
}