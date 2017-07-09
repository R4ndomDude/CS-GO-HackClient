#include "DllMain.h"

cCVARS g_CVARS;

std::string CvarNames[MAX_CVARS];

void cCVARS::Init(HMODULE hinstDLL)
{
	GetModuleFileName(hinstDLL,g_CVARS.szIniFilePath,2048);

	for(int i = strlen(g_CVARS.szIniFilePath); i > 0; i--) 
	{ 
		if(g_CVARS.szIniFilePath[i] == '\\') 
		{ 
			g_CVARS.szIniFilePath[i+1] = 0;
			break;//break loop so we dont delete everything
		} 
	}

	for(int i = 0; i < MAX_CVARS; i++)
	{
		char szName[10];
		sprintf(szName,/*[var%i]*/XorStr<0xC0,8,0x685B0EEB>("\x9B\xB7\xA3\xB1\xE1\xAC\x9B"+0x685B0EEB).s,i);
		CvarNames[i] = szName;
	}
}

//example:
//szPublicHtmlURL - should be for example "yoursite.com" (without http:// and www.)
//szURLPath - should be for example "kolo.php" (so this would mean yoursite.com/kolo.php)
//szURLPath - you can also assign it with something like "hehe/kolo.php" (this would result in a url of yoursite.com/hehe/kolo.php)
//return value - a byte array of any kind of data received from the url (binary data, text,...)
__forceinline void Winsock_StreamOnlineDataToMemory(char *szPublicHtmlURL, char *szURLPath, std::string &out)
{
	g_Winsock.InitWinsock();
	g_Winsock.Connect(szPublicHtmlURL);
	g_Winsock.SendHttpGet(szURLPath,out);
	g_Winsock.Disconnect();
	g_Winsock.CleanUp();
}

//kolonote:
//ok time to use my skillz ehi ehi
//note that this is pretty slow soooo
float FindVarValue(const char *szTextArray, const char *pszVarName)
{
	int iLen = strlen(pszVarName) - 1;

	for(int i = 0; i < strlen(szTextArray); i++)
	{
		//first character matches but we gotta check if whole word matches
		if(szTextArray[i] == pszVarName[0])
		{
			bool bSkipThis = false;

			for(int i2 = 0; i2 <= iLen; i2++)
			{
				//we already checked for first character in the array
				if(i2 == 0)
					continue;

				if(szTextArray[i+i2] != pszVarName[i2])
				{
					bSkipThis = true;
					break;//ok not our string lets skip this one
				}
			}

			//something didnt match so lets skip to this one
			if(bSkipThis)
				continue;

			//everything matches meaning we can retrieve the value now
			std::string sOwn = &szTextArray[i+iLen+1];//move 1 more spot as thats where the space is

			return atof(sOwn.data());
		}
	}

	return 0.0f;
}

void cCVARS::HandleConfig(const char *pszConfigName, int iType)
{
	//kolonote:
	//to save shit were gonna generate a config file
	std::string sSavedConfig;
	//our file loaded from server
	std::string sLoadResponse;

	HW_PROFILE_INFO hwProfileInfo;

	//REMEMBER: TEMPORARY
	//this is utter shit but i didnt find a quicker way for unique identification and i need it to store configs for different users on server
	GetCurrentHwProfile(&hwProfileInfo);

	for(int varindex = 0;varindex < MAX_CVARS; varindex++)
	{
		bool bLastInLoop = (varindex == (MAX_CVARS-1));
		bool bFirstInLoop = (varindex == 0);

		if(iType == LoadConfig && bFirstInLoop)
		{
			std::string sPath = /*confighandle.php*/XorStr<0x5B,17,0x34079F60>("\x38\x33\x33\x38\x36\x07\x09\x03\x0D\x00\x09\x03\x49\x18\x01\x1A"+0x34079F60).s;
			char *pszFullPath = new char[sPath.size() + strlen(pszConfigName) + strlen(hwProfileInfo.szHwProfileGuid) + 20];
			sprintf(pszFullPath,/*%s?n=%s&t=1&d=%s&g=2*/XorStr<0x54,21,0x02EED730>("\x71\x26\x69\x39\x65\x7C\x29\x7D\x28\x60\x6F\x79\x04\x5C\x47\x10\x42\x02\x5B\x55"+0x02EED730).s,sPath.data(),pszConfigName,hwProfileInfo.szHwProfileGuid);

			//Winsock_StreamOnlineDataToMemory(/*37.48.84.23*/XorStr<0x72,12,0xFE4D2C44>("\x41\x44\x5A\x41\x4E\x59\x40\x4D\x54\x49\x4F"+0xFE4D2C44).s,pszFullPath,sLoadResponse);

			Winsock_StreamOnlineDataToMemory(/*interwebz.cc*/XorStr<0xD0,13,0xAECE5734>("\xB9\xBF\xA6\xB6\xA6\xA2\xB3\xB5\xA2\xF7\xB9\xB8"+0xAECE5734).s,pszFullPath,sLoadResponse);

			delete [] pszFullPath;
		}

		if(iType == SaveConfig)
		{
#ifdef DEBUGMODE
			if(bFirstInLoop)
			{
				Base::Debug::LOG("");
				Base::Debug::LOG("Settings Saved!");
				Base::Debug::LOG("");
			}
			
			char szLog[256];
			sprintf(szLog,"%s =  %f",CvarNames[varindex].data(),g_CVARS.CvarList[varindex]);
			Base::Debug::LOG(szLog);
#endif
			char szBitcho[256];
			sprintf(szBitcho,/*%s%f*/XorStr<0x46,5,0x504F1CD1>("\x63\x34\x6D\x2F"+0x504F1CD1).s,CvarNames[varindex].data(),g_CVARS.CvarList[varindex]);
			sSavedConfig += szBitcho;

			if(bLastInLoop)
			{
				std::string sPath = /*confighandle.php*/XorStr<0x3A,17,0xE30B48FD>("\x59\x54\x52\x5B\x57\x58\x28\x20\x2C\x27\x28\x20\x68\x37\x20\x39"+0xE30B48FD).s;
				char *pszFullPath = new char[sPath.size() + sSavedConfig.size() + strlen(pszConfigName) + strlen(hwProfileInfo.szHwProfileGuid) + 20];
				sprintf(pszFullPath,/*%s?t=0&n=%s&f=%s&d=%s&g=2*/XorStr<0xDB,26,0xCAACD7BA>("\xFE\xAF\xE2\xAA\xE2\xD0\xC7\x8C\xDE\xC1\x96\xC0\x81\xD5\xCC\x99\xCD\x88\xD0\xCB\x9C\xD6\x96\xCF\xC1"+0xCAACD7BA).s,sPath.data(),pszConfigName,sSavedConfig.data(),hwProfileInfo.szHwProfileGuid);

				std::string sOut;
				Winsock_StreamOnlineDataToMemory(/*interwebz.cc*/XorStr<0xD0,13,0xAECE5734>("\xB9\xBF\xA6\xB6\xA6\xA2\xB3\xB5\xA2\xF7\xB9\xB8"+0xAECE5734).s,pszFullPath,sOut);

				delete [] pszFullPath;
			}

#ifdef DEBUGMODE
			if(bLastInLoop)
				Base::Debug::LOG("");
#endif
		}
		else if(iType == LoadConfig)
		{
#ifdef DEBUGMODE
			if(bFirstInLoop)
			{
				Base::Debug::LOG("");
				Base::Debug::LOG("Settings Loaded!");
				Base::Debug::LOG("");
			}
#endif

			g_CVARS.CvarList[varindex] = FindVarValue(sLoadResponse.data(),CvarNames[varindex].data());

#ifdef DEBUGMODE
			char szLog[256];
			sprintf(szLog,"%s =  %f",CvarNames[varindex].data(),g_CVARS.CvarList[varindex]);
			Base::Debug::LOG(szLog);

			if(bLastInLoop)
				Base::Debug::LOG("");
#endif
		}
	}
}

/*char *CvarNames[] = 
{
	"esp_name","esp_health","removals_norecoil","removals_nospread","aim_enable","esp_enable",
	"misc_bunnyhop","misc_autopistol","aim_onfire","aim_autoshoot","aim_silent","aim_team",
	"aim_fov","misc_psilent","misc_fakelagamount","trigger_enable","trigger_perfect","trigger_burst",
	"trigger_headonly","trigger_all","removals_novisrecoil","rcs_enable","rcs_fov","rcs_smooth",
	"rcs_spot","rcs_scs","aim_spot","rcs_aimteam","trigger_seed","aim_height","aim_hitscan",
	"aim_autowall","aim_hitscan_autowall"
};*/