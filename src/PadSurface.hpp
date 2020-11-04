/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef PADSURFACE_HPP_
#define PADSURFACE_HPP_

#include "BWidgets/DrawingSurface.hpp"
#include "BWidgets/Focusable.hpp"
#include "BWidgets/Text.hpp"

class PadSurface : public BWidgets::DrawingSurface, public BWidgets::Focusable
{
public:
        BWidgets::Text focusText;

        PadSurface () : PadSurface (0, 0, 0, 0, "padsurface") {}
        PadSurface (const double x, const double y, const double width, const double height, const std::string& name) :
                DrawingSurface (x, y, width, height, name),
                Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
			std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
                focusText (0, 0, 120, 100, name + "/focus", "", true)
        {
                focusText.setStacking (BWidgets::STACKING_OVERSIZE);
                focusText.hide ();
                add (focusText);
        }

        void onFocusIn (BEvents::FocusEvent* event) override
        {
                Widget::onFocusIn (event);

        	if (event && event->getWidget())
        	{
        		BUtilities::Point pos = event->getPosition();
        		focusText.moveTo (pos.x - 0.5 * focusText.getWidth(), pos.y - focusText.getHeight());

                        // Resize
                        cairo_surface_t* surface = getDrawingSurface();
                	cairo_t* cr = cairo_create (surface);
                        focusText.resize (400,100);	// Maximize size first to omit breaks
                	std::vector<std::string> textblock = focusText.getTextBlock ();
                	double blockheight = focusText.getTextBlockHeight (textblock);
                	double blockwidth = 0.0;
                	for (std::string textline : textblock)
                	{
                		cairo_text_extents_t ext = focusText.getFont ()->getTextExtents (cr, textline);
                		if (ext.width > blockwidth) blockwidth = ext.width;
                	}
                	focusText.resize (blockwidth + 2 * focusText.getXOffset (), blockheight + 2 * focusText.getYOffset ());
                	focusText.resize();

        		focusText.show();
        	}
        }

        void onFocusOut (BEvents::FocusEvent* event) override
        {
                Widget::onFocusOut (event);
        	if (event && event->getWidget()) focusText.hide();
        }

        virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		focusText.applyTheme (theme, name + "/focus");
                focusText.resize ();
	}

	virtual void applyTheme (BStyles::Theme& theme) override
	{
		applyTheme (theme, name_);
	}

};

#endif /*PADSURFACE_HPP_*/
