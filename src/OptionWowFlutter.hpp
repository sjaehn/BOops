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

#ifndef OPTIONWOWFLUTTER_HPP_
#define OPTIONWOWFLUTTER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionWowFlutter : public OptionWidget
{
public:
	OptionWowFlutter () : OptionWowFlutter (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionWowFlutter (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		wowLabel (50, 80, 60, 20, "ctlabel", "Wow"),
		flutterLabel (210, 80, 60, 20, "ctlabel", "Flutter"),
		wowDepthLabel (10, 100, 60, 20, "ctlabel", "Depth"),
		wowRateLabel (90, 100, 60, 20, "ctlabel", "Rate"),
		flutterDepthLabel (170, 100, 60, 20, "ctlabel", "Depth"),
		flutterRateLabel (250, 100, 60, 20, "ctlabel", "Rate")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "%");
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 0.0625 + 0.9375 * x;});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "%");
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new DialRange (250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 1.0 + 15.0 * x;});
			options[7] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 8; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (wowLabel);
		add (flutterLabel);
		add (wowDepthLabel);
		add (wowRateLabel);
		add (flutterDepthLabel);
		add (flutterRateLabel);
		for (int i = 0; i < 8; ++i) add (*options[i]);
	}

	OptionWowFlutter (const OptionWowFlutter& that) :
		OptionWidget (that), wowLabel (that.wowLabel), flutterLabel (that.flutterLabel),
		wowDepthLabel (that.wowDepthLabel), wowRateLabel (that.wowRateLabel),
		flutterDepthLabel (that.flutterDepthLabel), flutterRateLabel (that.flutterRateLabel)
	{
		add (wowLabel);
		add (flutterLabel);
		add (wowDepthLabel);
		add (wowRateLabel);
		add (flutterDepthLabel);
		add (flutterRateLabel);
	}

	OptionWowFlutter& operator= (const OptionWowFlutter& that)
	{
		release (&wowLabel);
		release (&flutterLabel);
		release (&wowDepthLabel);
		release (&wowRateLabel);
		release (&flutterDepthLabel);
		release (&flutterRateLabel);
		OptionWidget::operator= (that);
		wowLabel = that.wowLabel;
		flutterLabel = that.flutterLabel;
		wowDepthLabel = that.wowDepthLabel;
		wowRateLabel = that.wowRateLabel;
		flutterDepthLabel = that.flutterDepthLabel;
		flutterRateLabel = that.flutterRateLabel;
		add (wowLabel);
		add (flutterLabel);
		add (wowDepthLabel);
		add (wowRateLabel);
		add (flutterDepthLabel);
		add (flutterRateLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionWowFlutter (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		wowLabel.applyTheme (theme);
		flutterLabel.applyTheme (theme);
		wowDepthLabel.applyTheme (theme);
		wowRateLabel.applyTheme (theme);
		flutterDepthLabel.applyTheme (theme);
		flutterRateLabel.applyTheme (theme);
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
		for (int i = 0; i < 8; i += 2)
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
		for (int i = 0; i < 8; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label wowLabel;
	BWidgets::Label flutterLabel;
	BWidgets::Label wowDepthLabel;
	BWidgets::Label wowRateLabel;
	BWidgets::Label flutterDepthLabel;
	BWidgets::Label flutterRateLabel;


};

#endif /* OPTIONWOWFLUTTER_HPP_ */
