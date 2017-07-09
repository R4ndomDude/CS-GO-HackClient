#pragma once

#include "DynSkin\Skin\HSkins.h"

namespace SkinsUtil
{
	extern std::unordered_map<std::string, int> data;
	extern Skins::CSkins* pSkins;
	extern void ParseSkins();
	extern void SaveSkins();
	extern void LoadSkins();
	extern void DumpSkinConfig();
}