#pragma once

#include "ibaseclientdll.h"
#include "icliententitylist.h"
#include "iclientmodeshared.h"
#include "icvar.h"
#include "iengineclient.h"
#include "ienginetrace.h"
#include "iinputsystem.h"
#include "iglobalvarsbase.h"
#include "iphysicssurfaceprops.h"
#include "isurface.h"
#include "ivpanel.h"

namespace I // Interfaces
{
	extern IBaseClientDll*			Client;
	extern IClientModeShared*		ClientMode;
	extern IClientEntityList*		ClientEntList;
	extern ICVar*					Cvar;
	extern IInputSystem*			InputSystem;
	extern IEngineClient*			Engine;
	extern IEngineTrace*			EngineTrace;
	extern IGlobalVarsBase*			Globals;
	extern IPhysicsSurfaceProps*	PhysicsSurfaceProps;
	extern ISurface*				Surface;
	extern IVPanel*					VPanel;
}