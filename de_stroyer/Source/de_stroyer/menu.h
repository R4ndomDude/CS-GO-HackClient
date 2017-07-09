#pragma once

#include "elements.h"
#include "callbacks.h"

class CMenu
{
private:
	std::vector<MenuElements::Element*> elements;
	MenuElements::Element* frame;
public:
	void Setup();
	void Paint();
	void Add(MenuElements::Element* element);
	MenuElements::Element* GetFrame();
}; extern CMenu* Menu;