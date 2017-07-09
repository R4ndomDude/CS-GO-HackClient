/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"

// All misc visuals (Crosshairs etc) go in this class

class CVisuals : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd);
private:
	void DrawCrosshair();
	void DrawRecoilCrosshair();
	void DrawCenterRadar();
};