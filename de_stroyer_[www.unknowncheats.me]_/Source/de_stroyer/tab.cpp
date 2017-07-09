#include "sdk.h"

void MenuElements::Tab::Think()
{
	visible = parent->GetVisible() && (parent->GetTab() == GetTab());

	if (!parent->GetVisible()) return;

	if (Input->Clicked(rx, ry + (GetTab() * 22), 78, 20))
	{
		parent->SetTab(GetTab());
	}
}

void MenuElements::Tab::Paint()
{
	if (!parent->GetVisible()) return;

	bool bHovering = Input->Hovering(rx, ry + (GetTab() * 22), 78, 20);

	Draw::DrawRect(rx, ry + (GetTab() * 22), bHovering ? 78 : 74, 20, bHovering ? Color(40, 40, 40) : Color(33, 33, 33));
	Draw::DrawRect(rx, ry + (GetTab() * 22), 4, 20, visible ? Color(29, 255, 56) : Color(40, 40, 40));
	Draw::DrawString(Fonts::Gothic16, rx + (bHovering ? 39 : 35), ry + 10 + (GetTab() * 22), visible ? Color(29, 255, 56) : Color(255, 255, 255), true, title);
}