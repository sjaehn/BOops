/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SELECTWIDGET_HPP_
#define SELECTWIDGET_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "BWidgets/Focusable.hpp"
#include "BWidgets/Label.hpp"

class SelectWidget : public BWidgets::RangeWidget, public BWidgets::Focusable
{
public:
	SelectWidget ();
	SelectWidget (const double x, const double y, const double width, const double height, const std::string& name,
		      const double selectionWidth, const double selectionHeight, const double nrTools, const double value,
	      	      std::vector<std::string> labeltexts = {});
	SelectWidget (const SelectWidget& that);

	SelectWidget& operator= (const SelectWidget& that);

	void resizeSelection (const double width, const double height);
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;
	virtual void onFocusIn (BEvents::FocusEvent* event) override;
	virtual void onFocusOut (BEvents::FocusEvent* event) override;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	BColors::ColorSet bgColors;
	int nrTools;
	double selectionWidth;
	double selectionHeight;
	std::vector<std::string> labelTexts_;
	BWidgets::Label focusLabel_;
};

#endif /* SELECTWIDGET_HPP_ */
