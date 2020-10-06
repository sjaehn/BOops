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

#ifndef OPTIONDELAY_HPP_
#define OPTIONDELAY_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionDelay : public OptionWidget
{
public:
	OptionDelay () : OptionDelay (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionDelay (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		stepsLabel (10, 90, 60, 20, "ctlabel", "Range"),
		delayLabel (90, 90, 60, 20, "ctlabel", "Delay")
	{
		try
		{
			options[0] = new Dial (10, 20, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, "%1.0f", "steps", [] (double x) {return LIMIT (1.0 + x * (NR_STEPS - 1.0), 1, NR_STEPS - 1);});
			options[1] = new DialRange (90, 20, 60, 60, "dial", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return x;});
			options[2] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[1])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (stepsLabel);
		add (delayLabel);
		add (*options[0]);
		add (*options[1]);
		add (*options[2]);
	}

	OptionDelay (const OptionDelay& that) : OptionWidget (that), stepsLabel (that.stepsLabel), delayLabel (that.delayLabel)
	{
		add (stepsLabel);
		add (delayLabel);
	}

	OptionDelay& operator= (const OptionDelay& that)
	{
		release (&stepsLabel);
		release (&delayLabel);
		OptionWidget::operator= (that);
		stepsLabel = that.stepsLabel;
		delayLabel = that.delayLabel;
		add (stepsLabel);
		add (delayLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionDelay (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		stepsLabel.applyTheme (theme);
		delayLabel.applyTheme (theme);
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

		const float value = ((BWidgets::ValueWidget*)widget)->getValue();

		if (widget == p->getWidget(0))
		{
			const int steps = LIMIT (1.0 + value * (NR_STEPS - 1.0), 1, NR_STEPS - 1);
			((Dial*)widget)->setUnit (steps == 1? "step" : "steps");
		}

		// options[1] changed ? Send to range
		if (widget == p->getWidget(2)) ((DialRange*)p->getWidget(1))->range.setValue (value);

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
		if ((p == (DialRange*)pp->getWidget(1)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(2))->setValue (p->range.getValue ());
		}
	}

protected:
	BWidgets::Label stepsLabel;
	BWidgets::Label delayLabel;


};

#endif /* OPTIONDELAY_HPP_ */
