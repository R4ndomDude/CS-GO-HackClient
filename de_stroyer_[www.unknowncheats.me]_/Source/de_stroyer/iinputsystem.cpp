#include "sdk.h"

void IInputSystem::EnableInput(bool bEnable)
{
	typedef void(__thiscall* OriginalFn)(void*, bool);
	return Util::GetVFunc<OriginalFn>(this, 11)(this, bEnable);
}

void IInputSystem::ResetInputState()
{
	typedef void(__thiscall* OriginalFn)(void*);
	return Util::GetVFunc<OriginalFn>(this, 39)(this);
}