#include "sdk.h"

PaintTraverseFn oPaintTraverse;
void __fastcall Hooks::PaintTraverse( void* _this, void* _edx, unsigned int panel, bool forceRepaint, bool allowForce )
{
	oPaintTraverse( _this, _edx, panel, forceRepaint, allowForce );

	static unsigned int drawPanel;
	if(!drawPanel)
		if(strstr( I::VPanel->GetName( panel ), "MatSystemTopPanel" ))
			drawPanel = panel;

	if(panel != drawPanel)
		return;

	Cheat::Run_ESP( );
	Menu->Paint( );

	//Draw::DrawRoundedBox(20, 20, 25, 25, 2, 5, Color(255, 255, 255, 255));
}