#include "sdk.h"

void ICVar::RegisterConCommand(ConVar *pCvar)
{
	typedef void(__thiscall* OriginalFn)(void*, ConVar*);
	return Util::GetVFunc<OriginalFn>(this, 1)(this, pCvar);
}

ConVar* ICVar::FindVar(const char *var_name)
{
	typedef ConVar*(__thiscall* OriginalFn)(void*, const char*);
	return Util::GetVFunc<OriginalFn>(this, 15)(this, var_name);
}