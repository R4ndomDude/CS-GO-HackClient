#pragma once

namespace MenuElements
{
	class Tab : public MenuElements::Element
	{
	protected:
		char* title;
		std::vector<MenuElements::Element*> items;
	public:
		Tab() {}

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

		void AddItem( MenuElements::Element* item )
		{
			Add( item );
			//item->SetPosition()
		}
	};
}