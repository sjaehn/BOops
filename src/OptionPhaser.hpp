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

#ifndef OPTIONPHASER_HPP_
#define OPTIONPHASER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionPhaser : public OptionWidget
{
public:
	OptionPhaser () : OptionPhaser (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionPhaser (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		loFreqLabel (10, 100, 60, 20, "ctlabel", "Low Freq."),
		hiFreqLabel (90, 100, 60, 20, "ctlabel", "High Freq."),
		modRateLabel (160, 100, 80, 20, "ctlabel", "Modulation"),
		modPhaseLabel (240, 100, 80, 20, "ctlabel", "Stereo phase"),
		stepsLabel (330, 100, 60, 20, "ctlabel", "Steps"),
		feedbackLabel (410, 100, 60, 20, "ctlabel", "Feedback")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 3.0);}, [] (double x) {return pow ((LIMIT (x, 20, 20000) - 20.0) / 19980.0, 1.0 / 3.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 3.0);}, [] (double x) {return pow ((LIMIT (x, 20, 20000) - 20.0) / 19980.0, 1.0 / 3.0);});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "Hz", [] (double x) {return 10.0 * pow (x, 3.0);}, [] (double x) {return pow (LIMIT (x, 0.0, 10.0) / 10.0, 1.0 / 3.0);});
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new DialRange (250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "°", [] (double x) {return 360.0 * x;}, [] (double x) {return x / 360.0;});
			options[7] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[8] = new DialRange (410, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 2.0 * x - 1.0;}, [] (double x) {return (x + 1.0) / 2.0;});
			options[9] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[10] = new Dial (330, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "", [] (double x) {return floor (1.0 + LIMIT (x * 10.0, 0, 9));}, [] (double x) {return (x - 0.99999) / 10.0;});
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 10; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}
		options[10]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (loFreqLabel);
		add (hiFreqLabel);
		add (modRateLabel);
		add (modPhaseLabel);
		add (stepsLabel);
		add (feedbackLabel);
		for (int i = 0; i < 11; ++i) add (*options[i]);
	}

	OptionPhaser (const OptionPhaser& that) :
		OptionWidget (that),
		loFreqLabel (that.loFreqLabel), hiFreqLabel (that.hiFreqLabel),
		modRateLabel (that.modRateLabel), modPhaseLabel (that.modPhaseLabel),
		stepsLabel (that.stepsLabel), feedbackLabel (that.feedbackLabel)
	{
		add (loFreqLabel);
		add (hiFreqLabel);
		add (modRateLabel);
		add (modPhaseLabel);
		add (stepsLabel);
		add (feedbackLabel);
	}

	OptionPhaser& operator= (const OptionPhaser& that)
	{
		release (&loFreqLabel);
		release (&hiFreqLabel);
		release (&modRateLabel);
		release (&modPhaseLabel);
		release (&stepsLabel);
		release (&feedbackLabel);
		OptionWidget::operator= (that);
		loFreqLabel = that.loFreqLabel;
		hiFreqLabel = that.hiFreqLabel;
		modRateLabel = that.modRateLabel;
		modPhaseLabel = that.modPhaseLabel;
		stepsLabel = that.stepsLabel;
		feedbackLabel = that.feedbackLabel;
		add (loFreqLabel);
		add (hiFreqLabel);
		add (modRateLabel);
		add (modPhaseLabel);
		add (stepsLabel);
		add (feedbackLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionPhaser (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		loFreqLabel.applyTheme (theme);
		hiFreqLabel.applyTheme (theme);
		modRateLabel.applyTheme (theme);
		modPhaseLabel.applyTheme (theme);
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

		// options[i + 1] changed ? Send to range
		for (int i = 0; i < 10; i += 2)
		{
			if (widget == p->getWidget(i + 1))
			{
				((DialRange*)p->getWidget(i))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
				break;
			}
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

		// Send changed range to options[i + 1]
		for (int i = 0; i < 10; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label loFreqLabel;
	BWidgets::Label hiFreqLabel;
	BWidgets::Label modRateLabel;
	BWidgets::Label modPhaseLabel;
	BWidgets::Label stepsLabel;
	BWidgets::Label feedbackLabel;
};

#endif /* OPTIONPHASER_HPP_ */
