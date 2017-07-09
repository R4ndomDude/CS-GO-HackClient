#include "sdk.h"

void MenuElements::Frame::Think()
{
	static ConVar* mouselock = I::Cvar->FindVar("cl_mouseenable");

	if (tick == 0 && GetAsyncKeyState(Config->GetValue("Menu", "Key").m_iValue))
	{
		tick = 30;
		visible = !visible;
		mouselock->SetValue(!visible);
		I::InputSystem->EnableInput(!visible);
		if (visible) I::InputSystem->ResetInputState();
	}

	if (!visible) return;

	static int bWasntHolding = false;
	static int bGrabbing = false;
	static int iOffsetX = 0, iOffsetY = 0;
	
	POINT ptMouse = Input->GetMousePos();

	if (Input->Holding(x, y, width, 28) && !bGrabbing && bWasntHolding)
	{
		bGrabbing = true;
		iOffsetX = x - ptMouse.x;
		iOffsetY = y - ptMouse.y;
	}
	else if (bGrabbing && !GetAsyncKeyState(VK_LBUTTON))
	{
		bGrabbing = false;
		iOffsetX = 0;
		iOffsetY = 0;
	}

	if (bGrabbing)
	{
		x = ptMouse.x + iOffsetX;
		y = ptMouse.y + iOffsetY;
	}
	
	int iScrW, iScrH;
	I::Engine->GetScreenSize(iScrW, iScrH);

	x = Util::Clamp(x, 1, iScrW - width - 1);
	y = Util::Clamp(y, 1, iScrH - height - 1);

	bWasntHolding = Input->Hovering(x, y, width, 28) && !GetAsyncKeyState(VK_LBUTTON);

	//Util::PrintMessage("%d : %d : %d\n", ptMouse.x, ptMouse.y, Input->Holding(x, y, width, 28));
}

void MenuElements::Frame::Paint()
{
	if (!visible) return;

	static float rainbow;

	Draw::DrawOutlinedRect(x - 1, y - 1, width + 2, height + 2, Color(150, 150, 150));
	Draw::DrawRect(x, y, width, height, Color(21, 21, 21, 255));
	Draw::DrawRectRainbow(x, y, width, 2,  0.003f, rainbow);
	Draw::DrawRect(x + 2, y + 28, width - 4, 1, Color(27, 27, 27, 255));
	Draw::DrawRect(x + 90, y + 32, 1, height - 36, Color(27, 27, 27, 255));
	Draw::DrawString(Fonts::Morningtype24, x + width / 2, y + 14, Color(255, 255, 255), true, "%s", "de_stroyer");
}