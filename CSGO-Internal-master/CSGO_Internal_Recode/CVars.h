#include "SDK.h"

class CVars
{
public:
	CVars(HMODULE hModule);
	~CVars();

	void Load();

public:
	float aimbot_active;
	float aimbot_type;
	float aimbot_autoshoot;
	float aimbot_bone;
	float aimbot_hitbox;
	float aimbot_fov;
	float aimbot_key;
	float aimbot_smooth;
	float aimbot_delay;
	float aimbot_time;
	float aimbot_rcs;
	float aimbot_rcs_delay;
	float aimbot_silent;

	float esp_active;
	float esp_box;
	float esp_health;
	float esp_name;

	float chams_active;
	float chams_visibleonly;
	float chams_flat;

	float misc_bunnyhop;
	float misc_autostrafer;

private:
	HMODULE m_hModule;
};