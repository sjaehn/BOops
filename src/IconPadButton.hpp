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

#ifndef ICONPADBUTTON_HPP_
#define ICONPADBUTTON_HPP_

#include "PadButton.hpp"
#include "BWidgets/ImageIcon.hpp"

class IconPadButton : public PadButton
{
public:
	IconPadButton () : IconPadButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "padbutton", "", "", 0.0) {}
	IconPadButton (const double x, const double y, const double width, const double height, const std::string& name,
		const std::string buttonfilename, const std::string iconfilename, double defaultValue = 0.0) :
		PadButton (x, y, width, height, name, NOSYMBOL, defaultValue),
		button (0, 0, height, height, name + "/icon", buttonfilename),
		icon (height, 0, width - height, height, name + "/icon", iconfilename)
	{
		add (button);
		icon.setClickable (false);
		add (icon);
	}

	IconPadButton (const IconPadButton& that) : PadButton (that), button (that.button), icon (that.icon)
	{
		add (button);
		add (icon);
	}

	IconPadButton& operator= (const IconPadButton& that)
	{
		release (&icon);
		release (&button);

		PadButton::operator= (that);
		button = that.button;
		icon = that.icon;

		add (button);
		add (icon);

		return *this;
	}

	virtual Widget* clone () const override {return new IconPadButton (*this);}

	virtual void setWidth (const double width) override
	{
		PadButton::setWidth (width);
		button.moveTo (0, 0);
		button.setWidth (getHeight());
		icon.moveTo (getHeight(), 0);
		icon.setWidth (getWidth() - getHeight());
	}

	virtual void setHeight (const double height) override
	{
		PadButton::setHeight (height);
		button.moveTo (0, 0);
		button.setWidth (getHeight());
		icon.moveTo (getHeight(), 0);
		icon.setWidth (getWidth() - getHeight());
	}

	virtual void resize () override
	{
		icon.resize ();
		icon.moveTo (icon.getHeight(), 0);
		button.moveTo (0, 0);
		button.resize (icon.getHeight(), icon.getHeight());
		PadButton::resize ();
	}

	virtual void resize (const double width, const double height) override {resize (BUtilities::Point (width, height));}

	virtual void resize (const BUtilities::Point extends) override
	{
		PadButton::resize (BUtilities::Point (extends.x, extends.y));
		button.moveTo (0, 0);
		button.resize (getHeight(), getHeight());
		icon.moveTo (getHeight(), 0);
		icon.resize (getWidth() - getHeight(), getHeight());
	}

	void loadImage (BColors::State state, const std::string& filename)
	{
		icon.loadImage (state, filename);
		icon.update();
	}

	BWidgets::ImageIcon button;
	BWidgets::ImageIcon icon;
};

#endif /* ICONPADBUTTON_HPP_ */
