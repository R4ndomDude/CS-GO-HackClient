#include "sdk.h"

HFont Fonts::Courier14;
HFont Fonts::Gothic12;
HFont Fonts::Gothic16;
HFont Fonts::Morningtype24;
HFont Fonts::Gothic30;

void Draw::GrabFonts()
{
	I::Surface->SetFontGlyphSet(Fonts::Courier14 = I::Surface->Create_Font(), "Courier New", 14, FW_LIGHT, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(Fonts::Gothic12 = I::Surface->Create_Font(), "Franklin Gothic Medium", 12, FW_DONTCARE, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(Fonts::Gothic16 = I::Surface->Create_Font(), "Franklin Gothic Medium", 16, FW_DONTCARE, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(Fonts::Morningtype24 = I::Surface->Create_Font(), "Morningtype", 24, FW_LIGHT, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(Fonts::Gothic30 = I::Surface->Create_Font(), "Franklin Gothic Medium", 30, FW_DONTCARE, NULL, NULL, FONTFLAG_ANTIALIAS);
}

void Draw::DrawString(HFont font, int x, int y, Color color, bool bCenter, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int iWidth, iHeight;

	I::Surface->GetTextSize(font, wbuf, iWidth, iHeight);
	I::Surface->DrawSetTextFont(font);
	I::Surface->DrawSetTextColor(r, g, b, a);
	I::Surface->DrawSetTextPos(!bCenter ? x : x - iWidth / 2, y - iHeight / 2);
	I::Surface->DrawPrintText(wbuf, wcslen(wbuf));
}

void Draw::DrawStringUnicode(HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ...)
{
	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int iWidth, iHeight;

	I::Surface->GetTextSize(font, msg, iWidth, iHeight);
	I::Surface->DrawSetTextFont(font);
	I::Surface->DrawSetTextColor(r, g, b, a);
	I::Surface->DrawSetTextPos(!bCenter ? x : x - iWidth / 2, y - iHeight / 2);
	I::Surface->DrawPrintText(msg, wcslen(msg));
}

void Draw::DrawRect(int x, int y, int w, int h, Color col)
{
	I::Surface->DrawSetColor(col);
	I::Surface->DrawFilledRect(x, y, x + w, y + h);
}

void Draw::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	Color colColor(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		Draw::DrawRect(x + i, y, 1, height, colRainbow);
	}
}

void Draw::DrawRectGradientHorizontal(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)width;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)width;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)width;

	Util::PrintMessage("%f : %f : %f\n", flDifferenceR, flDifferenceG, flDifferenceB);

	for (float i = 0.f; i < width; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a());
		Draw::DrawRect(x + i, y, 1, height, colGradient);
	}
}

void Draw::DrawPixel(int x, int y, Color col)
{
	I::Surface->DrawSetColor(col);
	I::Surface->DrawFilledRect(x, y, x + 1, y + 1);
}

void Draw::DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	I::Surface->DrawSetColor(col);
	I::Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Draw::DrawOutlinedCircle(int x, int y, int r, Color col)
{
	I::Surface->DrawSetColor(col);
	I::Surface->DrawOutlinedCircle(x, y, r, 1);
}

void Draw::DrawLine(int x0, int y0, int x1, int y1, Color col)
{
	I::Surface->DrawSetColor(col);
	I::Surface->DrawLine(x0, y0, x1, y1);
}

void Draw::DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, Color colDraw)
{
	int iRealX = bRight ? iX - iWidth : iX;
	int iRealY = bDown ? iY - iHeight : iY;

	if (bDown && bRight)
		iWidth = iWidth + 1;

	Draw::DrawRect(iRealX, iY, iWidth, 1, colDraw);
	Draw::DrawRect(iX, iRealY, 1, iHeight, colDraw);

	Draw::DrawRect(iRealX, bDown ? iY + 1 : iY - 1, !bDown && bRight ? iWidth + 1 : iWidth, 1, Color(0, 0, 0, 255));
	Draw::DrawRect(bRight ? iX + 1 : iX - 1, bDown ? iRealY : iRealY - 1, 1, bDown ? iHeight + 2 : iHeight + 1, Color(0, 0, 0, 255));
}

void Draw::DrawPolygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = I::Surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };
	
	I::Surface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	I::Surface->DrawSetColor(color);
	I::Surface->DrawSetTexture(Texture);

	I::Surface->DrawTexturedPolygon(count, Vertexs);
}

void Draw::DrawRoundedBox(int x, int y, int w, int h, int r, int v, Color col)
{
	std::vector<Vertex_t> p;
	for (int _i = 0; _i < 3; _i++)
	{
		int _x = x + (_i < 2 && r || w - r);
		int _y = y + (_i % 3 > 0 && r || h - r);
		for (int i = 0; i < v; i++)
		{
			int a = RAD2DEG((i / v) * -90 - _i * 90);
			p.push_back(Vertex_t(Vector2D(_x + sin(a) * r, _y + cos(a) * r)));
		}
	}

	Draw::DrawPolygon(4 * (v + 1), &p[0], col);
	/*
	function DrawRoundedBox(x, y, w, h, r, v, col)
		local p = {};
		for _i = 0, 3 do
			local _x = x + (_i < 2 && r || w - r)
			local _y = y + (_i%3 > 0 && r || h - r)
			for i = 0, v do
				local a = math.rad((i / v) * - 90 - _i * 90)
				table.insert(p, {x = _x + math.sin(a) * r, y = _y + math.cos(a) * r})
			end
		end

		surface.SetDrawColor(col.r, col.g, col.b, 255)
		draw.NoTexture()
		surface.DrawPoly(p)
	end
	*/

// Notes: amount of vertexes is 4(v + 1) where v is the number of vertices on each corner bit.
// I did it in lua cause I have no idea how the vertex_t struct works and i'm still aids at C++
}

bool Draw::ScreenTransform(const Vector &point, Vector &screen) // tots not pasted
{
	float w;
	const VMatrix &worldToScreen = I::Engine->WorldToScreenMatrix();
	
	screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	screen.z = 0.0f;

	bool behind = false;

	if (w < 0.001f)
	{
		behind = true;
		screen.x *= 100000;
		screen.y *= 100000;
	}
	else
	{
		behind = false;
		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
	}

	return behind;
}

bool Draw::WorldToScreen(const Vector &origin, Vector &screen)
{
	if (!ScreenTransform(origin, screen))
	{
		int ScreenWidth, ScreenHeight;
		I::Engine->GetScreenSize(ScreenWidth, ScreenHeight);
		float x = ScreenWidth / 2;
		float y = ScreenHeight / 2;
		x += 0.5 * screen.x * ScreenWidth + 0.5;
		y -= 0.5 * screen.y * ScreenHeight + 0.5;
		screen.x = x;
		screen.y = y;
		return true;
	}

	return false;
}

int Draw::GetStringWidth(HFont font, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int iWidth, iHeight;

	I::Surface->GetTextSize(font, wbuf, iWidth, iHeight);

	return iWidth;
}