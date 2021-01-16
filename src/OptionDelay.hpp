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
		delayLabel (90, 90, 60, 20, "ctlabel", "Delay"),
		feedbackLabel (170, 90, 60, 20, "ctlabel", "Feedback")
	{
		try
		{
			options[0] = new Dial (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "steps", [] (double x) {return floor (LIMIT (1.0 + x * NR_STEPS, 1, NR_STEPS - 1));}, [] (double x) {return (x - 1.0) / NR_STEPS;});
			options[1] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[2] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[3] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[4] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[1])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[3])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[4]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (stepsLabel);
		add (delayLabel);
		add (feedbackLabel);
		for (int i = 0; i < 5; ++i) add (*options[i]);
	}

	OptionDelay (const OptionDelay& that) : OptionWidget (that), stepsLabel (that.stepsLabel), delayLabel (that.delayLabel), feedbackLabel (that.feedbackLabel)
	{
		add (stepsLabel);
		add (delayLabel);
		add (feedbackLabel);
	}

	OptionDelay& operator= (const OptionDelay& that)
	{
		release (&stepsLabel);
		release (&delayLabel);
		release (&feedbackLabel);
		OptionWidget::operator= (that);
		stepsLabel = that.stepsLabel;
		delayLabel = that.delayLabel;
		feedbackLabel = that.feedbackLabel;
		add (stepsLabel);
		add (delayLabel);
		add (feedbackLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionDelay (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		stepsLabel.applyTheme (theme);
		delayLabel.applyTheme (theme);
		feedbackLabel.applyTheme (theme);
	}

	static void valueChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		OptionWidget* p = (OptionWidget*) widget->getParent();
		if (!p) return;
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
		if (!ui) return;

		const float value = ((BWidgets::ValueWidget*)widget)->getValue();

		if (widget == p->getWidget(0))
		{
			const int steps = LIMIT (1.0 + value * (NR_STEPS - 1.0), 1, NR_STEPS - 1);
			((Dial*)widget)->setUnit (steps == 1? "step" : "steps");
		}

		// options[2 or 4] changed ? Send to range
		if (widget == p->getWidget(2)) ((DialRange*)p->getWidget(1))->range.setValue (value);
		if (widget == p->getWidget(4)) ((DialRange*)p->getWidget(3))->range.setValue (value);

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

		// Send changed range to options
		if ((p == (DialRange*)pp->getWidget(1)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(2))->setValue (p->range.getValue ());
		}

		else if ((p == (DialRange*)pp->getWidget(3)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(4))->setValue (p->range.getValue ());
		}
	}

protected:
	BWidgets::Label stepsLabel;
	BWidgets::Label delayLabel;
	BWidgets::Label feedbackLabel;
};

#endif /* OPTIONDELAY_HPP_ */
