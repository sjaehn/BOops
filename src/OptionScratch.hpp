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

#ifndef OPTIONSCRATCH_HPP_
#define OPTIONSCRATCH_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"
#include "ShapeWidget.hpp"

class OptionScratch : public OptionWidget
{
public:
	OptionScratch () : OptionScratch (0.0, 0.0, 0.0, 0.0, "widget", "") {}
	OptionScratch (const double x, const double y, const double width, const double height, const std::string& name, const std::string& pluginPath) :
		OptionWidget (x, y, width, height, name),
		rangeLabel (10, 90, 60, 20, "ctlabel", "Range"),
		shapeWidget (85, 10, 310, 85, "pad0"),
		toolboxIcon (86, 100, 308, 20, "widget", pluginPath + "inc/shape_tb.png"),
		shapeToolButtons
		{
			HaloToggleButton (83.5, 100, 20, 20, "widget", "Select"),
			HaloToggleButton (106, 100, 20, 20, "widget", "Point node"),
			HaloToggleButton (128.5, 100, 20, 20, "widget", "Auto Bezier node"),
			HaloToggleButton (151, 100, 20, 20, "widget", "Symmetric Bezier node"),
			HaloToggleButton (173.5, 100, 20, 20, "widget", "Asymmetric Bezier node")
		},
		editToolButtons
		{
			HaloButton (203.5, 100, 20, 20, "widget", "Cut"),
			HaloButton (226, 100, 20, 20, "widget", "Copy"),
			HaloButton (248.5, 100, 20, 20, "widget", "Paste")
		},
		historyToolButtons
		{
			HaloButton (278.5, 100, 20, 20, "widget", "Reset"),
			HaloButton (301, 100, 20, 20, "widget", "Undo"),
			HaloButton (323.5, 100, 20, 20, "widget", "Redo")
		},
		gridShowButton (353.5, 100, 20, 20, "widget", "Show grid"),
		gridSnapButton (376, 100, 20, 20, "widget", "Snap to grid"),
		clipboard()
	{
		try
		{
			options[0] = new DialRange (10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f", "", [] (double x) {return 2 * x;});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
		}
		catch (std::bad_alloc& ba) {throw ba;}

		options[0]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		((DialRange*)options[0])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
		options[1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		shapeWidget.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, shapeChangedCallback);
		for (HaloToggleButton& s: shapeToolButtons) s.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, shapeToolClickedCallback);
		for (HaloButton& e: editToolButtons) e.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, editToolClickedCallback);
		for (HaloButton& h: historyToolButtons) h.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, historyToolClickedCallback);
		gridShowButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, gridToolClickedCallback);
		gridSnapButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, gridToolClickedCallback);

		shapeWidget.setMergeable (BEvents::POINTER_DRAG_EVENT, false);
		shapeWidget.setTool (ToolType::POINT_NODE_TOOL);
		shapeWidget.setDefaultShape ();
		shapeWidget.setScaleParameters (0.05, -1.0, 1.1);
		shapeWidget.setMajorXSteps (1.0);
		shapeWidget.setMinorXSteps (1.0/16.0);
		shapeWidget.setLowerLimit (-1.0);
		shapeWidget.setHigherLimit (0.0);
		shapeToolButtons[1].setValue (1.0);

		add (rangeLabel);
		add (shapeWidget);
		add (toolboxIcon);
		add (gridShowButton);
		add (gridSnapButton);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		add (*options[0]);
		add (*options[1]);
	}

	OptionScratch (const OptionScratch& that) :
		OptionWidget (that), rangeLabel (that.rangeLabel), shapeWidget (that.shapeWidget), toolboxIcon (that.toolboxIcon),
		shapeToolButtons (that.shapeToolButtons), editToolButtons (that.editToolButtons), historyToolButtons (that.historyToolButtons),
		gridShowButton (that.gridShowButton), gridSnapButton (that.gridSnapButton)
	{
		add (rangeLabel);
		add (shapeWidget);
		add (toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		add (gridShowButton);
		add (gridSnapButton);
	}

	OptionScratch& operator= (const OptionScratch& that)
	{
		release (&rangeLabel);
		release (&shapeWidget);
		release (&toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) release (&s);
		for (HaloButton& e : editToolButtons) release (&e);
		for (HaloButton& h : historyToolButtons) release (&h);
		release (&gridShowButton);
		release (&gridSnapButton);

		OptionWidget::operator= (that);
		rangeLabel = that.rangeLabel;
		shapeWidget = that.shapeWidget;
		toolboxIcon = that.toolboxIcon;
		shapeToolButtons = that.shapeToolButtons;
		editToolButtons = that.editToolButtons;
		historyToolButtons = that.historyToolButtons;
		gridShowButton = that.gridShowButton;
		gridSnapButton = that.gridSnapButton;

		add (rangeLabel);
		add (shapeWidget);
		add (toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		add (gridShowButton);
		add (gridSnapButton);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionScratch (*this);}

	virtual void setShape (const Shape<SHAPE_MAXNODES>& shape) override
	{
		shapeWidget.setValueEnabled (false);
		shapeWidget.clearShape();
		for (unsigned int i = 0; i < shape.size(); ++i) shapeWidget.appendNode (shape.getNode(i));
		shapeWidget.validateShape();
		shapeWidget.pushToSnapshots ();
		shapeWidget.update ();
		shapeWidget.setValueEnabled (true);
	}

	virtual Shape<SHAPE_MAXNODES> getShape() const override {return Shape<SHAPE_MAXNODES>(shapeWidget);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		rangeLabel.applyTheme (theme);
		shapeWidget.applyTheme (theme);
		toolboxIcon.applyTheme (theme);
		for (HaloToggleButton& s : shapeToolButtons) s.applyTheme (theme);
		for (HaloButton& e : editToolButtons) e.applyTheme (theme);
		for (HaloButton& h : historyToolButtons) h.applyTheme (theme);
		gridShowButton.applyTheme (theme);
		gridSnapButton.applyTheme (theme);
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

		// options[1] changed ? Send to range
		if (widget == p->getWidget(1)) ((DialRange*)p->getWidget(0))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());

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
		if ((p == (DialRange*)pp->getWidget(0)) && (widget == (BWidgets::Widget*)&p->range))
		{
			p->update();
			((BWidgets::ValueWidget*)pp->getWidget(1))->setValue (p->range.getValue ());
		}
	}

	static void shapeChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		ShapeWidget* p = (ShapeWidget*) widget->getParent();
		if (!p) return;
		BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
		if (!ui) return;

		// Forward changed shape to ui
		ui->optionChangedCallback (event);
	}

	static void shapeToolClickedCallback(BEvents::Event* event)
	{
		if (!event) return;
		HaloToggleButton* widget = (HaloToggleButton*) event->getWidget ();
		if (!widget) return;
		float value = widget->getValue();
		OptionScratch* p = (OptionScratch*) widget->getParent();
		if (!p) return;

		// Identify tool
		int widgetNr = 0;
		if (value)
		{
			for (int i = 1; i < NR_TOOLS; ++i)
			{
				if (widget == &p->shapeToolButtons[i - 1])
				{
					widgetNr = i;
					break;
				}
			}
		}

		p->shapeWidget.setTool (ToolType (widgetNr));

		// Allow only one button pressed
		for (HaloToggleButton& s : p->shapeToolButtons)
		{
			if (&s != widget) s.setValue (0.0);
		}
	}

	static void editToolClickedCallback(BEvents::Event* event)
	{
		if (!event) return;
		HaloButton* widget = (HaloButton*) event->getWidget ();
		if (!widget) return;
		float value = widget->getValue();
		if (value != 1.0) return;
		OptionScratch* p = (OptionScratch*) widget->getParent();
		if (!p) return;

		// Identify editButtons
		int widgetNr = -1;
		for (size_t i = 0; i < p->editToolButtons.size(); ++i)
		{
			if (widget == &p->editToolButtons[i])
			{
				widgetNr = i;
				break;
			}
		}

		// Action
		switch (widgetNr)
		{
			case 0:		p->clipboard = p->shapeWidget.cutSelection();
					break;

			case 1:		p->clipboard = p->shapeWidget.copySelection();
					break;

			case 2:		p->shapeWidget.pasteSelection (p->clipboard);
					break;

			default:	break;
		}
	}

	static void historyToolClickedCallback(BEvents::Event* event)
	{
		if (!event) return;
		HaloButton* widget = (HaloButton*) event->getWidget ();
		if (!widget) return;
		float value = widget->getValue();
		if (value != 1.0) return;
		OptionScratch* p = (OptionScratch*) widget->getParent();
		if (!p) return;

		// Identify editButtons
		int widgetNr = -1;
		for (size_t i = 0; i < p->historyToolButtons.size(); ++i)
		{
			if (widget == &p->historyToolButtons[i])
			{
				widgetNr = i;
				break;
			}
		}

		// Action
		switch (widgetNr)
		{
			case 0:		p->shapeWidget.reset();
					break;

			case 1:		p->shapeWidget.undo();
					break;

			case 2:		p->shapeWidget.redo();
					break;

			default:	break;
		}
	}

	static void gridToolClickedCallback (BEvents::Event* event)
	{
		if (!event) return;
		HaloToggleButton* widget = (HaloToggleButton*) event->getWidget ();
		if (!widget) return;
		float value = widget->getValue();
		OptionScratch* p = (OptionScratch*) widget->getParent();
		if (!p) return;

		if (widget == &p->gridShowButton)
		{
			if (value) p->shapeWidget.showGrid();
			else p->shapeWidget.hideGrid();
			p->shapeWidget.setSnap (false);
			p->gridSnapButton.setValue (0.0);
		}

		else if (widget == &p->gridSnapButton)
		{
			if (value)
			{
				p->shapeWidget.showGrid();
				p->shapeWidget.setSnap (true);
			}
			else
			{
				p->shapeWidget.hideGrid();
				p->shapeWidget.setSnap (false);
			}
			p->gridShowButton.setValue (0.0);
		}
	}

protected:
	BWidgets::Label rangeLabel;
	ShapeWidget shapeWidget;
	BWidgets::ImageIcon toolboxIcon;
        std::array<HaloToggleButton, 5> shapeToolButtons;
        std::array<HaloButton, 3> editToolButtons;
        std::array<HaloButton, 3> historyToolButtons;
        HaloToggleButton gridShowButton;
        HaloToggleButton gridSnapButton;
	std::vector<Node> clipboard;
};

#endif /* OPTIONSCRATCH_HPP_ */
