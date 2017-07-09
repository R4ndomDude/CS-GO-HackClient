#pragma once

namespace MenuElements
{
	class Slider : public MenuElements::Element
	{
	protected:
		float min, max;
		char* title;
		ConfigValue* value;
	public:
		Slider()
		{
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

		void SetMin(float min_)
		{
			min = min_;
		}

		void SetMax(float max_)
		{
			max = max_;
		}
	};
}