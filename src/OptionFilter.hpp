/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
 *
 * This program is free software: you can reloribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is loributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OPTIONFILTER_HPP_
#define OPTIONFILTER_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionFilter : public OptionWidget
{
public:
	OptionFilter () : OptionFilter (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionFilter (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		loLabel (0, 90, 80, 20, "ctlabel", "High pass"),
		hiLabel (90, 90, 60, 20, "ctlabel", "Low pass"),
		ordLabel (170, 90, 60, 20, "ctlabel", "Roll off")
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 4.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 4.0);});
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new Dial (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "-db/o", [] (double x) {return 12 * int (LIMIT (1.0 + 8.0 * x, 1, 8));});
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[2]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[2])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[3]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[4]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);

		add (loLabel);
		add (hiLabel);
		add (ordLabel);
		for (int i = 0; i < 5; ++i) add (*options[i]);
	}

	OptionFilter (const OptionFilter& that) : OptionWidget (that), loLabel (that.loLabel), hiLabel (that.hiLabel), ordLabel (that.ordLabel)
	{
		add (loLabel);
		add (hiLabel);
		add (ordLabel);
	}

	OptionFilter& operator= (const OptionFilter& that)
	{
		release (&loLabel);
		release (&hiLabel);
		release (&ordLabel);
		OptionWidget::operator= (that);
		loLabel = that.loLabel;
		hiLabel = that.hiLabel;
		ordLabel = that.ordLabel;
		add (loLabel);
		add (hiLabel);
		add (ordLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionFilter (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		loLabel.applyTheme (theme);
		hiLabel.applyTheme (theme);
		ordLabel.applyTheme (theme);
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

		// options[1 or 3] changed ? Send to range
		if (widget == p->getWidget(1)) ((DialRange*)p->getWidget(0))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
		else if (widget == p->getWidget(3)) ((DialRange*)p->getWidget(2))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());

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
		if ((p == (DialRange*)pp->getWidget(0)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(1))->setValue (p->range.getValue ());
		}

		else if ((p == (DialRange*)pp->getWidget(2)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(3))->setValue (p->range.getValue ());
		}
	}

protected:
	BWidgets::Label loLabel;
	BWidgets::Label hiLabel;
	BWidgets::Label ordLabel;


};

#endif /* OPTIONFILTER_HPP_ */
