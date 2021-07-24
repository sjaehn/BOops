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

#ifndef OPTIONBANGER_HPP_
#define OPTIONBANGER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionBanger : public OptionWidget
{
public:
	OptionBanger () : OptionBanger (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionBanger (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		gainLabel (0, 90, 80, 20, "ctlabel", BOOPS_LABEL_GAIN),
		firstLabel (80, 90, 80, 20, "ctlabel", BOOPS_LABEL_FIRST),
		lastLabel (160, 90, 80, 20, "ctlabel", BOOPS_LABEL_LAST),
		nukeLabel (240, 90, 80, 20, "ctlabel", BOOPS_LABEL_NUKE),
		speedLabel (320, 90, 80, 20, "ctlabel", BOOPS_LABEL_SPEED),
		spinLabel (400, 90, 80, 20, "ctlabel", BOOPS_LABEL_SPIN)
	{
		try
		{
			for (int i = 0; i < 5; ++i)
			{
				options[2 * i] = new DialRange (i * 80 + 10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
				options[2 * i + 1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			}
			options[10] = new DialRange (410, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 2.0 * x - 1.0;}, [] (double x) {return 0.5 * (LIMIT (x, -1, 1) + 1.0);});
			options[11] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 12; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (gainLabel);
		add (firstLabel);
		add (lastLabel);
		add (nukeLabel);
		add (speedLabel);
		add (spinLabel);
		for (int i = 0; i < 12; ++i) add (*options[i]);
	}

	OptionBanger (const OptionBanger& that) :
		OptionWidget (that),
		gainLabel (that.gainLabel), firstLabel (that.firstLabel),
		lastLabel (that.lastLabel), nukeLabel (that.nukeLabel), 
		speedLabel (that.speedLabel), spinLabel (that.spinLabel)
	{
		add (gainLabel);
		add (firstLabel);
		add (lastLabel);
		add (nukeLabel);
		add (speedLabel);
		add (spinLabel);
	}

	OptionBanger& operator= (const OptionBanger& that)
	{
		release (&gainLabel);
		release (&firstLabel);
		release (&lastLabel);
		release (&nukeLabel);
		release (&speedLabel);
		release (&spinLabel);
		OptionWidget::operator= (that);
		gainLabel = that.gainLabel;
		firstLabel = that.firstLabel;
		lastLabel = that.lastLabel;
		nukeLabel = that.nukeLabel;
		speedLabel = that.speedLabel;
		spinLabel = that.spinLabel;
		add (gainLabel);
		add (firstLabel);
		add (lastLabel);
		add (nukeLabel);
		add (speedLabel);
		add (spinLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionBanger (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		gainLabel.applyTheme (theme);
		firstLabel.applyTheme (theme);
		lastLabel.applyTheme (theme);
		nukeLabel.applyTheme (theme);
		speedLabel.applyTheme (theme);
		spinLabel.applyTheme (theme);
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
		for (int i = 0; i < 12; i += 2)
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
		for (int i = 0; i < 12; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label gainLabel;
	BWidgets::Label firstLabel;
	BWidgets::Label lastLabel;
	BWidgets::Label nukeLabel;
	BWidgets::Label speedLabel;
	BWidgets::Label spinLabel;
};

#endif /* OPTIONBANGER_HPP_ */
