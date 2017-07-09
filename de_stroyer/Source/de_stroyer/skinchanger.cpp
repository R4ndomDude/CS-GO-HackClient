#include "sdk.h"

void MenuElements::SkinChanger::Think()
{
	visible = parent->GetVisible();

	if (!visible) return;
}

void MenuElements::SkinChanger::Paint()
{
	if (!visible) return;

	Paint1();
	Paint2();
}

void MenuElements::SkinChanger::Paint1()
{
	Draw::DrawOutlinedRect(rx, ry, 140, 356, Color(40, 40, 40));
	Draw::DrawOutlinedRect(rx + 139, ry, 10, 356, Color(40, 40, 40));

	static int current = 3;
	static int scrollamount = 0;
	static int scry = 0;
	static int bWasntHolding = false;
	static int bGrabbing = false;
	static int iOffsetY = 0;

	static auto& _skininfo = SkinsUtil::pSkins->GetData();
	int i = 0;
	for (auto& w : _skininfo) {
		int j = i - scrollamount;
		if (j > -1 && j < 25)
		{
			if (i == current)
			{
				currentWepName = w.first;
				currentWep = w.second;
				Draw::DrawRect(rx + 1, ry + 3 + (j * 14), 138, 14, Color(40, 210, 116, 200));
			}
			if (Input->Clicked(rx + 1, ry + 3 + (j * 14), 138, 14) && !bGrabbing) current = i;
			Draw::DrawString(Fonts::Gothic12, rx + 70, ry + 10 + (j * 14), Color(255, 255, 255), true, "%s", w.first.c_str());
		}
		i++;
	}

	POINT ptMouse = Input->GetMousePos();

	if (Input->Holding(rx + 140, ry + scry + 1, 8, 30) && !bGrabbing && bWasntHolding)
	{
		bGrabbing = true;
		iOffsetY = ry + scry - ptMouse.y;
	}
	else if (bGrabbing && !GetAsyncKeyState(VK_LBUTTON))
	{
		bGrabbing = false;
		iOffsetY = 0;
	}

	bWasntHolding = Input->Hovering(rx + 140, ry + scry + 1, 8, 30) && !GetAsyncKeyState(VK_LBUTTON);

	if (bGrabbing)
	{
		scry = ptMouse.y + iOffsetY - ry;

		scry = Util::Clamp(scry, 0, 324);

		float diff = (324.f / (float(_skininfo.size()) - 25.f));
		scrollamount = scry / diff;
		Util::PrintMessage("%d : %f\n", scry, diff);
	}

	Draw::DrawRect(rx + 140, ry + scry + 1, 8, 30, Color(115, 115, 115));
}

void MenuElements::SkinChanger::Paint2()
{
	int _rx = rx + 160;

	Draw::DrawOutlinedRect(_rx, ry, 140, 356, Color(40, 40, 40));

	static int current = 3;
	static int scrollamount = 0;
	static int scry = 0;
	static int bWasntHolding = false;
	static int bGrabbing = false;
	static int iOffsetY = 0;

	static auto& _skininfo = SkinsUtil::pSkins->GetData();
	int i = 0;
	for (auto& w : currentWep) {
		std::unordered_map<std::string, int>::iterator data = SkinsUtil::data.find(currentWepName);
		if (data == SkinsUtil::data.end()) continue;
		int j = i - scrollamount;
		if (j > -1 && j < 25)
		{
			if (w._id == data->second)
				Draw::DrawRect(_rx + 1, ry + 3 + (j * 14), 138, 14, Color(40, 210, 116, 200));
			if (Input->Clicked(_rx + 1, ry + 3 + (j * 14), 138, 14) && !bGrabbing) data->second = w._id;
			Draw::DrawString(Fonts::Gothic12, _rx + 70, ry + 10 + (j * 14), Color(255, 255, 255), true, "%s", w._name.c_str());
		}
		i++;
	}

	if (currentWep.size() <= 25) return;

	Draw::DrawOutlinedRect(_rx + 139, ry, 10, 356, Color(40, 40, 40));

	POINT ptMouse = Input->GetMousePos();

	if (Input->Holding(_rx + 140, ry + scry + 1, 8, 30) && !bGrabbing && bWasntHolding)
	{
		bGrabbing = true;
		iOffsetY = ry + scry - ptMouse.y;
	}
	else if (bGrabbing && !GetAsyncKeyState(VK_LBUTTON))
	{
		bGrabbing = false;
		iOffsetY = 0;
	}

	bWasntHolding = Input->Hovering(_rx + 140, ry + scry + 1, 8, 30) && !GetAsyncKeyState(VK_LBUTTON);

	if (bGrabbing)
	{
		scry = ptMouse.y + iOffsetY - ry;

		scry = Util::Clamp(scry, 0, 324);

		float diff = (324.f / (float(currentWep.size()) - 25.f));
		scrollamount = scry / diff;
	}

	Draw::DrawRect(_rx + 140, ry + scry + 1, 8, 30, Color(115, 115, 115));
}