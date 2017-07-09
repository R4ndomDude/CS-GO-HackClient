#include "SDK.h"

class CPlayer;
class CDrawings
{
public:
	CDrawings();
	~CDrawings();

	void	DrawString(int x, int y, Color clrColor, /*bool bCenter,*/ wchar_t* szText);
	void	DrawString(int x, int y, Color clrColor, bool bCenter, const char* szText, ...);
	void	DrawRect(int x, int y, int w, int h, Color clrColor);
	void	DrawOutlineRect(int x, int y, int w, int h, Color clrColor);
	void	DrawLine(int x0, int y0, int x1, int y1, Color clrColor);
	void	DrawCornerBox(int x, int y, int w, int h, int cx, int cy, Color clrColor);

	void	DynamicBox(CPlayer* pPlayer, const char* szName, Color clrColor);

private:
	HFont	hFont;
};