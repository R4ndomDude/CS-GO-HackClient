#pragma once

namespace MenuElements
{
	class Frame : public MenuElements::Element
	{
	protected:

	public:
		Frame()
		{
			//ox = 4;
			//oy = 32;
			static ConVar* mouselock = I::Cvar->FindVar("cl_mouseenable");
			mouselock->SetValue(0);
			visible = true;
			I::InputSystem->EnableInput(false);
		}

		void Think();
		void Paint();
	};
}