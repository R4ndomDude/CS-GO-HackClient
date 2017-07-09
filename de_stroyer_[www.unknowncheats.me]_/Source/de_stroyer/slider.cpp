#include "sdk.h"

void MenuElements::Slider::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;

	if (value)
		value->m_flValue = Util::Clamp(value->m_flValue, min, max);
}

void MenuElements::Slider::Paint()
{
	if (!visible) return;

	Color color = Color(40, 210, 116);

	Draw::DrawString(Fonts::Gothic16, rx + 2, ry, Color(255, 255, 255), false, title);
	Draw::DrawOutlinedRect(rx, ry + 7, width, height, Color(40, 40, 40));
	Draw::DrawRect(rx + 1, ry + 8, value->m_flValue * ((float(width) - 2.f) / max), height - 2, color);

	POINT ptMouse = Input->GetMousePos();
	if (Input->Hovering(rx + 1, ry + 8, width - 3, height))
	{
		Color newColor = Color(color.r(), color.g(), color.b(), 100);
		Draw::DrawRect(rx + 1, ry + 8, ptMouse.x - rx, height - 2, newColor);
		Draw::DrawString(4, rx + width - (Draw::GetStringWidth(4, "%.2f", value->m_flValue)) - 2, ry, Color(255, 255, 255), false, "%.2f", value->m_flValue);
	}

	if (Input->Holding(rx + 1, ry + 8, width - 3, height))
	{
		value->m_flValue = Util::Clamp((ptMouse.x - rx) * max / ((float)width - 2.f), min, max);
	}
}