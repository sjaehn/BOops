/* Knob.hpp
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

#ifndef BWIDGETS_KNOB_HPP_
#define BWIDGETS_KNOB_HPP_

#include "RangeWidget.hpp"

#define BWIDGETS_DEFAULT_KNOB_WIDTH 20.0
#define BWIDGETS_DEFAULT_KNOB_HEIGHT 20.0
#define BWIDGETS_DEFAULT_KNOB_DEPTH 1.0

namespace BWidgets
{
/**
 * Class BWidgets::Knob
 *
 * Drawing widget. Draws a 3d knob.
 */
class Knob : public Widget
{
public:
	Knob ();
	Knob (const double x, const double y, const double width, const double height, const double depth, const std::string& name);

	/**
	 * Creates a new (orphan) knob and copies the knob properties from a
	 * source knob. This method doesn't copy any parent or child widgets.
	 * @param that Source knob
	 */
	Knob (const Knob& that);

	~Knob ();

	/**
	 * Assignment. Copies the knob properties from a source knob and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible.
	 * @param that Source knob
	 */
	Knob& operator= (const Knob& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Sets the depth of the 3d knob
	 * @param depth Depth
	 */
	void setDepth (const double depth);

	/**
	 * Sets the depth of the 3d knob
	 * @return Depth
	 */
	double getDepth () const;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	double knobDepth;
	BColors::ColorSet bgColors;
};

}

#endif /* BWIDGETS_KNOB_HPP_ */
