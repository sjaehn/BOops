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

#ifndef OPTIONDECIMATE_HPP_
#define OPTIONDECIMATE_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionDecimate : public OptionWidget
{
public:
	OptionDecimate () : OptionDecimate (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionDecimate (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		decimateLabel (0, 90, 80, 20, "ctlabel", "Resolution")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", [] (double x) {return 0.01 + 0.99 * x;});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (decimateLabel);
		add (*options[0]);
		add (*options[1]);
	}

	OptionDecimate (const OptionDecimate& that) : OptionWidget (that), decimateLabel (that.decimateLabel)
	{
		add (decimateLabel);
	}

	OptionDecimate& operator= (const OptionDecimate& that)
	{
		release (&decimateLabel);
		OptionWidget::operator= (that);
		decimateLabel = that.decimateLabel;
		add (decimateLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionDecimate (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		decimateLabel.applyTheme (theme);
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

		// options[1] changed ? Send to range
		if (widget == p->getWidget(1)) ((DialRange*)p->getWidget(0))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());

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

		// Send changed range to options[1]
		if ((p == (DialRange*)pp->getWidget(0)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(1))->setValue (p->range.getValue ());
		}
	}

protected:
	BWidgets::Label decimateLabel;
};

#endif /* OPTIONDECIMATE_HPP_ */