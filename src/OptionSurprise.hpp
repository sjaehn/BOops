/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OPTIONSURPRISE_HPP_
#define OPTIONSURPRISE_HPP_

#define FX_SURPRISE_RATIO 0
#define FX_SURPRISE_SLOT 1
#define FX_SURPRISE_NR 6

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/PopupListBox.hpp"
#include "DialRange.hpp"

class OptionSurprise : public OptionWidget
{
public:
	OptionSurprise () : OptionSurprise (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionSurprise (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		screen (0, 0, 160, 130, "screen")
	{
		for (int i = 0; i < FX_SURPRISE_NR; ++i)
		{
			try
			{
				options[i * 2] = new Dial (170 + i * 80, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.2f");
				options[i * 2 + 1] = new BWidgets::PopupListBox
				(
					170 + i * 80, 90, 60, 20, 0, -80, 60, 80, "menu",
					BItems::ItemList
					(
						{
							{0, "None"}, {1.0/16.0, "Slot 1"}, {2.0/16.0, "Slot 2"}, {3.0/16.0, "Slot 3"}, {4.0/16.0, "Slot 4"},
							{5.0/16.0, "Slot 5"}, {6.0/16.0, "Slot 6"}, {7.0/16.0, "Slot 7"}, {8.0/16.0, "Slot 8"},
							{9.0/16.0, "Slot 9"}, {10.0/16.0, "Slot 10"}, {11.0/16.0, "Slot 11"}, {12.0/16.0, "Slot 12"}
						}
					),
					0
				);
			}
			catch (std::bad_alloc& ba) {throw ba;}
		}

		for (int i = 0; i < FX_SURPRISE_NR; ++i)
		{
			options[2 * i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			options[2 * i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		for (int i = 0; i < FX_SURPRISE_NR; ++i)
		{
			add (*options[2 * i]);
			add (*options[2 * i + 1]);
		}

		add (screen);
	}

	OptionSurprise (const OptionSurprise& that) : OptionWidget (that), screen (that.screen)
	{
		add (screen);
	}

	OptionSurprise& operator= (const OptionSurprise& that)
	{
		release (&screen);
		OptionWidget::operator= (that);
		screen = that.screen;
		add (screen);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionSurprise (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		screen.applyTheme (theme);
	}

protected:
	BWidgets::Widget screen;
};

#endif /* OPTIONSURPRISE_HPP_ */
