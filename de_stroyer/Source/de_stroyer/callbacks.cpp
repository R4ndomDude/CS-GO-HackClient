#include "sdk.h"

void MenuCallbacks::SaveSkins()
{
	SkinsUtil::SaveSkins();
	SkinsUtil::DumpSkinConfig();
	Util::PrintMessage("Saved Skins\n");
}

void MenuCallbacks::LoadSkins()
{
	SkinsUtil::LoadSkins();
	Util::PrintMessage("Loaded Skins\n");
}