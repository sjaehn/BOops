/* B.Noname01
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

#include "Definitions.hpp"
#include "RingBuffer.hpp"
#include "Stereo.hpp"
#include "Pad.hpp"
#include "Ports.hpp"
#include "Fx.hpp"
#include "Shape.hpp"

class BNoname01; // Forward declaration;

struct Slot
{
	Slot();
	Slot (BNoname01* plugin, const BNoname01EffectsIndex effect, const bool playing, const float pan,
	      const float mix, float* params, Pad* pads, const size_t size, const double framesPerStep);
	Slot (const Slot& that);
	~Slot ();

	Slot& operator= (const Slot& that);
	Fx* newFx (const BNoname01EffectsIndex effect);
	int getStart () const {return (fx ? fx->getStart () : -1);}
	int getStart (const double position) const {return (fx ? fx->getStart (position) : -1);}
	bool isPad (const double position) const {return (fx && fx->isPad (position));}
	void start (const double position)  {if (fx) fx->start (position);}
	Stereo play (const double position) {return (fx && buffer ? BUtilities::mix<Stereo> ((*buffer)[0], fx->play (position), mix) : Stereo ());}
	void end () {if (fx) fx->end ();}

	BNoname01* plugin;
	BNoname01EffectsIndex effect;
	float mix;
	float params [NR_PARAMS];
	Pad pads[NR_STEPS];
	Fx* fx;
	size_t size;
	double framesPerStep;
	RingBuffer<Stereo>* buffer;
	Shape<SHAPE_MAXNODES> shape;
};

#endif /* SLOT_HPP_ */
