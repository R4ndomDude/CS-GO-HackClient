#include "sdk.h"

void MenuElements::Key::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;

	if (waiting)
	{
		for (int i = 1; i < 112; i++)
		{
			if (i != 3 && i != 13)
			{
				// ghetto af but again i don't care
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					tick = 30;
					value->m_iValue = VK_LBUTTON;
					waiting = false;
				}

				if (GetAsyncKeyState(i) == -32767 && tick == 0)
				{
					value->m_iValue = i;
					waiting = false;
				}
			}
		}
	}
	else if (Input->Clicked(rx, ry, width, height) && tick == 0)
	{
		waiting = true;
	}
}

void MenuElements::Key::Paint()
{
	if (!visible) return;

	Color color = Color(27, 195, 106);
	Color outlineColor = Color(color.r(), color.g(), color.b(), 100);

	Draw::DrawOutlinedRect(rx, ry, width, height, Color(140, 140, 140));
	Draw::DrawRect(rx + 1, ry + 1, width - 2, height - 2, Color(27, 27, 27));

	Draw::DrawString(Fonts::Gothic16, rx + width / 2, ry + height / 2, Color(255, 255, 255), true, waiting ? "waiting" : szKeyNames[value->m_iValue]);

	Draw::DrawString(Fonts::Gothic16, rx + width + 4, ry + height / 2, Color(255, 255, 255), false, title);
}

// this is stupid but i don't care.

char* szKeyNames[] = {
	"",
	"Mouse 1",
	"Mouse 2",
	"",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide"
};