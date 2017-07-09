#pragma once

#include "sdk.h"

typedef void(__fastcall *PaintTraverseFn)(void*, void*, unsigned int, bool, bool);
extern PaintTraverseFn oPaintTraverse;

void __fastcall PaintTraverse(void* _this, void* _edx, unsigned int panel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(_this, _edx, panel, forceRepaint, allowForce);

	static unsigned int drawPanel;
	if (!drawPanel)
		if (strstr(I::VPanel->GetName(panel), "MatSystemTopPanel"))
			drawPanel = panel;

	if (panel != drawPanel)
		return;

	
}