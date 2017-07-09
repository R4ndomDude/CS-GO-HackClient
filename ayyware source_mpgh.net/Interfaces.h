/*
Syn's AyyWare Framework
*/

#pragma once

// Includes
#include "SDK.h"

// Namespace to contain all the valve interfaces
namespace Interfaces
{
	// Gets handles to all the interfaces needed
	void Initialise();

	extern IBaseClientDLL* Client;
	//extern IVEngineClient* Engine;
	extern IPanel* Panels;
	extern IClientEntityList* EntList;
	extern ISurface* Surface;
	extern IVDebugOverlay* DebugOverlay;
	extern DWORD *ClientMode;
	extern CGlobalVarsBase *Globals;
	extern DWORD *Prediction;
	extern CMaterialSystem* MaterialSystem;
	extern CVRenderView* RenderView;
	extern IVModelRender* ModelRender;
	extern CModelInfo* ModelInfo;
	extern IEngineTrace* Trace;
	extern IPhysicsSurfaceProps* PhysProps;
	extern ICVar *CVar;
};