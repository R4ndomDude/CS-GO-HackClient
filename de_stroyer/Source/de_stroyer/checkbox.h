#pragma once

namespace MenuElements
{
	class Checkbox : public MenuElements::Element
	{
	protected:
		int size;
		char* title;
		ConfigValue* value;
	public:
		Checkbox()
		{
			size = 16;
			title = "";
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