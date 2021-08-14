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

#ifndef SLOT_HPP_
#define SLOT_HPP_

#include <array>
#include "Definitions.hpp"
#include "RingBuffer.hpp"
#include "Stereo.hpp"
#include "Pad.hpp"
#include "Ports.hpp"
#include "Fx.hpp"
#include "Shape.hpp"
#include "MidiKey.hpp"
#include "StaticArrayList.hpp"

enum SlotMode
{
	MODE_PATTERN	= 0,
	MODE_SHAPE		= 1,
	MODE_KEYS		= 2
};

class BOops; // Forward declaration;

class Slot
{
public:
	Slot();
	Slot (BOops* plugin, const BOopsEffectsIndex effect, float* params, Pad* pads, const size_t size, const float mixf, const double framesPerStep);
	Slot (const Slot& that);
	~Slot ();

	Slot& operator= (const Slot& that);
	void setPad (const int index, const Pad& pad);
	Pad getPad (const int index) const {return pads[index];}
	void setSlotShape (const Shape<SHAPE_MAXNODES>& source);
	Shape<SHAPE_MAXNODES> getSlotShape () const {return slotShape;}
	void setSlotKeys (const std::array<bool, NR_PIANO_KEYS + 1>& source);
	bool isKey (const int index) {return slotKeys[index];}
	void addMidiKey (const MidiKey& midiKey);
	void removeMidiKey (const MidiKey& midiKey);
	MidiKey findMidiKey (const uint8_t note);
	SlotMode getMode () const {return slotMode;}
	Fx* newFx (const BOopsEffectsIndex effect);
	int getStartPad (const int index) const;
	bool isPadSet (const int index) const {return ((startPos[index] >= 0) && (startPos[index] + pads[startPos[index]].size > index));}
	void init (const double position) {if (fx) fx->init (position);}
	Stereo play (const double position);
	Stereo play (const double position, const float mx);
	void end () {if (fx) fx->end ();}

	BOops* plugin;
	BOopsEffectsIndex effect;
	float params [NR_PARAMS];
	StaticArrayList<MidiKey, 16> midis;
protected:
	float adsr (const double position, const double size) const;
	Pad pads[NR_STEPS];
	Shape<SHAPE_MAXNODES> slotShape;
	std::array<bool, NR_PIANO_KEYS + 1> slotKeys;
	SlotMode slotMode;

public:
	int startPos[NR_STEPS];
	Fx* fx;
	size_t size;
	float mixf;
	double framesPerStep;
	RingBuffer<Stereo>* buffer;
	Shape<SHAPE_MAXNODES> shape;
};

#endif /* SLOT_HPP_ */
