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

#ifndef OPTIONWAH_HPP_
#define OPTIONWAH_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionWah : public OptionWidget
{
public:
	OptionWah () : OptionWah (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionWah (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		lfoFreqLabel (10, 100, 60, 20, "ctlabel", "LFO Freq."),
		lfoPhaseLabel (80, 100, 80, 20, "ctlabel", "LFO Phase"),
		cFreqLabel (240, 100, 80, 20, "ctlabel", "Frequency"),
		depthLabel (170, 100, 60, 20, "ctlabel", "Depth"),
		widthLabel (330, 100, 60, 20, "ctlabel", "Width"),
		orderLabel (410, 100, 60, 20, "ctlabel", "Roll off")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "Hz", [] (double x) {return 0.01 + 9.99 * pow (x, 3.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 3.0);});
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[7] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[8] = new DialRange (330, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[9] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[10] = new Dial (410, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "-db/o", [] (double x) {return 12 * int (LIMIT (1.0 + 8.0 * x, 1, 8));});
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 10; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}
		options[10]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (lfoFreqLabel);
		add (lfoPhaseLabel);
		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);
		for (int i = 0; i < 11; ++i) add (*options[i]);
	}

	OptionWah (const OptionWah& that) :
		OptionWidget (that),
		lfoFreqLabel (that.lfoFreqLabel), lfoPhaseLabel (that.lfoPhaseLabel),
		cFreqLabel (that.cFreqLabel), depthLabel (that.depthLabel),
		widthLabel (that.widthLabel), orderLabel (that.orderLabel)
	{
		add (lfoFreqLabel);
		add (lfoPhaseLabel);
		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);
	}

	OptionWah& operator= (const OptionWah& that)
	{
		release (&lfoFreqLabel);
		release (&lfoPhaseLabel);
		release (&cFreqLabel);
		release (&depthLabel);
		release (&orderLabel);
		release (&widthLabel);
		OptionWidget::operator= (that);
		lfoFreqLabel = that.lfoFreqLabel;
		lfoPhaseLabel = that.lfoPhaseLabel;
		cFreqLabel = that.cFreqLabel;
		depthLabel = that.depthLabel;
		orderLabel = that.orderLabel;
		widthLabel = that.widthLabel;
		add (lfoFreqLabel);
		add (lfoPhaseLabel);
		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionWah (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		lfoFreqLabel.applyTheme (theme);
		lfoPhaseLabel.applyTheme (theme);
		cFreqLabel.applyTheme (theme);
		depthLabel.applyTheme (theme);
		widthLabel.applyTheme (theme);
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
	BWidgets::Label lfoFreqLabel;
	BWidgets::Label lfoPhaseLabel;
	BWidgets::Label cFreqLabel;
	BWidgets::Label depthLabel;
	BWidgets::Label widthLabel;
	BWidgets::Label orderLabel;
};

#endif /* OPTIONWAH_HPP_ */
