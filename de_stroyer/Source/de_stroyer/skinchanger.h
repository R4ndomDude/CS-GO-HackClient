#pragma once

namespace MenuElements
{
	class SkinChanger : public MenuElements::Element
	{
	protected:
		Skins::vecSkinInfo currentWep;
		std::string currentWepName;
	public:
		SkinChanger() {}

		void Think();
		void Paint();
		void Paint1();
		void Paint2();
	};
}