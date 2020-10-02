/* PianoWidget.cpp
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

#include "PianoWidget.hpp"

namespace BWidgets
{

PianoWidget::PianoWidget () : PianoWidget (0, 0, 0, 0, "piano", 0, 0) {}

PianoWidget::PianoWidget (const double x, const double y, const double width, const double height, const std::string& name) :
		PianoWidget (x, y, width, height, name, 0, 119) {}

PianoWidget::PianoWidget (const double x, const double y, const double width, const double height, const std::string& name,
						const int startMidiKey, const int endMidiKey) :
		Widget (x, y, width, height, name),
		startMidiKey (startMidiKey), endMidiKey (endMidiKey),
		activeKeys (endMidiKey - startMidiKey + 1, true), pressedKeys (endMidiKey - startMidiKey + 1, false) {}

Widget* PianoWidget::clone () const {return new PianoWidget (*this);}

void PianoWidget::pressKeys (std::vector<bool>& keys)
{
	if (((int) keys.size()) == endMidiKey - startMidiKey + 1) pressedKeys = keys;
	// TODO else throw exception
	update ();
}

std::vector<bool> PianoWidget::getPressedKeys () const {return pressedKeys;}

void PianoWidget::activateKeys (std::vector<bool>& keys)
{
	if (((int) keys.size()) == endMidiKey - startMidiKey + 1) activeKeys = keys;
	// TODO else throw exception
	update ();
}

std::vector<bool> PianoWidget::getActiveKeys () const {return activeKeys;}

}
