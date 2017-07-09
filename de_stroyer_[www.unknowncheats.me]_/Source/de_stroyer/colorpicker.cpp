#include "sdk.h"

void MenuElements::ColorPicker::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;

	if (value)
		value->m_flValue = Util::Clamp(value->m_flValue, min, max);
}

void MenuElements::ColorPicker::Paint()
{
	if (!visible) return;

	Color color = Color(40, 210, 116);

	Draw::DrawString(Fonts::Gothic16, rx + 2, ry, Color(255, 255, 255), false, title);
	Draw::DrawOutlinedRect(rx, ry + 7, width, height, Color(40, 40, 40));
	Draw::DrawRect(rx + 1, ry + 8, value->m_flValue * ((float(width) - 2.f) / max), height - 2, Color().FromHSB(value->m_flValue, 1.f, 1.f));

	POINT ptMouse = Input->GetMousePos();
	if (Input->Hovering(rx + 1, ry + 8, width - 3, height))
	{
		int start = value->m_flValue * ((float(width) - 2.f) / max);
		float flColor = Util::Clamp((ptMouse.x - rx) * max / ((float)width - 2.f), min, max);
		Color newColor = Color().FromHSB(flColor, 1.f, 1.f);
		if (ptMouse.x > (rx + 1 + start))
		Draw::DrawRect(rx + 1 + start, ry + 8, ptMouse.x - rx - start, height - 2, newColor);
		else Draw::DrawRect(rx + 1, ry + 8, ptMouse.x - rx, height - 2, newColor);
		Draw::DrawString(5, rx + width / 2, ry, Color(255, 255, 255), true, "%.2f", value->m_flValue);
	}

	if (Input->Holding(rx + 1, ry + 8, width - 3, height))
	{
		value->m_flValue = Util::Clamp((ptMouse.x - rx) * max / ((float)width - 2.f), min, max);
	}
}