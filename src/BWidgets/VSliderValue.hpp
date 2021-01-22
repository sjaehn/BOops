/* VSliderValue.hpp
 * Copyright (C) 2018, 2019  Sven Jähnichen
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

#ifndef BWIDGETS_VSLIDERVALUE_HPP_
#define BWIDGETS_VSLIDERVALUE_HPP_

#include "Label.hpp"
#include "VSlider.hpp"

#define BWIDGETS_DEFAULT_VSLIDERVALUE_WIDTH 24.0
#define BWIDGETS_DEFAULT_VSLIDERVALUE_HEIGHT BWIDGETS_DEFAULT_VSLIDER_HEIGHT

namespace BWidgets
{

/**
 * Class BWidgets::VSliderValue
 *
 * Composite BWidgets::VSlider widget that additionally displays the value.
 */
class VSliderValue : public VSlider
{
public:
	VSliderValue ();
	VSliderValue (const double x, const double y, const double width, const double height, const std::string& name,
		      const double value, const double min, const double max, const double step,
		      const std::string& valueFormat, LabelPosition valuePos = LABEL_TOP);

	/**
	 * Creates a new (orphan) slider and copies the slider properties from a
	 * source slider.
	 * @param that Source slider
	 */
	VSliderValue (const VSliderValue& that);

	/**
	 * Assignment. Copies the slider properties from a source slider and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible and a value changed event.
	 * @param that Source slider
	 */
	VSliderValue& operator= (const VSliderValue& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Changes the value of the widget and keeps it within the defined range.
	 * Passes the value to its predefined child widgets.
	 * Emits a value changed event and (if visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val) override;

	/**
	 * Sets the position of the value display label.
	 * @param pos	Position of the value display label.
	 */
	virtual void setValuePosition (const LabelPosition pos);

	/**
	 * Gets the position of the value display label.
	 * @return	Position of the value display label.
	 */
	LabelPosition getValuePosition () const;

	/**
	 * Sets the value output format.
	 * @param valueFormat Format of the output in printf standard for type double.
	 */
	void setValueFormat (const std::string& valueFormat);

	/**
	 * Gets the value output format.
	 * @return Format of the output in printf standard for type double.
	 */
	std::string getValueFormat () const;

	/**
	 * Gets (a pointer to) the Label for direct access.
	 * @return Pointer to the label
	 */
	Label* getDisplayLabel ();

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				Styles used are:
	 * 				BWIDGETS_KEYWORD_BORDER
	 * 				BWIDGETS_KEYWORD_BACKGROUND
	 * 				BWIDGETS_KEYWORD_FGCOLORS
	 * 				BWIDGETS_KEYWORD_BGCOLORS
	 * 				BWIDGETS_KEYWORD_TEXTCOLORS
	 * 				BWIDGETS_KEYWORD_FONT.
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

protected:
	virtual void updateCoords () override;
	static void displayDraggedCallback (BEvents::Event* event);
	static void displayMessageCallback (BEvents::Event* event);

	Label valueDisplay;

	LabelPosition valPosition;
	std::string valFormat;
	BUtilities::RectArea displayArea;
};

}

#endif /* BWIDGETS_VSLIDERVALUE_HPP_ */
