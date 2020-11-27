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
#include "ShapeWidget.hpp"

class OptionWah : public OptionWidget
{
public:
	OptionWah () : OptionWah (0.0, 0.0, 0.0, 0.0, "widget", "") {}
	OptionWah (const double x, const double y, const double width, const double height, const std::string& name, const std::string& pluginPath) :
		OptionWidget (x, y, width, height, name),
		cFreqLabel (490, 90, 60, 20, "ctlabel", "Center"),
		depthLabel (410, 90, 60, 20, "ctlabel", "Depth"),
		widthLabel (570, 90, 60, 20, "ctlabel", "Width"),
		orderLabel (650, 90, 60, 20, "ctlabel", "Roll off"),
		reachLabel (330, 90, 60, 20, "ctlabel", "Reach"),
		shapeWidget (5, 10, 310, 85, "pad0"),
		toolboxIcon (6, 100, 308, 20, "widget", pluginPath + "inc/shape_tb.png"),
		shapeToolButtons
		{
			HaloToggleButton (3.5, 100, 20, 20, "widget", "Select"),
			HaloToggleButton (26, 100, 20, 20, "widget", "Point node"),
			HaloToggleButton (48.5, 100, 20, 20, "widget", "Auto Bezier node"),
			HaloToggleButton (71, 100, 20, 20, "widget", "Symmetric Bezier node"),
			HaloToggleButton (93.5, 100, 20, 20, "widget", "Asymmetric Bezier node")
		},
		editToolButtons
		{
			HaloButton (123.5, 100, 20, 20, "widget", "Cut"),
			HaloButton (146, 100, 20, 20, "widget", "Copy"),
			HaloButton (168.5, 100, 20, 20, "widget", "Paste")
		},
		historyToolButtons
		{
			HaloButton (198.5, 100, 20, 20, "widget", "Reset"),
			HaloButton (221, 100, 20, 20, "widget", "Undo"),
			HaloButton (243.5, 100, 20, 20, "widget", "Redo")
		},
		gridShowButton (273.5, 100, 20, 20, "widget", "Show grid"),
		gridSnapButton (296, 100, 20, 20, "widget", "Snap to grid"),
		clipboard()
	{
		try
		{
			options[0] = new DialRange (490, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.0f", "Hz", [] (double x) {return 20.0 + 19980.0 * pow (x, 3.0);});
			options[1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[2] = new DialRange (410, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[3] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[4] = new DialRange (570, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.2f");
			options[5] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			options[6] = new Dial (650, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "-db/o", [] (double x) {return 12 * int (LIMIT (1.0 + 8.0 * x, 1, 8));});
			options[7] = new Dial (330, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, "%1.0f", "steps", [] (double x) {return 1 + LIMIT (32.0 * x, 0.0, 31.0);});
		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 6; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}
		options[6]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		options[7]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		shapeWidget.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, shapeChangedCallback);
		for (HaloToggleButton& s: shapeToolButtons) s.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, shapeToolClickedCallback);
		for (HaloButton& e: editToolButtons) e.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, editToolClickedCallback);
		for (HaloButton& h: historyToolButtons) h.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, historyToolClickedCallback);
		gridShowButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, gridToolClickedCallback);
		gridSnapButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, gridToolClickedCallback);

		shapeWidget.setMergeable (BEvents::POINTER_DRAG_EVENT, false);
		shapeWidget.setTool (ToolType::POINT_NODE_TOOL);
		shapeWidget.setDefaultShape ();
		shapeWidget.setScaleParameters (0.05, -1.0, 2.2);
		shapeWidget.setMajorXSteps (1.0);
		shapeWidget.setMinorXSteps (1.0/16.0);
		shapeWidget.setLowerLimit (-1.0);
		shapeWidget.setHigherLimit (1.0);
		shapeToolButtons[1].setValue (1.0);

		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);
		add (reachLabel);
		add (toolboxIcon);
		add (gridShowButton);
		add (gridSnapButton);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		for (int i = 0; i < 8; ++i) add (*options[i]);
		add (shapeWidget);
	}

	OptionWah (const OptionWah& that) :
		OptionWidget (that),
		cFreqLabel (that.cFreqLabel), depthLabel (that.depthLabel),
		widthLabel (that.widthLabel), orderLabel (that.orderLabel), reachLabel (that.reachLabel),
		shapeWidget (that.shapeWidget), toolboxIcon (that.toolboxIcon),
		shapeToolButtons (that.shapeToolButtons), editToolButtons (that.editToolButtons), historyToolButtons (that.historyToolButtons),
		gridShowButton (that.gridShowButton), gridSnapButton (that.gridSnapButton)
	{
		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);
		add (reachLabel);
		add (toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		add (gridShowButton);
		add (gridSnapButton);
		add (shapeWidget);
	}

	OptionWah& operator= (const OptionWah& that)
	{
		release (&cFreqLabel);
		release (&depthLabel);
		release (&orderLabel);
		release (&widthLabel);
		release (&reachLabel);
		release (&shapeWidget);
		release (&toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) release (&s);
		for (HaloButton& e : editToolButtons) release (&e);
		for (HaloButton& h : historyToolButtons) release (&h);
		release (&gridShowButton);
		release (&gridSnapButton);

		OptionWidget::operator= (that);
		cFreqLabel = that.cFreqLabel;
		depthLabel = that.depthLabel;
		orderLabel = that.orderLabel;
		widthLabel = that.widthLabel;
		reachLabel = that.reachLabel;
		shapeWidget = that.shapeWidget;
		toolboxIcon = that.toolboxIcon;
		shapeToolButtons = that.shapeToolButtons;
		editToolButtons = that.editToolButtons;
		historyToolButtons = that.historyToolButtons;
		gridShowButton = that.gridShowButton;
		gridSnapButton = that.gridSnapButton;

		add (cFreqLabel);
		add (depthLabel);
		add (orderLabel);
		add (widthLabel);
		add (reachLabel);
		add (toolboxIcon);
		for (HaloToggleButton& s : shapeToolButtons) add (s);
		for (HaloButton& e : editToolButtons) add (e);
		for (HaloButton& h : historyToolButtons) add (h);
		add (gridShowButton);
		add (gridSnapButton);
		add (shapeWidget);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionWah (*this);}

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
		cFreqLabel.applyTheme (theme);
		depthLabel.applyTheme (theme);
		widthLabel.applyTheme (theme);
		orderLabel.applyTheme (theme);
		reachLabel.applyTheme (theme);
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
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
		if (!ui) return;

		// options[i + 1] changed ? Send to range
		for (int i = 0; i < 6; i += 2)
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
		for (int i = 0; i < 6; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

	static void shapeChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		ShapeWidget* p = (ShapeWidget*) widget->getParent();
		if (!p) return;
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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
		OptionWah* p = (OptionWah*) widget->getParent();
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
		OptionWah* p = (OptionWah*) widget->getParent();
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
		OptionWah* p = (OptionWah*) widget->getParent();
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
		OptionWah* p = (OptionWah*) widget->getParent();
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
	BWidgets::Label cFreqLabel;
	BWidgets::Label depthLabel;
	BWidgets::Label widthLabel;
	BWidgets::Label orderLabel;
	BWidgets::Label reachLabel;
	ShapeWidget shapeWidget;
	BWidgets::ImageIcon toolboxIcon;
        std::array<HaloToggleButton, 5> shapeToolButtons;
        std::array<HaloButton, 3> editToolButtons;
        std::array<HaloButton, 3> historyToolButtons;
        HaloToggleButton gridShowButton;
        HaloToggleButton gridSnapButton;
	std::vector<Node> clipboard;
};

#endif /* OPTIONWAH_HPP_ */
