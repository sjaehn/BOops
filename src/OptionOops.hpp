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

#ifndef OPTIONOOPS_HPP_
#define OPTIONOOPS_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionOops : public OptionWidget
{
public:
	OptionOops () : OptionOops (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionOops (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		ampLabel (10, 90, 60, 20, "ctlabel", BOOPS_LABEL_AMP),
		pitchLabel (90, 90, 60, 20, "ctlabel", BOOPS_LABEL_PITCH),
		offsetLabel (170, 90, 60, 20, "ctlabel", BOOPS_LABEL_OFFSET)
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 2.0 * x;}, [] (double x) {return 0.5 * LIMIT (x, 0.0, 2.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", BOOPS_LABEL_SEMI, [] (double x) {return 24.0 * x - 12.0;}, [] (double x) {return (LIMIT (x, -12.0, 12.0) + 12.0) / 24.0;});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 3; ++i)
		{
			options[i * 2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i * 2])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i * 2 + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}


		add (ampLabel);
		add (pitchLabel);
		add (offsetLabel);
		for (int i = 0; i < 6; ++i) add (*options[i]);
	}

	OptionOops (const OptionOops& that) : OptionWidget (that), ampLabel (that.ampLabel), pitchLabel (that.pitchLabel), offsetLabel (that.offsetLabel)
	{
		add (ampLabel);
		add (pitchLabel);
		add (offsetLabel);
	}

	OptionOops& operator= (const OptionOops& that)
	{
		release (&ampLabel);
		release (&pitchLabel);
		release (&offsetLabel);
		OptionWidget::operator= (that);
		ampLabel = that.ampLabel;
		pitchLabel = that.pitchLabel;
		offsetLabel = that.offsetLabel;
		add (ampLabel);
		add (pitchLabel);
		add (offsetLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionOops (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		ampLabel.applyTheme (theme);
		pitchLabel.applyTheme (theme);
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

		// options[1 or 3 or 5] changed ? Send to range
		for (int i = 0; i < 3; ++i)
		{
			if (widget == p->getWidget(i * 2 + 1)) ((DialRange*)p->getWidget(i * 2))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
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

		// Send changed range to options[1 or 3 or 5]
		for (int i = 0; i < 3; ++i)
		{
			if ((p == (DialRange*)pp->getWidget(i * 2)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i * 2 + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label ampLabel;
	BWidgets::Label pitchLabel;
	BWidgets::Label offsetLabel;


};

#endif /* OPTIONOOPS_HPP_ */
