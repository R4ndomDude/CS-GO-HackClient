#pragma once

namespace MenuElements
{
	class Element
	{
	protected:
		int x, y, rx, ry, ox, oy, width, height, tab, tick;
		bool visible; 
		Element* parent;
		std::vector<Element*> children;
	public:
		Element()
		{
			x = y = 0;
			rx = ry = 0;
			ox = oy = 0;
			width = height = 100;
			tick = 0;
			tab = 0;
			parent = NULL;
			visible = false;
		}

		void Tick()
		{
			if (tick > 0) --tick;

			if (parent)
			{
				rx = x + parent->ox + parent->rx;
				ry = y + parent->ox + parent->ry;
			}
			else
			{
				rx = x + ox;
				ry = y + oy;
			}

			Think();
			Paint();

			for (Element* pChild : children)
			{
				pChild->Tick();
			}
		}

		virtual void Think() {}
		virtual void Paint() {}

		void SetPosition(int x_, int y_)
		{
			x = x_;
			y = y_;
		}

		void GetPosition(int& x_, int& y_)
		{
			x_ = x;
			y_ = y;
		}

		void SetSize(int width_, int height_)
		{
			width = width_;
			height = height_;
		}

		void GetSize(int& width_, int& height_)
		{
			width = width_;
			height = height_;
		}

		void SetVisible(bool visible_)
		{
			visible = visible_;
		}

		bool GetVisible()
		{
			return visible;
		}

		void SetTab(int tab_)
		{
			tab = tab_;
		}

		int GetTab()
		{
			return tab;
		}

		int GetRealX()
		{
			return rx;
		}

		int GetRealY()
		{
			return ry;
		}

		int GetOffsetX()
		{
			return ox;
		}

		int GetOffsetY()
		{
			return oy;
		}

		void Add(Element* pElement)
		{
			pElement->parent = this;
			children.push_back(pElement);
		}
	};
}

#include "frame.h"
#include "tab.h"
#include "checkbox.h"
#include "slider.h"
#include "key.h"
#include "colorpicker.h"
#include "skinchanger.h"
#include "button.h"