#pragma once

typedef void(*ButtonCallback)(void);

namespace MenuElements
{
	class Button : public MenuElements::Element
	{
	protected:
		char* title;
		ButtonCallback callback;
	public:
		Button()
		{
			title = "";
		}

		void Think();
		void Paint();

		void SetTitle(char* title_)
		{
			title = title_;
		}

		char* GetTitle()
		{
			return title;
		}

		void SetCallback(ButtonCallback callback_)
		{
			callback = callback_;
		}
	};
}

// this is stupid but i don't care.
char* szKeyNames[];