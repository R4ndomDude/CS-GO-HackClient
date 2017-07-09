#include "sdk.h"

void ConVar::SetValue(const char* value)
{
	typedef void(__thiscall* OriginalFn)(void*, const char*);
	return Util::GetVFunc<OriginalFn>(this, 14)(this, value);
}

void ConVar::SetValue(float value)
{
	typedef void(__thiscall* OriginalFn)(void*, float);
	return Util::GetVFunc<OriginalFn>(this, 15)(this, value);
}

void ConVar::SetValue(int value)
{
	typedef void(__thiscall* OriginalFn)(void*, int);
	return Util::GetVFunc<OriginalFn>(this, 16)(this, value);
}

void ConVar::SetValue(Color value)
{
	typedef void(__thiscall* OriginalFn)(void*, Color);
	return Util::GetVFunc<OriginalFn>(this, 17)(this, value);
}

char* ConVar::GetName()
{
	typedef char*(__thiscall* OriginalFn)(void*);
	return Util::GetVFunc<OriginalFn>(this, 5)(this);
}

char* ConVar::GetDefault()
{
	return pszDefaultValue;
}