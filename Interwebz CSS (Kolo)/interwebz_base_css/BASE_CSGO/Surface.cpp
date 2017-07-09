#include "DllMain.h"

CDraw g_Draw;
CESP g_ESP;

bool bMenu = false;

std::vector<ValveSDK::IMaterial*> vecMapMaterials;
std::vector<ValveSDK::IMaterial*> vecOldMapMaterials;
std::vector<ValveSDK::IMaterial*> vecSkyMaterials;
ValveSDK::IMaterial *pHands = NULL;

bool bGetMaterials = true;

void __stdcall hkdPaintTraverse(unsigned vguiPanel, bool forceRepaint, bool allowForce)
{
	static DWORD dwOrgPaintTraverse = g_pPanelVMT.dwGetMethodAddress(41);
	
	__asm
	{
		PUSH allowForce;
		PUSH forceRepaint;
		PUSH vguiPanel;
		CALL dwOrgPaintTraverse;
	}
	
	static bool bDidOnce = false;

	if (bDidOnce == false)
	{
		g_Draw.InitFonts();

#ifdef DEBUGMODE
		Base::Debug::LOG("Font initalized");
#endif
		bDidOnce = true;
	}
	
	const char* pszPanelName = g_Valve.pPanel->GetName(vguiPanel);

	//if (!(pszPanelName[0] == 'M' && pszPanelName[3] == 'S' && pszPanelName[9] == 'T' && pszPanelName[12] == 'P'))
	//	return;

	static std::string sPanel = /*FocusOverlayPanel*/XorStr<0xDB,18,0xB1284984>("\x9D\xB3\xBE\xAB\xAC\xAF\x97\x87\x91\x88\x84\x9F\xB7\x89\x87\x8F\x87"+0xB1284984).s;

	if(!strstr(pszPanelName,sPanel.data()))
		return;

	if(g_Valve.pEngine->IsInGame())
	{
		static DWORD dwRetAddress;

		if(dwRetAddress == NULL)
			dwRetAddress = (DWORD)_ReturnAddress();

		if(dwRetAddress != (DWORD)_ReturnAddress())
			return;
	}
	else
	{
		static DWORD dwRetAddress;

		if(dwRetAddress == NULL)
			dwRetAddress = (DWORD)_ReturnAddress();

		if(dwRetAddress == (DWORD)_ReturnAddress())
			return;		
	}

	//pointer address to convars never change so we only get it once
	static ValveSDK::ConVar *pMouseEnable = g_Valve.pConVar->FindVar(/*cl_mouseenable*/XorStr<0x77,15,0xA851ADE6>("\x14\x14\x26\x17\x14\x09\x0E\x1B\x1A\xEE\xE0\xE0\xEF\xE1"+0xA851ADE6).s);

	static std::string sXor = /*INTERWEBZ CSS*/XorStr<0x8C,14,0x50D39DA3>("\xC5\xC3\xDA\xCA\xC2\xC6\xD7\xD1\xCE\xB5\xD5\xC4\xCB"+0x50D39DA3).s;

	g_Draw.DrawStringA(g_Draw.m_WatermarkFont,true,250,20,255,255,255,255,sXor.data());

	if(GetAsyncKeyState(VK_INSERT) & 1)
		bMenu = !bMenu;

	//make sure you got m_rawinput 1 ingame...
	if(bMenu)
	{
		g_Mouse.Update(pMouseEnable);

		int copy_x, copy_y;
		int copy_w, copy_h;

		g_Menu.GetMenuPos(copy_x,copy_y);
		g_Menu.GetMenuSize(copy_w,copy_h);

		g_Menu.DrawMenu();

		g_Mouse.Drag(g_Mouse.bDragged[0],
			!g_Menu.IsHandlingItem(),
			g_Mouse.LeftClick(copy_x,copy_y,copy_w,copy_h),copy_x,copy_y,g_Mouse.iDiffX[0],g_Mouse.iDiffY[0]);

		g_Menu.SetMenuPos(copy_x,copy_y);

		if(g_Menu.IsHandlingItem())
			g_Menu.RemoveMenuFlag(FL_DISABLEDRAG);//enable menu dragging

		//DRAW MENU HEREEEEEEEEEE 
		//concept window by me (to show dragging for example)
		//---------------------------------------------------------------------------------------
		/*static int winx = 100, winy = 100, winw = 100, winh = winw;

		//position (almost whole box)
		g_Mouse.Drag(g_Mouse.bDragged[0],//0 dragobject
			!(g_Mouse.bDragged[1] || g_Mouse.bDragged[2]),
			g_Mouse.LeftClick(winx,winy,winw-25,winh-25),winx,winy,g_Mouse.iDiffX[0],g_Mouse.iDiffY[0]);

		//resize (bottom and left side)
		g_Mouse.Drag(g_Mouse.bDragged[1],//next dragobject is 1
			!(g_Mouse.bDragged[0] || g_Mouse.bDragged[2]),//check all other drag objects being dragged or smth
			(g_Mouse.LeftClick(winx,winy+winh-25,winw,25) || g_Mouse.LeftClick(winx+winw-25,winy,25,winh)),//bottom and right side are the spots to resize we reserved 25 pixels for that
			winw,winh,g_Mouse.iDiffX[1],g_Mouse.iDiffY[1]);//same number dragobject we used for bDragged

		if(winh < 100)
			winh = 100;

		if(winw < 100)
			winw = 100;

		g_Draw.FillRGBA(winx,winy,winw,winh,200,70,70,255);
		//---------------------------------------------------------------------------------------*/

		//this comes at end so we overdraw menu
		g_Mouse.DrawMouse();
	} 
	else if(g_Mouse.bReturn)//do this once as buymenu enables/disables mouse same way we do
	{
		pMouseEnable->SetValue(1);
		g_Mouse.bReturn = false;
	}
	
	static bool bNosky;
	//static bool bAsus;

	if(!g_Valve.pEngine->IsInGame() || !g_Valve.pEngine->IsConnected())
	{
		//bGetMaterials = true;
		//bNosky = false;
		//bAsus = false;

		return;
	}

	/*if(bGetMaterials) 
	{
		vecSkyMaterials.clear();
		vecMapMaterials.clear();

		for(ValveSDK::MaterialHandle_t h = 0; h < g_Valve.pMaterialSystem->GetNumMaterials(); h++)
		{ 
			ValveSDK::IMaterial *pFirstMaterial = g_Valve.pMaterialSystem->GetMaterial(h);

			if(!pFirstMaterial)
				continue;

			if(strstr(pFirstMaterial->GetTextureGroupName(),XorStr<0xA4,7,0xFAF2E307>("\xF7\xCE\xDF\xE5\xC7\xD1"+0xFAF2E307).s))
				vecSkyMaterials.push_back(pFirstMaterial); 
			else if(strstr(pFirstMaterial->GetTextureGroupName(),XorStr<0x8A,6,0x3FB546ED>("\xDD\xE4\xFE\xE1\xEA"+0x3FB546ED).s))
				vecMapMaterials.push_back(pFirstMaterial);
			else if(strstr(pFirstMaterial->GetName(),XorStr<0x8D,8,0xFE3E79EF>("\xFB\xD1\xE7\xF1\xFF\xF6\xE0"+0xFE3E79EF).s))
				pHands = pFirstMaterial;
		}

		bGetMaterials = false;
	}

	static float fOldStrength;

	if(g_CVARS.CvarList[Asus])
	{
		if((!bAsus || g_CVARS.CvarList[AsusAmount] != fOldStrength) && !vecMapMaterials.empty())
		{
			for(int iTex = 0; iTex < vecMapMaterials.size(); iTex++)
			{
				if(!vecMapMaterials[iTex])
					continue;

				vecMapMaterials[iTex]->AlphaModulate((g_CVARS.CvarList[AsusAmount] * 0.1f));

				if(iTex == (vecMapMaterials.size()-1))
				{
					bAsus = true;
					fOldStrength = g_CVARS.CvarList[AsusAmount];
				}
			}
		}
	}
	else
	{
		if(bAsus && !vecMapMaterials.empty())
		{
			for(int iTex = 0; iTex < vecMapMaterials.size(); iTex++)
			{
				if(!vecMapMaterials[iTex])
					continue;

				vecMapMaterials[iTex]->AlphaModulate(1.0f);

				if(iTex == (vecMapMaterials.size()-1))
					bAsus = false;
			}
		}
	}

	if(g_CVARS.CvarList[NoSky])
	{
		if(!bNosky && !vecSkyMaterials.empty())
		{
			for(int iTex = 0; iTex < vecSkyMaterials.size(); iTex++)
			{
				if(!vecSkyMaterials[iTex])
					continue;

				vecSkyMaterials[iTex]->ColorModulate(0.0f,0.0f,0.0f);

				if(iTex == (vecSkyMaterials.size()-1))
					bNosky = true;
			}
		}
	}
	else
	{
		if(bNosky && !vecSkyMaterials.empty())
		{
			for(int iTex = 0; iTex < vecSkyMaterials.size(); iTex++)
			{
				if(!vecSkyMaterials[iTex])
					continue;

				vecSkyMaterials[iTex]->ColorModulate(1.0f,1.0f,1.0f);

				if(iTex == (vecSkyMaterials.size()-1))
					bNosky = false;
			}
		}
	}

	//CSS only - we just use findmaterial
	static bool bHands;

	if(g_CVARS.CvarList[NoHands] && !bHands)
	{
		if(!pHands->GetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW))
			pHands->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW,true);

		bHands = true;
	}
	else if(!g_CVARS.CvarList[NoHands] && bHands)
	{
		if(pHands->GetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW))
			pHands->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW,false);

		bHands = false;
	}*/
	
	if(g_CVARS.CvarList[EnableESP])
		g_ESP.draw();
}

bool LoadFromBuff(KeyValues *pThis, const char *pszFirst, const char *pszSecond, PVOID pSomething = 0, PVOID pAnother = 0)
{
	typedef bool(__thiscall *LoadFromBuffer_t)(KeyValues*,const char*,const char*,PVOID,PVOID);	

	static DWORD dwAddress = NULL;

	if(dwAddress == NULL)
	{
		dwAddress = Base::Utils::PatternSearch(/*client.dll*/XorStr<0xA1,11,0x8F27B968>("\xC2\xCE\xCA\xC1\xCB\xD2\x89\xCC\xC5\xC6"+0x8F27B968).s,(PBYTE)"\x55\x8B\xEC\x83\xEC\x34\x53\x8B\x5D\x0C",/*xxxxxxxxxx*/XorStr<0x4A,11,0x4007B1F8>("\x32\x33\x34\x35\x36\x37\x28\x29\x2A\x2B"+0x4007B1F8).s,NULL,NULL);
#ifdef DEBUGMODE
		char szLog[256];
		sprintf(szLog,"LoadFromBuffer: 0x%x",dwAddress);
		Base::Debug::LOG(szLog);
#endif
	}

	static LoadFromBuffer_t callLoadBuff = (LoadFromBuffer_t)(dwAddress);

	return callLoadBuff(pThis,pszFirst,pszSecond,pSomething,pAnother);
}

void ChamModel( float r, float g, float b, ValveSDK::IMaterial *pMat)
{
	float fColor[4] = { r/255, g/255, b/255, 1.0f };

	static float fGetColor[4];

	g_Valve.pRenderView->GetColorModulation(fGetColor);

	if(fGetColor[0] != fColor[0] || fGetColor[1] != fColor[1] || fGetColor[2] != fColor[2])
		g_Valve.pRenderView->SetColorModulation( fColor );

	if(pMat)
		g_Valve.pModelRender->ForcedMaterialOverride(pMat);
}

void FullCham( ValveSDK::IMaterial *pMat, const ValveSDK::ModelRenderInfo_t &pInfo, int r, int g, int b, int r2, int g2, int b2, bool bDeadIgnorez)
{
	CBaseEntity *pBaseEnt = g_Valve.pEntList->GetClientEntity(pInfo.entity_index);
	
	if(!pBaseEnt)
	{
		g_Valve.pModelRender->ForcedMaterialOverride(NULL);
		return;
	}

	if(pBaseEnt->GetHealth() >= 1)//regular lifestate checking didnt work -.-''''
	{
		int iTeamNum = pBaseEnt->GetTeamNum();

		CBaseEntity *pLocal = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());

		int iMyTeamNum = pLocal->GetTeamNum();

		if(g_CVARS.CvarList[ChamsEnemyOnly] && iTeamNum == iMyTeamNum)
			return;

		if( iTeamNum == 3 )
			ChamModel(r, g, b, pMat);
		else if( iTeamNum == 2 )
			ChamModel(r2, g2, b2, pMat);
		else
			g_Valve.pModelRender->ForcedMaterialOverride(NULL);
	}
	else if(!bDeadIgnorez)
		ChamModel(255, 255, 255, pMat);//if deed then they white hehe
}

//kolonote:
//macro c&p from my REAAAAALY old source (tbh kiro didnt make this first as i made it already in 2010 hah ownd - yes i did give him my code why ? hehe)
#define MAT( _TYPE_ ) "\"" + _TYPE_ + "\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$ignorez\" \"0\"\n\t\"$nofog\" \"1\"\n\t\"$halflambert\" \"1\"\n}"
#define MAT_IGNOREZ( _TYPE_ ) "\"" + _TYPE_ + "\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$ignorez\" \"1\"\n\t\"$nofog\" \"1\"\n\t\"$halflambert\" \"1\"\n}"

void GenerateRandomString(char *s, const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; i++)
	{
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	s[len] = 0;
}

ValveSDK::IMaterial *CreateMaterial(BOOL bFullbright, BOOL bIgnorez)
{
	char szBuff[2048];

	std::string type = (bFullbright ? /*UnlitGeneric*/XorStr<0xAF,13,0x932BA6F1>("\xFA\xDE\xDD\xDB\xC7\xF3\xD0\xD8\xD2\xCA\xD0\xD9"+0x932BA6F1).s : /*VertexLitGeneric*/XorStr<0xFD,17,0x1247276C>("\xAB\x9B\x8D\x74\x64\x7A\x4F\x6D\x71\x41\x62\x66\x6C\x78\x62\x6F"+0x1247276C).s);
	std::string tmp( (bIgnorez ? MAT_IGNOREZ(type) : MAT(type)) );
	sprintf( szBuff, tmp.c_str() );

	KeyValues *pKeyValues = new KeyValues(type.c_str());

	//generate some random file name for the .vmt (residing only in memory)
	char szMaterialName[128];
	GenerateRandomString(szMaterialName,17);
	strcat(szMaterialName,/*.vmt*/XorStr<0x26,5,0x8CB78199>("\x08\x51\x45\x5D"+0x8CB78199).s);

	LoadFromBuff(pKeyValues,szMaterialName,szBuff);

	ValveSDK::IMaterial *pNew = g_Valve.pMaterialSystem->CreateMaterial(szMaterialName,(PVOID)pKeyValues);

	if(pNew)
		pNew->IncrementReferenceCount();

#ifdef DEBUGMODE
	char szNewMaterial[1024];
	sprintf(szNewMaterial,"Created new Valve Material Type[%s] Fullbright[%i] Ignorez[%i]",szMaterialName,bFullbright,bIgnorez);
	Base::Debug::LOG(szNewMaterial);
#endif

	return pNew;
}