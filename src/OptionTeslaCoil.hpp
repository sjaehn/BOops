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

#ifndef OPTIONTESLACOIL_HPP_
#define OPTIONTESLACOIL_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionTeslaCoil : public OptionWidget
{
public:
	OptionTeslaCoil () : OptionTeslaCoil (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionTeslaCoil (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		driveLabel (10, 90, 60, 20, "ctlabel", BOOPS_LABEL_DRIVE),
		levelLabel (90, 90, 60, 20, "ctlabel", BOOPS_LABEL_LEVEL)
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", BOOPS_LABEL_DB, [] (double x) {return -30.0 + 100.0 * x;}, [] (double x) {return (LIMIT (x, -30.0, 70.0) + 30.0) / 100.0;});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", BOOPS_LABEL_DB, [] (double x) {return -70.0 + 100.0 * x;}, [] (double x) {return (LIMIT (x, -70.0, 30.0) + 70.0) / 100.0;});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (driveLabel);
		add (levelLabel);
		add (*options[0]);
		add (*options[1]);
		add (*options[2]);
		add (*options[3]);
	}

	OptionTeslaCoil (const OptionTeslaCoil& that) : OptionWidget (that), driveLabel (that.driveLabel)
	{
		add (driveLabel);
		add (levelLabel);
	}

	OptionTeslaCoil& operator= (const OptionTeslaCoil& that)
	{
		release (&driveLabel);
		OptionWidget::operator= (that);
		driveLabel = that.driveLabel;
		release (&levelLabel);
		OptionWidget::operator= (that);
		levelLabel = that.levelLabel;
		add (driveLabel);
		add (levelLabel);


		return *this;
	}

	virtual Widget* clone () const override {return new OptionTeslaCoil (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		driveLabel.applyTheme (theme);
		levelLabel.applyTheme (theme);
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
		for (int i = 0; i < 4; i += 2)
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

		// Send changed range to options[1]
		// Send changed range to options[i + 1]
		for (int i = 0; i < 4; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label driveLabel;
	BWidgets::Label levelLabel;
};

#endif /* OPTIONTESLACOIL_HPP_ */
