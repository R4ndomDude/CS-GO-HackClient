#pragma once

//#include "sdk.h"

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

typedef unsigned long HFont;

struct Vector2D
{
public:
	float x, y;

	Vector2D() {}
	Vector2D(float x_, float y_) { x = x_; y = y_; }
};

struct FontVertex_t
{
	Vector2D m_Position;
	Vector2D m_TexCoord;

	FontVertex_t() {}
	FontVertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
};

typedef FontVertex_t Vertex_t;

namespace Fonts
{
	extern HFont Courier14;
	extern HFont Gothic12;
	extern HFont Gothic16;
	extern HFont Morningtype24;
	extern HFont Gothic30;
}

namespace Draw
{
	extern void GrabFonts();
	extern void DrawString(HFont font, int x, int y, Color color, bool bCenter, const char* msg, ...);
	extern void DrawStringUnicode(HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ...);
	extern void DrawRect(int x, int y, int w, int h, Color col);
	extern void DrawRectRainbow(int x, int y, int w, int h, float flSpeed, float &flRainbow);
	extern void DrawRectGradientVertical(int x, int y, int w, int h, Color color1, Color color2);
	extern void DrawRectGradientHorizontal(int x, int y, int w, int h, Color color1, Color color2);
	extern void DrawPixel(int x, int y, Color col);
	extern void DrawOutlinedRect(int x, int y, int w, int h, Color col);
	extern void DrawOutlinedCircle(int x, int y, int r, Color col);
	extern void DrawLine(int x0, int y0, int x1, int y1, Color col);
	extern void DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, Color colDraw);
	extern void DrawRoundedBox(int x, int y, int w, int h, int r, int v, Color col);
	extern void DrawPolygon(int count, Vertex_t* Vertexs, Color color);
	extern bool ScreenTransform(const Vector &point, Vector &screen);
	extern bool WorldToScreen(const Vector &origin, Vector &screen);
	extern int GetStringWidth(HFont font, const char* msg, ...);
}