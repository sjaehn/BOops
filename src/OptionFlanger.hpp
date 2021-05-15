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

#ifndef OPTIONFLANGER_HPP_
#define OPTIONFLANGER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionFlanger : public OptionWidget
{
public:
	OptionFlanger () : OptionFlanger (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionFlanger (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		minDelayLabel (10, 100, 60, 20, "ctlabel", BOOPS_LABEL_DELAY),
		modDelayLabel (90, 100, 60, 20, "ctlabel", BOOPS_LABEL_AMOUNT),
		freqLabel (160, 100, 80, 20, "ctlabel", BOOPS_LABEL_MODULATION),
		phaseLabel (240, 100, 80, 20, "ctlabel", BOOPS_LABEL_STEREO_PHASE),
		feedbackLabel (330, 100, 60, 20, "ctlabel", BOOPS_LABEL_FEEDBACK)
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", BOOPS_LABEL_MS, [] (double x) {return 10.0 * x;}, [] (double x) {return 0.1 * LIMIT (x, 0.0, 10.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", BOOPS_LABEL_MS, [] (double x) {return 10.0 * x;}, [] (double x) {return 0.1 * LIMIT (x, 0.0, 10.0);});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", BOOPS_LABEL_HZ, [] (double x) {return 10.0 * pow (x, 3);}, [] (double x) {return pow (0.1 * LIMIT (x, 0.0, 10.0), 1.0 / 3.0);});
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new DialRange (250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "°", [] (double x) {return 360.0 * x;}, [] (double x) {return LIMIT (x, 0.0, 360.0) / 360.0;});
			options[7] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[8] = new DialRange (330, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 2.0 * x - 1.0;}, [] (double x) {return (LIMIT (x, -1.0, 1.0) + 1.0) / 2.0;});
			options[9] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 10; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (minDelayLabel);
		add (modDelayLabel);
		add (freqLabel);
		add (phaseLabel);
		add (feedbackLabel);
		for (int i = 0; i < 10; ++i) add (*options[i]);
	}

	OptionFlanger (const OptionFlanger& that) :
		OptionWidget (that),
		minDelayLabel (that.minDelayLabel), modDelayLabel (that.modDelayLabel),
		freqLabel (that.freqLabel), phaseLabel (that.phaseLabel), feedbackLabel (that.feedbackLabel)
	{
		add (minDelayLabel);
		add (modDelayLabel);
		add (freqLabel);
		add (phaseLabel);
		add (feedbackLabel);
	}

	OptionFlanger& operator= (const OptionFlanger& that)
	{
		release (&minDelayLabel);
		release (&modDelayLabel);
		release (&freqLabel);
		release (&phaseLabel);
		release (&feedbackLabel);
		OptionWidget::operator= (that);
		minDelayLabel = that.minDelayLabel;
		modDelayLabel = that.modDelayLabel;
		freqLabel = that.freqLabel;
		phaseLabel = that.phaseLabel;
		feedbackLabel = that.feedbackLabel;
		add (minDelayLabel);
		add (modDelayLabel);
		add (freqLabel);
		add (phaseLabel);
		add (feedbackLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionFlanger (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		minDelayLabel.applyTheme (theme);
		modDelayLabel.applyTheme (theme);
		freqLabel.applyTheme (theme);
		phaseLabel.applyTheme (theme);
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
	BWidgets::Label minDelayLabel;
	BWidgets::Label modDelayLabel;
	BWidgets::Label freqLabel;
	BWidgets::Label phaseLabel;
	BWidgets::Label feedbackLabel;
};

#endif /* OPTIONFLANGER_HPP_ */
