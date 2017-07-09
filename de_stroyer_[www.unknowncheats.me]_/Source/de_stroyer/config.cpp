#include "sdk.h"

void CConfig::Setup()
{
	Config->SetValue("Aimbot", "Enabled", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Use Key", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Key", ConfigValue(VK_XBUTTON2, CONFIG_INT));
	Config->SetValue("Aimbot", "Bone", ConfigValue(6, CONFIG_INT));
	Config->SetValue("Aimbot", "RCS", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "RCS Amount", ConfigValue(100.f, CONFIG_FLOAT));
	Config->SetValue("Aimbot", "Visibilty Check", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Silent", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Spinbot", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Aim Step", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Autowall", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Smoothing", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Smoothness", ConfigValue(5.f, CONFIG_FLOAT));
	Config->SetValue("Aimbot", "Closest Angle", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "FOV Check", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Aimbot", "FOV", ConfigValue(90.f, CONFIG_FLOAT));
	Config->SetValue("Aimbot", "Hit Chance", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Hit Chance Amount", ConfigValue(98.78f, CONFIG_FLOAT));
	Config->SetValue("Aimbot", "Auto Shoot", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Auto Stop", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Auto Crouch", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Aimbot", "Ignore Team", ConfigValue(true, CONFIG_BOOL));

	Config->SetValue("Visuals", "Enabled", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Visibility Check", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Name", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Box", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Bone Box", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Line", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Health Bar", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Health Text", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Chams", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Chams Lit", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Visuals", "Chams Wireframe", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Visuals", "No Hands", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Visuals", "Rainbow Hands", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Enemy Only", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Visuals", "Recoil Crosshair", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "No Visual Recoil", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "Custom FOV", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Visuals", "FOV Amount", ConfigValue(100.f, CONFIG_FLOAT));
	Config->SetValue("Visuals", "Team Color", ConfigValue(.39f, CONFIG_FLOAT));
	Config->SetValue("Visuals", "Enemy Color", ConfigValue(.75f, CONFIG_FLOAT));
	
	Config->SetValue("Misc", "Bhop", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Misc", "Auto Strafe", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Misc", "Name Steal", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Misc", "Achievement Spam", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Misc", "Location Spam", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Misc", "Air Stuck", ConfigValue(false, CONFIG_BOOL));
	Config->SetValue("Misc", "Air Stuck Key", ConfigValue(VK_XBUTTON1, CONFIG_INT));

	Config->SetValue("Skins", "Enabled", ConfigValue(true, CONFIG_BOOL));
	Config->SetValue("Skins", "StatTrak", ConfigValue(true, CONFIG_BOOL));

	Config->SetValue("Menu", "Key", ConfigValue(VK_INSERT, CONFIG_INT));
}

void CConfig::SetValue(std::string strGroup, std::string strVariable, ConfigValue nValue)
{
	bool bFoundGroup = false, bFoundVariable = false;
	for (auto pGroup : groups)
	{
		if (pGroup->m_strName == strGroup)
		{
			bFoundGroup = true;
			for (auto pVariable : pGroup->m_Variables)
			{
				if (pVariable->m_strName == strVariable)
				{
					bFoundVariable = true;
					pVariable->m_nValue = nValue;
				}
			}

			if (!bFoundVariable)
			{
				ConfigVariable* pVariable = new ConfigVariable(strVariable, nValue);
				pGroup->m_Variables.push_back(pVariable);
			}
		}
	}

	if (!bFoundGroup)
	{
		ConfigGroup* pGroup = new ConfigGroup(strGroup);
		
		ConfigVariable* pVariable = new ConfigVariable(strVariable, nValue);
		pGroup->m_Variables.push_back(pVariable);

		groups.push_back(pGroup);
	}
}

ConfigValue CConfig::GetValue(std::string strGroup, std::string strVariable)
{
	return *GetValuePtr(strGroup, strVariable);
}

ConfigValue* CConfig::GetValuePtr(std::string strGroup, std::string strVariable)
{
	for (auto pGroup : groups)
	{
		if (pGroup->m_strName == strGroup)
		{
			for (auto pVariable : pGroup->m_Variables)
			{
				if (pVariable->m_strName == strVariable)
				{
					return &pVariable->m_nValue;
				}
			}
		}
	}

	return new ConfigValue();
}

void CConfig::SaveConfig(std::string strName)
{
	static TCHAR szPath[MAX_PATH];
	static char szFolder[MAX_PATH];
	static char szFile[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		sprintf(szFolder, "%s\\de_stroyed\\", szPath);
		sprintf(szFile, "%s\\de_stroyed\\%s.ini", szPath, strName.c_str());
	}

	CreateDirectory(szFolder, NULL);

	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			switch (pVariable->m_nValue.m_Type)
			{
			case CONFIG_INT:
				WritePrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), std::to_string(pVariable->m_nValue.m_iValue).c_str(), szFile);
				break;
			case CONFIG_FLOAT:
				WritePrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), std::to_string(pVariable->m_nValue.m_flValue).c_str(), szFile);
				break;
			case CONFIG_BOOL:
				WritePrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), pVariable->m_nValue.m_bValue ? "true" : "false", szFile);
				break;
			}
		}
	}
}

void CConfig::LoadConfig(std::string strName)
{
	static TCHAR szPath[MAX_PATH];
	static char szFolder[MAX_PATH];
	static char szFile[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		sprintf(szFolder, "%s\\de_stroyed\\", szPath);
		sprintf(szFile, "%s\\de_stroyed\\%s.ini", szPath, strName.c_str());
	}

	CreateDirectory(szFolder, NULL);

	static char szValue[16];

	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			switch (pVariable->m_nValue.m_Type)
			{
			case CONFIG_INT:
				GetPrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 16, szFile);
				pVariable->m_nValue.m_iValue = atoi(szValue);
				break;
			case CONFIG_FLOAT:
				GetPrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 16, szFile);
				pVariable->m_nValue.m_flValue = atof(szValue);
				break;
			case CONFIG_BOOL:
				GetPrivateProfileString(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 16, szFile);
				pVariable->m_nValue.m_bValue = !!atoi(szValue);
				break;
			}
		}
	}
}

void CConfig::DumpConfig()
{
	Util::PrintMessage("%s\n", "Config Dump:");
	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			switch (pVariable->m_nValue.m_Type)
			{
			case CONFIG_INT:
				Util::PrintMessage("   %s -> %s -> %d\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_iValue
				);
				break;
			case CONFIG_FLOAT:
				Util::PrintMessage("   %s -> %s -> %g\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_flValue
				);
				break;
			case CONFIG_BOOL:
				Util::PrintMessage("   %s -> %s -> %s\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_bValue ? "true" : "false"
				);
				break;
			}
		}
	}
}

CConfig* Config = new CConfig;