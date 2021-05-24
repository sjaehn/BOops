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

#ifndef OPTIONINFINITY_HPP_
#define OPTIONINFINITY_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"

class OptionInfinity : public OptionWidget
{
public:
	OptionInfinity () : OptionInfinity (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionInfinity (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		filterLabel (0, 90, 80, 20, "ctlabel", BOOPS_LABEL_FILTER),
		sizeLabel (80, 90, 80, 20, "ctlabel", BOOPS_LABEL_SIZE),
		dampLabel (160, 90, 80, 20, "ctlabel", BOOPS_LABEL_DAMP),
		allpassLabel (240, 90, 80, 20, "ctlabel", BOOPS_LABEL_ALLPASS),
		feedbackLabel (320, 90, 80, 20, "ctlabel", BOOPS_LABEL_FEEDBACK)
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "");
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (90, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "");
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (170, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "");
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new DialRange (250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "");
			options[7] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[8] = new DialRange (330, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "");
			options[9] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 5; ++i)
		{
			options[2 * i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[2 * i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[2 * i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (filterLabel);
		add (sizeLabel);
		add (dampLabel);
		add (allpassLabel);
		add (feedbackLabel);
		for (int i = 0; i < 10 ; ++i)add (*options[i]);
	}

	OptionInfinity (const OptionInfinity& that) :
		OptionWidget (that),
		filterLabel (that.filterLabel),
		sizeLabel (that.sizeLabel),
		dampLabel (that.dampLabel),
		allpassLabel (that.allpassLabel),
		feedbackLabel (that.feedbackLabel)
	{
		add (filterLabel);
		add (sizeLabel);
		add (dampLabel);
		add (allpassLabel);
		add (feedbackLabel);
	}

	OptionInfinity& operator= (const OptionInfinity& that)
	{
		release (&filterLabel);
		release (&sizeLabel);
		release (&dampLabel);
		release (&allpassLabel);
		release (&feedbackLabel);
		OptionWidget::operator= (that);
		filterLabel = that.filterLabel;
		sizeLabel = that.sizeLabel;
		dampLabel = that.dampLabel;
		allpassLabel = that.allpassLabel;
		feedbackLabel = that.feedbackLabel;
		add (filterLabel);
		add (sizeLabel);
		add (dampLabel);
		add (allpassLabel);
		add (feedbackLabel);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionInfinity (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		filterLabel.applyTheme (theme);
		sizeLabel.applyTheme (theme);
		dampLabel.applyTheme (theme);
		allpassLabel.applyTheme (theme);
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

		// options[1] changed ? Send to range
		for (int i = 0; i < 5; ++i)
		{
			if (widget == p->getWidget(2 * i + 1))
			{
				((DialRange*)p->getWidget(2 * i))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
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
		for (int i = 0; i < 5; ++i)
		{
			if ((p == (DialRange*)pp->getWidget(2 * i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(2 * i + 1))->setValue (p->range.getValue ());
				break;
			}
		}
	}

protected:
	BWidgets::Label filterLabel;
	BWidgets::Label sizeLabel;
	BWidgets::Label dampLabel;
	BWidgets::Label allpassLabel;
	BWidgets::Label feedbackLabel;
};

#endif /* OPTIONINFINITY_HPP_ */
