#pragma once

typedef bool( __stdcall *CreateMoveFn )(float, CUserCmd*);
extern CreateMoveFn oCreateMove;

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

typedef void( __stdcall* FrameStageNotifyFn )(ClientFrameStage_t);
extern FrameStageNotifyFn oFrameStageNotify;

typedef void( __fastcall *PaintTraverseFn )(void*, void*, unsigned int, bool, bool);
extern PaintTraverseFn oPaintTraverse;

typedef void( __stdcall *OverrideViewFn )(CViewSetup*);
extern OverrideViewFn oOverrideView;

namespace Hooks
{
	extern bool __stdcall CreateMove( float flInputSampleTime, CUserCmd* cmd );
	extern void __stdcall FrameStageNotify( ClientFrameStage_t stage );
	extern bool __stdcall InPrediction( void );
	extern void __fastcall PaintTraverse( void* _this, void* _edx, unsigned int panel, bool forceRepaint, bool allowForce );
	extern void __stdcall OverrideView( CViewSetup* vsView );
}