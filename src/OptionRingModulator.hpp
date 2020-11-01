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

#ifndef OPTIONRINGMOD_HPP_
#define OPTIONRINGMOD_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"
#include "BWidgets/ListBox.hpp"

class OptionRingModulator : public OptionWidget
{
public:
	OptionRingModulator () : OptionRingModulator (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionRingModulator (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		ratioLabel (10, 90, 60, 20, "ctlabel", "Ratio"),
		freqLabel (80, 90, 80, 20, "ctlabel", "Frequency")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20000.0 * pow (x, 4.0);});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new BWidgets::ListBox
			(
				160, 20, 80, 90, "menu",
				BItems::ItemList ({{0.0, "Sine"}, {0.125, "Triangle"}, {0.25, "Square"}, {0.375, "Saw"}, {0.5, "Reverse saw"}}),
				0.3
			);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[2])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[4]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (ratioLabel);
		add (freqLabel);
		add (*options[0]);
		add (*options[1]);
		add (*options[2]);
		add (*options[3]);
		add (*options[4]);
	}

	OptionRingModulator (const OptionRingModulator& that) : OptionWidget (that), ratioLabel (that.ratioLabel), freqLabel (that.freqLabel)
	{
		add (ratioLabel);
		add (freqLabel);
	}

	OptionRingModulator& operator= (const OptionRingModulator& that)
	{
		release (&ratioLabel);
		release (&freqLabel);
		OptionWidget::operator= (that);
		ratioLabel = that.ratioLabel;
		freqLabel = that.freqLabel;
		add (ratioLabel);
		add (freqLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionRingModulator (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		ratioLabel.applyTheme (theme);
		freqLabel.applyTheme (theme);
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

		// options[0 or 2] changed ? Send to range
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

		// Send changed range to options[0 or 2]
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
	BWidgets::Label ratioLabel;
	BWidgets::Label freqLabel;


};

#endif /* OPTIONRINGMOD_HPP_ */
