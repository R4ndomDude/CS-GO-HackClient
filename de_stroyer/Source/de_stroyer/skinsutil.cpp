#include "sdk.h"

std::unordered_map<std::string, int> SkinsUtil::data;

Skins::CSkins* SkinsUtil::pSkins = new Skins::CSkins();

void SkinsUtil::ParseSkins()
{
	char filename[MAX_PATH];
	GetModuleFileName(NULL, filename, MAX_PATH);
	std::string filestr(filename);
	const char* filepath = filestr.substr(0, filestr.size() - 4).c_str();

	if (pSkins->Load(filepath, "csgo")) 
	{
		//pSkins->Dump();
	}

	for (auto& w : pSkins->GetData())
	{
		data.insert(std::pair<std::string, int>(w.first, 0));
	}
}

void SkinsUtil::SaveSkins()
{
	static TCHAR szPath[MAX_PATH];
	static char szFolder[MAX_PATH];
	static char szFile[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		sprintf(szFolder, "%s\\de_stroyed\\", szPath);
		sprintf(szFile, "%s\\de_stroyed\\%s.ini", szPath, "skins");
	}

	CreateDirectory(szFolder, NULL);

	for (auto& skin : data)
	{
		WritePrivateProfileString("Skins", skin.first.c_str(), std::to_string(skin.second).c_str(), szFile);
	}
}

void SkinsUtil::LoadSkins()
{
	static TCHAR szPath[MAX_PATH];
	static char szFolder[MAX_PATH];
	static char szFile[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		sprintf(szFolder, "%s\\de_stroyed\\", szPath);
		sprintf(szFile, "%s\\de_stroyed\\%s.ini", szPath, "skins");
	}

	CreateDirectory(szFolder, NULL);

	char szValue[16];

	for (auto& skin : data)
	{
		GetPrivateProfileString("Skins", skin.first.c_str(), "", szValue, 16, szFile);
		skin.second = atoi(szValue);
	}
}

void SkinsUtil::DumpSkinConfig()
{
	for (auto skin : data)
	{
		Util::PrintMessage("%s -> %d\n", skin.first.c_str(), skin.second);
	}
}