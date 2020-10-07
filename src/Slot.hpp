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

#include <new>
#include <iostream>
#include "Definitions.hpp"
#include "RingBuffer.hpp"
#include "Stereo.hpp"
#include "Pad.hpp"
#include "Ports.hpp"
#include "Fx.hpp"
#include "FxAmp.hpp"
#include "FxBalance.hpp"
#include "FxWidth.hpp"
#include "FxDelay.hpp"
#include "FxReverser.hpp"
#include "FxChopper.hpp"
#include "FxJumbler.hpp"
#include "FxTapeStop.hpp"
#include "FxTapeSpeed.hpp"
#include "FxBitcrush.hpp"
#include "FxDecimate.hpp"
#include "FxDistortion.hpp"
#include "FxNoise.hpp"

struct Slot
{
	Slot 	() : Slot (FX_INVALID, false, 0.0f, 0.0f, nullptr, nullptr, 0, 0.0) {}

	Slot 	(const BNoname01EffectsIndex effect, const bool playing, const float pan,
		const float mix, float* params, Pad* pads, const size_t size, const double framesPerStep) :
		effect (FX_INVALID), fx (nullptr),
		size (size), framesPerStep (framesPerStep), buffer (nullptr)
	{
		buffer = new RingBuffer<Stereo> (1.5 * double (size) * framesPerStep);

		if (params) std::copy (params, params + NR_PARAMS, this->params);
		else std::fill (this->params, this->params + NR_PARAMS, 0.5f);

		if (pads) std::copy (pads, pads + NR_STEPS, this->pads);
		else std::fill (this->pads, this->pads + NR_STEPS, Pad());

		fx = newFx (effect);
	}

	Slot (const Slot& that) :
	effect (that.effect), fx (nullptr),
	size (that.size), framesPerStep (that.framesPerStep), buffer (nullptr)
	{
		if (that.params) std::copy (that.params, that.params + NR_PARAMS, params);
		if (that.pads) std::copy (that.pads, that.pads + NR_PARAMS, pads);
		if (that.buffer) buffer = new RingBuffer<Stereo> (*that.buffer);
		if (that.fx) fx = newFx (effect);
	}

	~Slot ()
	{
		if (fx) delete fx;
		if (buffer) delete buffer;
	}

	Slot& operator= (const Slot& that)
	{
		effect = that.effect;
		size = that.size;
		framesPerStep = that.framesPerStep;

		if (that.params) std::copy (that.params, that.params + NR_PARAMS, params);
		if (that.pads) std::copy (that.pads, that.pads + NR_PARAMS, pads);

		if (fx) {delete fx; fx = nullptr;}
		if (buffer) {delete buffer; buffer = nullptr;}

		if (that.buffer) buffer = new RingBuffer<Stereo> (*that.buffer);
		if (that.fx) fx = newFx (effect);

		return *this;
	}

	Fx* newFx (const BNoname01EffectsIndex effect)
	{
		Fx* fx = nullptr;

		switch (effect)
		{
			case FX_AMP:		fx = new FxAmp (&buffer, params, pads);
						break;

			case FX_BALANCE:	fx = new FxBalance (&buffer, params, pads);
						break;

			case FX_WIDTH:		fx = new FxWidth (&buffer, params, pads);
						break;

			case FX_DELAY:		fx = new FxDelay (&buffer, params, pads, &framesPerStep, &size);
						break;

			case FX_REVERSER:	fx = new FxReverser (&buffer, params, pads, &framesPerStep);
						break;

			case FX_CHOPPER:	fx = new FxChopper (&buffer, params, pads);
						break;

			case FX_JUMBLER:	fx = new FxJumbler (&buffer, params, pads, &framesPerStep, &size);
						break;

			case FX_TAPE_STOP:	fx = new FxTapeStop (&buffer, params, pads, &framesPerStep);
						break;

			case FX_TAPE_SPEED:	fx = new FxTapeSpeed (&buffer, params, pads, &framesPerStep);
						break;

			case FX_BITCRUSH:	fx = new FxBitcrush (&buffer, params, pads);
						break;

			case FX_DECIMATE:	fx = new FxDecimate (&buffer, params, pads);
						break;

			case FX_DISTORTION:	fx = new FxDistortion (&buffer, params, pads);
						break;

			case FX_NOISE:		fx = new FxNoise (&buffer, params, pads);
						break;

			case FX_INVALID:	fx = nullptr;
						break;

			default: 		fx = new Fx (&buffer, params, pads);
		}

		return fx;
	}

	int getStart () const {return (fx ? fx->getStart () : -1);}
	int getStart (const double position) const {return (fx ? fx->getStart (position) : -1);}
	bool isPad (const double position) const {return (fx ? fx->isPad (position) : false);}
	void start (const double position) {if (fx) fx->start (position);}
	Stereo play (const double position) {return (fx ? fx->play (position): Stereo ());}
	void end () {if (fx) fx->end ();}

	BNoname01EffectsIndex effect;
	float params [NR_PARAMS];
	Pad pads[NR_STEPS];
	Fx* fx;
	size_t size;
	double framesPerStep;
	RingBuffer<Stereo>* buffer;
};

#endif /* SLOT_HPP_ */
