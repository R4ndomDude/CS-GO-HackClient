#include "sdk.h"

bool G::Aimbotting = false;
Angle G::AimPunch = Angle();
Angle G::LastAngle = Angle();
CBaseEntity* G::LocalPlayer = NULL;
bool G::Return = true;
CUserCmd* G::UserCmd = NULL;