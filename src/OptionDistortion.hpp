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

#ifndef OPTIONDISTORTION_HPP_
#define OPTIONDISTORTION_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"
#include "BWidgets/ListBox.hpp"

class OptionDistortion : public OptionWidget
{
public:
	OptionDistortion () : OptionDistortion (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionDistortion (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		driveLabel (90, 90, 60, 20, "ctlabel", "Drive"),
		levelLabel (170, 90, 60, 20, "ctlabel", "Level")
	{
		try
		{
			options[0] = new BWidgets::ListBox
			(
				0, 20, 80, 90, "menu",
				BItems::ItemList ({{0.0, "Hardclip"}, {0.125, "Softclip"}, {0.25, "Foldback"}, {0.375, "Overdrive"}, {0.5, "Fuzz"}}),
				0.3
			);
			options[1] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "db", [] (double x) {return -30.0 + 100.0 * x;});
			options[2] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[3] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "db", [] (double x) {return -70.0 + 100.0 * x;});
			options[4] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		((BWidgets::ListBox*)options[0])->setTop (2);

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[1])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[3])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[4]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (driveLabel);
		add (levelLabel);
		add (*options[0]);
		add (*options[1]);
		add (*options[2]);
		add (*options[3]);
		add (*options[4]);
	}

	OptionDistortion (const OptionDistortion& that) : OptionWidget (that), driveLabel (that.driveLabel), levelLabel (that.levelLabel)
	{
		add (driveLabel);
		add (levelLabel);
	}

	OptionDistortion& operator= (const OptionDistortion& that)
	{
		release (&driveLabel);
		release (&levelLabel);
		OptionWidget::operator= (that);
		driveLabel = that.driveLabel;
		levelLabel = that.levelLabel;
		add (driveLabel);
		add (levelLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionDistortion (*this);}

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
		BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
		if (!ui) return;

		// options[1 or 3] changed ? Send to range
		if (widget == p->getWidget(2)) ((DialRange*)p->getWidget(1))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
		else if (widget == p->getWidget(4)) ((DialRange*)p->getWidget(3))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());

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

		// Send changed range to options[1 or 3]
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
	BWidgets::Label driveLabel;
	BWidgets::Label levelLabel;


};

#endif /* OPTIONDISTORTION_HPP_ */
