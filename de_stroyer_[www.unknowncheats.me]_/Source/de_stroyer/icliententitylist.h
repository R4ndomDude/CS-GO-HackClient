#pragma once

class IClientEntityList
{
public:
	virtual void Function0();
	virtual void Function1();
	virtual void Function2();

	virtual CBaseEntity*	GetClientEntity(int iIndex);
	virtual CBaseEntity*	GetClientEntityFromHandle(void* hHandle);
	virtual void			Function3();
	virtual int				GetHighestEntityIndex();
};