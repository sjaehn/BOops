/* HPianoRoll.cpp
 * Copyright (C) 2019  Sven Jähnichen
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

#include "HPianoRoll.hpp"

namespace BWidgets
{

HPianoRoll::HPianoRoll () : HPianoRoll (0, 0, 0, 0, "piano", 0, 0) {}

HPianoRoll::HPianoRoll (const double x, const double y, const double width, const double height, const std::string& name) :
		HPianoRoll (x, y, width, height, name, 0, 119) {}

HPianoRoll::HPianoRoll (const double x, const double y, const double width, const double height, const std::string& name,
						const int startMidiKey, const int endMidiKey) :
		PianoWidget (x, y, width, height, name, startMidiKey, endMidiKey),
		toggleKeys (false), actKeyNr (-1),
		blackBgColors ({{0.05, 0.05, 0.05, 1.0}, {0.4, 0.1, 0.1, 1.0}, {0.25, 0.25, 0.25, 1.0}, {0.0, 0.0, 0.0, 1.0}}),
		whiteBgColors ({{0.9, 0.9, 0.9, 1.0}, {1.0, 0.6, 0.6, 1.0}, {0.5, 0.5, 0.5, 1.0}, {0.0, 0.0, 0.0, 1.0}})
{
	setDraggable (true);
	cbfunction_[BEvents::EventType::POINTER_DRAG_EVENT] = Widget::defaultCallback;
}

Widget* HPianoRoll::clone () const {return new HPianoRoll (*this);}

void HPianoRoll::setKeysToggleable (const bool toggle) {toggleKeys = toggle;}

bool HPianoRoll::isKeysToggleable () const {return toggleKeys;}

//void HPianoRoll::applyTheme (BStyles::Theme& theme, const std::string& name) override;

void HPianoRoll::onButtonPressed (BEvents::PointerEvent* event)
{
	if (event)
	{
		int newKeyNr = getKey (event->getPosition());
		int relNewKeyNr = newKeyNr - startMidiKey;
		if ((newKeyNr >= startMidiKey) && (newKeyNr <= endMidiKey) && (activeKeys[relNewKeyNr]))
		{
			if (toggleKeys)
			{
				if (actKeyNr != newKeyNr)
				{
					pressedKeys[relNewKeyNr] = !pressedKeys[relNewKeyNr];
					if (pressedKeys[relNewKeyNr]) postMessage (BWIDGETS_PIANO_KEY_PRESSED_MESSAGE, BUtilities::makeAny<int> (relNewKeyNr));
					else postMessage (BWIDGETS_PIANO_KEY_RELEASED_MESSAGE, BUtilities::makeAny<int> (relNewKeyNr));
					update();
				}
			}
			else
			{
				if (actKeyNr != newKeyNr)
				{
					if ((actKeyNr >= startMidiKey) && (actKeyNr <= endMidiKey))
					{
						pressedKeys[actKeyNr - startMidiKey] = false;
						postMessage (BWIDGETS_PIANO_KEY_RELEASED_MESSAGE, BUtilities::makeAny<int> (actKeyNr - startMidiKey));
					}

					pressedKeys[relNewKeyNr] = true;
					postMessage (BWIDGETS_PIANO_KEY_PRESSED_MESSAGE, BUtilities::makeAny<int> (relNewKeyNr));
					update();
				}
			}

			actKeyNr = newKeyNr;
		}

		if (event->getEventType() == BEvents::BUTTON_PRESS_EVENT) Widget::onButtonPressed (event);
		else if (event->getEventType() == BEvents::POINTER_DRAG_EVENT) Widget::onPointerDragged (event);
	}
}

void HPianoRoll::onButtonReleased (BEvents::PointerEvent* event)
{
	if ((event) && (actKeyNr >= startMidiKey) && (actKeyNr <= endMidiKey))
	{
		if  ((!toggleKeys) && (pressedKeys[actKeyNr - startMidiKey]))
		{
			pressedKeys[actKeyNr - startMidiKey] = false;
			postMessage (BWIDGETS_PIANO_KEY_RELEASED_MESSAGE, BUtilities::makeAny<int> (actKeyNr - startMidiKey));
			update ();
		}
		actKeyNr = -1;
	}

	Widget::onButtonReleased (event);
}

void HPianoRoll::onPointerDragged (BEvents::PointerEvent* event)
{
	onButtonPressed (event);
}


// TODO startMidiKey shift
int HPianoRoll::getKey (const BUtilities::Point position)
{
	double x0 = getXOffset();
	double y0 = getYOffset();
	double w = getEffectiveWidth();
	double h = getEffectiveHeight();

	if ((w >= 2) && (h >= 2) && (endMidiKey >= startMidiKey))
	{
		int startKeyNrOffset = startMidiKey % 12;
		int endKeyNrOffset = endMidiKey % 12;
		double startKeyX = keyCoords[startKeyNrOffset].x + ((int)(startMidiKey / 12)) * 7;
		double endKeyX = keyCoords[endKeyNrOffset].x + keyCoords[endKeyNrOffset].width + ((int)(endMidiKey / 12)) * 7;
		double xs = w / (endKeyX - startKeyX);

		double keyX = position.x / xs - x0;
		int keyXOctave = keyX / 7;
		double keyXOffset = keyX - keyXOctave * 7;

		// The blacks first
		for (int i = 0; i < 12; ++i)
		{
			if (!keyCoords[i].whiteKey)
			{
				if
				(
					(position.y >= y0) &&
					(position.y <= y0 + 0.6667 * h) &&
					(keyXOffset >= keyCoords[i].x) &&
					(keyXOffset <= keyCoords[i].x + keyCoords[i].width)
				)
				{
					return keyXOctave * 12 + i + startMidiKey;
				}
			}
		}

		// Then the whites
		for (int i = 0; i < 12; ++i)
		{
			if (keyCoords[i].whiteKey)
			{
				if
				(
					(position.y >= y0) &&
					(position.y <= y0 + h) &&
					(keyXOffset >= keyCoords[i].x)
					&& (keyXOffset <= keyCoords[i].x + keyCoords[i].width)
				)
				{
					return keyXOctave * 12 + i + startMidiKey;
				}
			}
		}
	}

	return -1;
}

void HPianoRoll::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	double x0 = getXOffset();
	double y0 = getYOffset();
	double w = getEffectiveWidth();
	double h = getEffectiveHeight();

	if ((w >= 2) && (h >= 2) && (endMidiKey >= startMidiKey))
	{
		// Draw widget class elements first
		Widget::draw (area);

		cairo_t* cr = cairo_create (widgetSurface_);
		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
			cairo_clip (cr);

			cairo_set_line_width (cr, 0.0);

			int startKeyNrOffset = startMidiKey % 12;
			int endKeyNrOffset = endMidiKey % 12;
			double startKeyX = keyCoords[startKeyNrOffset].x + ((int)(startMidiKey / 12)) * 7;
			double endKeyX = keyCoords[endKeyNrOffset].x + keyCoords[endKeyNrOffset].width + ((int)(endMidiKey / 12)) * 7;
			double xs = w / (endKeyX - startKeyX);

			BColors::Color color;

			for (int i = 0; i <= endMidiKey - startMidiKey; ++i)
			{
				int keyNrOffset = (i + startMidiKey) % 12;
				double keyX = keyCoords[keyNrOffset].x + ((int)((startMidiKey + i) / 12)) * 7 - startKeyX;

				if (keyCoords[keyNrOffset].whiteKey)
				{
					color = *(whiteBgColors.getColor (!activeKeys[i] ? BColors::INACTIVE : (pressedKeys[i] ? BColors::ACTIVE : BColors::NORMAL)));
					cairo_set_source_rgba (cr, CAIRO_RGBA (color));
					cairo_move_to (cr, x0 + (keyX + keyCoords[keyNrOffset].dx1  + 0.025) * xs, y0);
					cairo_line_to (cr, x0 + (keyX + keyCoords[keyNrOffset].dx1  + 0.025) * xs, y0 + 0.667 * h);
					cairo_line_to (cr, x0 + (keyX + 0.025) * xs, y0 + 0.667 * h);
					cairo_line_to (cr, x0 + (keyX + 0.025) * xs, y0 + h - 0.05 * xs);
					cairo_arc_negative (cr, x0 + (keyX + 0.125) * xs, y0 + h - 0.1 * xs, 0.1 * xs, M_PI, M_PI / 2);
					cairo_line_to (cr, x0 + (keyX + keyCoords[keyNrOffset].width - 0.125) * xs, y0 + h);
					cairo_arc_negative (cr, x0 + (keyX + keyCoords[keyNrOffset].width - 0.125) * xs, y0 + h - 0.1 * xs, 0.1 * xs, M_PI / 2, 0);
					cairo_line_to (cr, x0 + (keyX + keyCoords[keyNrOffset].width - 0.025) * xs, y0 + 0.667 * h);
					cairo_line_to (cr, x0 + (keyX + keyCoords[keyNrOffset].width - keyCoords[keyNrOffset].dx2 - 0.025) * xs, y0 + 0.667 * h);
					cairo_line_to (cr, x0 + (keyX + keyCoords[keyNrOffset].width - keyCoords[keyNrOffset].dx2 - 0.025) * xs, y0);
					cairo_close_path (cr);
					cairo_fill (cr);
				}
				else
				{
					color = *(blackBgColors.getColor (!activeKeys[i] ? BColors::INACTIVE : (pressedKeys[i] ? BColors::ACTIVE : BColors::NORMAL)));
					cairo_set_source_rgba (cr, CAIRO_RGBA (color));
					cairo_rectangle_rounded (cr, x0 + keyX * xs, y0, keyCoords[keyNrOffset].width * xs, 0.6667 * h, 0.1 * xs, 0b1100);
					cairo_fill (cr);
				}

			}
			cairo_destroy (cr);
		}
	}
}

}
