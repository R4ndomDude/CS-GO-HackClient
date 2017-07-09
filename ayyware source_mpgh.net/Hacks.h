#pragma once

#pragma once

#include "SDK.h"
#include "Menu.h"
#include <vector>

namespace Hacks
{
	void SetupHacks();
	void DrawHacks();
	void MoveHacks(CUserCmd *pCmd);
}

class CHack
{
public:
	virtual void Draw() = 0;
	virtual void Move(CUserCmd *pCmd) = 0;
	virtual void Init() = 0;
};

class HackManager
{
public:
	void RegisterHack(CHack* hake);
	void Ready();

	void Draw();
	void Move(CUserCmd *pCmd);
	IClientEntity *pLocal();
private:
	std::vector<CHack*> Hacks;
	IClientEntity *pLocalInstance;
	bool IsReady;
};

extern HackManager hackManager;