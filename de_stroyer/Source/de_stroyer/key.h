#pragma once

namespace MenuElements
{
	class Key : public MenuElements::Element
	{
	protected:
		bool waiting;
		char* title;
		ConfigValue* value;
	public:
		Key()
		{
			title = "";
			waiting = false;
			value = NULL;
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

		void SetValue(ConfigValue* value_)
		{
			value = value_;
		}

		ConfigValue* GetValue()
		{
			return value;
		}
	};
}

// this is stupid but i don't care.
char* szKeyNames[];