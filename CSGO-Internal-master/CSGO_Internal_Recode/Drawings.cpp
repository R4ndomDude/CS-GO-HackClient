#include "Drawings.h"
#include "NetVars.h"
#include "Misc.h"
#include "Player.h"
#include "CVars.h"

CDrawings::CDrawings()
{
	hFont = Interfaces.Surface->CreateFnt();
	if(!hFont)
		return;

	Interfaces.Surface->SetFontGlyphSet(hFont, "Tahoma", 12, 400, 0, 0, FONTFLAG_OUTLINE);

	Interfaces.Engine->GetScreenSize(ScreenSize.Width, ScreenSize.Height);
}

CDrawings::~CDrawings()
{
	hFont = 0;
}

void CDrawings::DrawString(int x, int y, Color clrColor, /*bool bCenter,*/ wchar_t* szText)
{
	if(!szText)
		return;

	Interfaces.Surface->DrawSetTextPos(x, y);
	Interfaces.Surface->DrawSetTextFont(hFont);
	Interfaces.Surface->DrawSetTextColor(clrColor);
	Interfaces.Surface->DrawPrintText(szText, wcslen(szText));
}

void CDrawings::DrawString(int x, int y, Color clrColor, bool bCenter, const char* szText, ...)
{
	if(!szText)
		return;

	va_list va_alist = nullptr;
	char szBuffer[256] = { '\0' };
	wchar_t szString[128] = { '\0' };

	va_start(va_alist, szText);
	vsprintf_s(szBuffer, szText, va_alist);
	va_end(va_alist);

	MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szString, 128);

	if(bCenter)
	{
		int Wide = 0, Tall = 0;

		Interfaces.Surface->GetTextSize(hFont, szString, Wide, Tall);

		x -= Wide / 2;
	}

	Interfaces.Surface->DrawSetTextPos(x, y);
	Interfaces.Surface->DrawSetTextFont(hFont);
	Interfaces.Surface->DrawSetTextColor(clrColor);
	Interfaces.Surface->DrawPrintText(szString, strlen(szBuffer));
}

void CDrawings::DrawRect(int x, int y, int w, int h, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawFilledRect(x, y, x + w, y + h);
}

void CDrawings::DrawOutlineRect(int x, int y, int w, int h, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void CDrawings::DrawLine(int x, int y, int x1, int y1, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawLine(x, y, x1, y1);
}

void CDrawings::DrawCornerBox(int x, int y, int w, int h, int cx, int cy, Color clrColor)
{
	DrawLine(x, y, x + (w / cx), y, clrColor);
	DrawLine(x, y, x, y + (h / cy), clrColor);

	DrawLine(x + w, y, x + w - (w / cx), y, clrColor);
	DrawLine(x + w, y, x + w, y + (h / cy), clrColor);

	DrawLine(x, y + h, x + (w / cx), y + h, clrColor);
	DrawLine(x, y + h, x, y + h - (h / cy), clrColor);

	DrawLine(x + w, y + h, x + w - (w / cx), y + h, clrColor);
	DrawLine(x + w, y + h, x + w, y + h - (h / cy), clrColor);
}

void CDrawings::DynamicBox(CPlayer* pPlayer, const char* szName, Color clrColor)
{
	const matrix3x4& trans = *reinterpret_cast<matrix3x4*>(pPlayer + NetVars.m_rgflCoordinateFrame);

	Vector3 min = *reinterpret_cast<Vector3*>(pPlayer + NetVars.m_Collision + 0x8);
	Vector3 max = *reinterpret_cast<Vector3*>(pPlayer + NetVars.m_Collision + 0x14);

	Vector3 pointList[] = {
		Vector3(min.x, min.y, min.z),
		Vector3(min.x, max.y, min.z),
		Vector3(max.x, max.y, min.z),
		Vector3(max.x, min.y, min.z),
		Vector3(max.x, max.y, max.z),
		Vector3(min.x, max.y, max.z),
		Vector3(min.x, min.y, max.z),
		Vector3(max.x, min.y, max.z)
	};

	Vector3 transformed[8];

	for(int i = 0; i < 8; i++)
		Hack.Misc->VectorTransform(pointList[i], trans, transformed[i]);

	Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

	if(!Hack.Misc->WorldToScreen(transformed[3], flb) ||
		!Hack.Misc->WorldToScreen(transformed[0], blb) ||
		!Hack.Misc->WorldToScreen(transformed[2], frb) ||
		!Hack.Misc->WorldToScreen(transformed[6], blt) ||
		!Hack.Misc->WorldToScreen(transformed[5], brt) ||
		!Hack.Misc->WorldToScreen(transformed[4], frt) ||
		!Hack.Misc->WorldToScreen(transformed[1], brb) ||
		!Hack.Misc->WorldToScreen(transformed[7], flt))
		return;

	Vector3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for(int i = 0; i < 8; i++) {
		if(left > arr[i].x)
			left = arr[i].x;
		if(top < arr[i].y)
			top = arr[i].y;
		if(right < arr[i].x)
			right = arr[i].x;
		if(bottom > arr[i].y)
			bottom = arr[i].y;
	}

	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;

	if(Hack.CVars->esp_box)
	{
		DrawCornerBox(x - 1, y - 1, w + 2, h + 1, 3, 5, Color::Black());
		DrawCornerBox(x, y, w, h - 1, 3, 5, clrColor);
	}

	if(Hack.CVars->esp_name)
	{
		DrawString(x + (w / 2), y - 14, Color::White(), true, "%s", szName);
	}

	if(Hack.CVars->esp_health)
	{
		int health = pPlayer->GetHealth();

		if(health > 100)
			health = 100;

		int r = 255 - health * 2.55;
		int g = health * 2.55;

		int healthBar = h / 100 * health;
		int healthBarDelta = h - healthBar;

		DrawOutlineRect(x - 5, y - 1, 3, h + 2, Color::Black());
		DrawOutlineRect(x - 4, y + healthBarDelta, 1, healthBar, Color(r, g, 0, 255));
	}
}