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

#ifndef OPTIONSTUTTER_HPP_
#define OPTIONSTUTTER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "Dial.hpp"
#include "VSlider.hpp"

class OptionStutter : public OptionWidget
{
public:
	OptionStutter () : OptionStutter (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionStutter (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		stepsLabel (10, 90, 60, 20, "ctlabel", "Stutters"),
		smoothLabel (90, 90, 60, 20, "ctlabel", "Smooth")

	{
		try
		{
			options[0] = new Dial (10, 20, 60, 60, "pad0", 1.0, 0.0, 1.0, 0.0, "%1.0f", "", [] (double x) {return LIMIT (int (2.0 + 7.0 * x), 2, 8);});
			options[1] = new Dial (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.2f", "", [] (double x) {return x;});
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 2; ++i) options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		for (int i = 0; i < 2; ++i) add (*options[i]);
		add (stepsLabel);
		add (smoothLabel);
	}

	OptionStutter (const OptionStutter& that) :
		OptionWidget (that), stepsLabel (that.stepsLabel), smoothLabel (that.smoothLabel)
	{
		add (stepsLabel);
		add (smoothLabel);
	}

	OptionStutter& operator= (const OptionStutter& that)
	{
		release (&stepsLabel);
		release (&smoothLabel);
		OptionWidget::operator= (that);
		stepsLabel = that.stepsLabel;
		smoothLabel = that.smoothLabel;
		add (stepsLabel);
		add (smoothLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionStutter (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		stepsLabel.applyTheme (theme);
		smoothLabel.applyTheme (theme);
	}

	/*static void valueChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		OptionStutter* p = (OptionStutter*) widget->getParent();
		if (!p) return;
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
		if (!ui) return;

		// Forward all changed options to ui
		ui->optionChangedCallback (event);
	}*/

protected:
	BWidgets::Label stepsLabel;
	BWidgets::Label smoothLabel;
};

#endif /* OPTIONSTUTTER_HPP_ */
