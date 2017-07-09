#include "sdk.h"

void MenuElements::Button::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;

	if (Input->Clicked(rx, ry, width, height))
		callback();
}

void MenuElements::Button::Paint()
{
	if (!visible) return;

	Color color = Color(27, 195, 106);
	Color outlineColor = Color(color.r(), color.g(), color.b(), 100);

	Draw::DrawOutlinedRect(rx, ry, width, height, Color(140, 140, 140));
	Draw::DrawRect(rx + 1, ry + 1, width - 2, height - 2, Color(27, 27, 27));

	Draw::DrawString(Fonts::Gothic16, rx + width / 2, ry + height / 2, Color(255, 255, 255), true, "%s", title);
}