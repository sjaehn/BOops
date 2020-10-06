/* B.Noname01
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

#ifndef OPTIONTAPESTOP_HPP_
#define OPTIONTAPESTOP_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionTapeStop : public OptionWidget
{
public:
	OptionTapeStop () : OptionTapeStop (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionTapeStop (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		distLabel (10, 90, 60, 20, "ctlabel", "Reach"),
		ordLabel (80, 90, 80, 20, "ctlabel", "Deceleration")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 1.0 + 9.0 * x;});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[2])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (distLabel);
		add (ordLabel);
		add (*options[0]);
		add (*options[1]);
		add (*options[2]);
		add (*options[3]);
	}

	OptionTapeStop (const OptionTapeStop& that) : OptionWidget (that), distLabel (that.distLabel), ordLabel (that.ordLabel)
	{
		add (distLabel);
		add (ordLabel);
	}

	OptionTapeStop& operator= (const OptionTapeStop& that)
	{
		release (&distLabel);
		release (&ordLabel);
		OptionWidget::operator= (that);
		distLabel = that.distLabel;
		ordLabel = that.ordLabel;
		add (distLabel);
		add (ordLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionTapeStop (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		distLabel.applyTheme (theme);
		ordLabel.applyTheme (theme);
	}

	static void valueChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		OptionWidget* p = (OptionWidget*) widget->getParent();
		if (!p) return;
		BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
		if (!ui) return;

		// options[1 or 3] changed ? Send to range
		if (widget == p->getWidget(1)) ((DialRange*)p->getWidget(0))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
		else if (widget == p->getWidget(3)) ((DialRange*)p->getWidget(2))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());

		// Forward all changed options to ui
		ui->optionChangedCallback (event);
	}

	static void rangeChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		DialRange* p = (DialRange*) widget->getParent();
		if (!p) return;
		OptionWidget* pp = (OptionWidget*) p->getParent();
		if (!pp) return;

		// Send changed range to options[1 or 3]
		if ((p == (DialRange*)pp->getWidget(0)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(1))->setValue (p->range.getValue ());
		}

		else if ((p == (DialRange*)pp->getWidget(2)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(3))->setValue (p->range.getValue ());
		}
	}

protected:
	BWidgets::Label distLabel;
	BWidgets::Label ordLabel;


};

#endif /* OPTIONTAPESTOP_HPP_ */
