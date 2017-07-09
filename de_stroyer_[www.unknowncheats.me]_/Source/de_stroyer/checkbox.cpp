#include "sdk.h"

void MenuElements::Checkbox::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;

	if (Input->Clicked(rx, ry, size, size))
	{
		value->m_bValue = !value->m_bValue;
	}
}

void MenuElements::Checkbox::Paint()
{
	if (!visible) return;

	Color color = Color(27, 195, 106);
	Color outlineColor = Color(color.r(), color.g(), color.b(), 100);

	Draw::DrawOutlinedRect(rx, ry, size, size, Color(140, 140, 140));
	
	if (value->m_bValue)
	{
		Draw::DrawOutlinedRect(rx + 2, ry + 2, size - 4, size - 4, outlineColor);
		Draw::DrawRect(rx + 3, ry + 3, size - 6, size - 6, color);
	}
	
	Draw::DrawString(Fonts::Gothic16, rx + size + 4, ry + 8, Color(255, 255, 255), false, title);
}