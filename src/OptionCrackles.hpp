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

#ifndef OPTIONCRACKLES_HPP_
#define OPTIONCRACKLES_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionCrackles : public OptionWidget
{
public:
	OptionCrackles () : OptionCrackles (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionCrackles (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		rateLabel (10, 90, 60, 20, "ctlabel", "Rate"),
		maxsizeLabel (90, 90, 60, 20, "ctlabel", "Max. size"),
		distribLabel (160, 90, 80, 20, "ctlabel", "Distribution")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 200 * x;});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 0.2 * x;});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 10.0 * x;});
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 3; ++i)
		{
			options[2 * i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[2 * i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[2 * i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (rateLabel);
		add (maxsizeLabel);
		add (distribLabel);
		for (int i = 0; i < 6; ++i) add (*options[i]);
	}

	OptionCrackles (const OptionCrackles& that) : OptionWidget (that), rateLabel (that.rateLabel), maxsizeLabel (that.maxsizeLabel), distribLabel (that.distribLabel)
	{
		add (rateLabel);
		add (maxsizeLabel);
		add (distribLabel);
	}

	OptionCrackles& operator= (const OptionCrackles& that)
	{
		release (&rateLabel);
		release (&maxsizeLabel);
		release (&distribLabel);
		OptionWidget::operator= (that);
		rateLabel = that.rateLabel;
		maxsizeLabel = that.maxsizeLabel;
		distribLabel = that.distribLabel;
		add (rateLabel);
		add (maxsizeLabel);
		add (distribLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionCrackles (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		rateLabel.applyTheme (theme);
		maxsizeLabel.applyTheme (theme);
		distribLabel.applyTheme (theme);
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
		for (int i = 0; i < 3; ++i)
		{
			if (widget == p->getWidget(2 * i + 1)) ((DialRange*)p->getWidget(2 * i))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
		}

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
		for (int i = 0; i < 3; ++i)
		{
			if ((p == (DialRange*)pp->getWidget(2 * i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(2 * i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label rateLabel;
	BWidgets::Label maxsizeLabel;
	BWidgets::Label distribLabel;


};

#endif /* OPTIONCRACKLES_HPP_ */
