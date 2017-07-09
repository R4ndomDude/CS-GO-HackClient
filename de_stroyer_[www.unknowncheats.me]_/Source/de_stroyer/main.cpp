#include "sdk.h"
#include "hooks.h"

void CheatThread()
{
	Util::CaptureInterfaces();

	Util::NetVars->Initialize();
	Offsets::GrabOffsets();
	//delete Util::NetVars;

	Config->Setup();
	Draw::GrabFonts();
	Menu->Setup();

	oPaintTraverse = (PaintTraverseFn)Util::Hook(I::VPanel, 41, Hooks::PaintTraverse);
	oCreateMove = (CreateMoveFn)Util::Hook(I::ClientMode, 24, Hooks::CreateMove);
	oFrameStageNotify = (FrameStageNotifyFn)Util::Hook(I::Client, 36, Hooks::FrameStageNotify);
	oOverrideView = (OverrideViewFn)Util::Hook(I::ClientMode, 18, Hooks::OverrideView);

	Config->DumpConfig();

	while (SkinsUtil::pSkins->GetSkinInfo().size() == 0)
	{
		SkinsUtil::ParseSkins();
		Sleep(100);
	}

	while (true)
	{
		if (GetAsyncKeyState(VK_DELETE))
		{
			Util::PrintMessage("UNHOOKED.");
			Util::Unhook(I::ClientMode, 24, Hooks::CreateMove);
			Util::Unhook(I::VPanel, 41, Hooks::PaintTraverse);
		}

		Sleep(1);
	}
}

DWORD WINAPI DllMain(HINSTANCE hiInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason != DLL_PROCESS_ATTACH) return TRUE;

	if (!GetModuleHandle("client.dll")) Sleep(300); 
	if (!GetModuleHandle("engine.dll")) Sleep(300);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheatThread, NULL, NULL, NULL);

	return TRUE;
}