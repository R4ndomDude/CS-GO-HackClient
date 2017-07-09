#include "sdk.h"

void IEngineClient::GetScreenSize( int &width, int &height )
{
	typedef void( __thiscall* OriginalFn )(void*, int&, int&);
	return Util::GetVFunc<OriginalFn>( this, 5 )(this, width, height);
}

void IEngineClient::GetPlayerInfo( int ent_num, player_info_t* pinfo )
{
	typedef void( __thiscall* OriginalFn )(void*, int ent_num, player_info_t* pinfo);
	return Util::GetVFunc<OriginalFn>( this, 8 )(this, ent_num, pinfo);
}

int IEngineClient::GetLocalPlayer( )
{
	typedef int( __thiscall* OriginalFn )(void*);
	return Util::GetVFunc<OriginalFn>( this, 12 )(this);
}

float IEngineClient::GetLastTimeStamp( )
{
	typedef float( __thiscall* OriginalFn )(void*);
	return Util::GetVFunc<OriginalFn>( this, 14 )(this);
}

bool IEngineClient::IsInGame( )
{
	typedef bool( __thiscall* OriginalFn )(void*);
	return Util::GetVFunc<OriginalFn>( this, 26 )(this);
}

const VMatrix& IEngineClient::WorldToScreenMatrix( )
{
	typedef VMatrix&(__thiscall* OriginalFn)(void*);
	return Util::GetVFunc<OriginalFn>( this, 37 )(this);
}

void IEngineClient::ClientCmd_Unrestricted( const char* szCmdString )
{
	typedef void( __thiscall* OriginalFn )(void*, const char* szCmdString);
	return Util::GetVFunc<OriginalFn>( this, 114 )(this, szCmdString);
}