#pragma once

enum ConfigValueType
{
	CONFIG_INT,
	CONFIG_FLOAT,
	CONFIG_BOOL
};

struct ConfigValue
{
	ConfigValue() {}

	ConfigValue(int iValue, ConfigValueType type)
	{
		m_iValue = iValue;
		m_bValue = !!iValue;
		m_Type = type;
	}

	ConfigValue(float flValue, ConfigValueType type)
	{
		m_flValue = flValue;
		m_Type = type;
	}

	ConfigValue(bool bValue, ConfigValueType type)
	{
		m_bValue = bValue;
		m_iValue = (int)bValue;
		m_Type = type;
	}

	int m_iValue;
	float m_flValue;
	bool m_bValue;

	ConfigValueType m_Type;
};

struct ConfigVariable
{
	ConfigVariable() {}

	ConfigVariable(std::string strName, ConfigValue nValue)
	{
		m_strName = strName;
		m_nValue = nValue;
	}

	std::string m_strName;
	ConfigValue m_nValue;
};

struct ConfigGroup
{
	ConfigGroup() {}

	ConfigGroup(std::string strName)
	{
		m_strName = strName;
	}

	std::string m_strName;
	std::vector<ConfigVariable*> m_Variables;
};

class CConfig
{
public:
	void Setup();
	void SetValue(std::string strGroup, std::string strValue, ConfigValue nValue);
	ConfigValue GetValue(std::string strGroup, std::string strValue);
	ConfigValue* GetValuePtr(std::string strGroup, std::string strValue);
	void SaveConfig(std::string strName);
	void LoadConfig(std::string strName);
	void DumpConfig();
private:
	std::vector<ConfigGroup*> groups;
};

extern CConfig* Config;