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


#include "Slot.hpp"
#include "BUtilities/mix.hpp"
#include "BOops.hpp"
#include <new>
#include <iostream>
#include "FxSurprise.hpp"
#include "FxAmp.hpp"
#include "FxBalance.hpp"
#include "FxWidth.hpp"
#include "FxDelay.hpp"
#include "FxReverser.hpp"
#include "FxChopper.hpp"
#include "FxJumbler.hpp"
#include "FxTapeStop.hpp"
#include "FxTapeSpeed.hpp"
#include "FxScratch.hpp"
#include "FxWowFlutter.hpp"
#include "FxBitcrush.hpp"
#include "FxDecimate.hpp"
#include "FxDistortion.hpp"
#include "FxFilter.hpp"
#include "FxNoise.hpp"
#include "FxCrackles.hpp"
#include "FxStutter.hpp"
#include "FxFlanger.hpp"
#include "FxPhaser.hpp"
#include "FxRingModulator.hpp"
#include "FxOops.hpp"
#include "FxWah.hpp"
#include "FxReverb.hpp"
#include "FxGalactic.hpp"
#include "FxInfinity.hpp"
#include "FxTremolo.hpp"
#include "FxWaveshaper.hpp"
#include "FxTeslaCoil.hpp"
#include "FxBanger.hpp"
#include "FxEQ.hpp"

Slot::Slot () : Slot (nullptr, FX_INVALID, nullptr, nullptr, 0, 0.0f, 0.0) {}

Slot::Slot (BOops* plugin, const BOopsEffectsIndex effect, float* params, Pad* pads, const size_t size, const float mixf, const double framesPerStep) :
	plugin (plugin), effect (FX_INVALID), midis (), slotShape(), slotKeys(), slotMode (MODE_PATTERN),
	initPos (0.0), lastPos (0.0), patchPos (0.0), shapePaused (true),
	fx (nullptr),
	size (size), mixf (mixf), framesPerStep (framesPerStep), buffer (nullptr), shape ()
{
	std::fill (startPos, startPos + NR_STEPS, -1);

	buffer = new RingBuffer<Stereo> (1.5 * double (size) * framesPerStep);

	if (params) std::copy (params, params + NR_PARAMS, this->params);
	else std::fill (this->params, this->params + NR_PARAMS, 0.5f);

	if (pads) std::copy (pads, pads + NR_STEPS, this->pads);
	else std::fill (this->pads, this->pads + NR_STEPS, Pad());

	shape.setDefaultShape();
	slotKeys.fill (false);

	fx = newFx (effect);
}

Slot:: Slot (const Slot& that) :
	plugin (that.plugin), 
	effect (that.effect), 
	midis (that.midis),
	slotShape (that.slotShape),
	slotKeys (that.slotKeys),
	slotMode (that.slotMode),
	initPos (that.initPos),
	lastPos (that.lastPos),
	fx (nullptr),
	size (that.size), 
	mixf (that.mixf),
	framesPerStep (that.framesPerStep), 
	buffer (nullptr),
	shape (that.shape)
{
	std::copy (that.params, that.params + NR_PARAMS, params);
	std::copy (that.pads, that.pads + NR_STEPS, pads);
	std::copy (that.startPos, that.startPos + NR_STEPS, startPos);

	if (that.buffer) buffer = new RingBuffer<Stereo> (*that.buffer);
	if (that.fx) fx = newFx (effect);
}

Slot::~Slot ()
{
	if (fx) delete fx;
	if (buffer) delete buffer;
}

Slot& Slot::operator= (const Slot& that)
{
	plugin = that.plugin;
	effect = that.effect;
	midis = that.midis;
	slotShape = that.slotShape;
	slotKeys = that.slotKeys;
	slotMode = that.slotMode;
	initPos = that.initPos;
	lastPos = that.lastPos;
	patchPos = that.patchPos;
	size = that.size;
	mixf = that.mixf;
	framesPerStep = that.framesPerStep;
	shape = that.shape;

	std::copy (that.params, that.params + NR_PARAMS, params);
	std::copy (that.pads, that.pads + NR_STEPS, pads);
	std::copy (that.startPos, that.startPos + NR_STEPS, startPos);

	if (fx) {delete fx; fx = nullptr;}
	if (buffer) {delete buffer; buffer = nullptr;}

	if (that.buffer) buffer = new RingBuffer<Stereo> (*that.buffer);
	if (that.fx) fx = newFx (effect);

	return *this;
}

void Slot::setPad (const int index, const Pad& pad)
{
	const int size = (pad.size > pads[index].size ? pad.size : pads[index].size);
	pads[index] = pad;
	startPos[index] = getStartPad (index);
	for (int i = 1; (i < size) && (index + i < NR_STEPS); ++i) startPos[index + i] = getStartPad (index + i);
}

int Slot::getStartPad (const int index) const
{
	for (int i = index; i >= 0; --i)
	{
		if ((pads[i].gate > 0) && (pads[i].mix > 0))
		{
			if (i + pads[i].size > index) return i;
			else return -1;
		}
	}

	return -1;
}

Fx* Slot::newFx (const BOopsEffectsIndex effect)
{
	Fx* fx = nullptr;

	switch (effect)
	{
		case FX_SURPRISE:	fx = new FxSurprise (&buffer, params, pads, plugin);
							break;

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

		case FX_SCRATCH:	fx = new FxScratch (&buffer, params, pads, &framesPerStep, &shape);
							break;

		case FX_WOWFLUTTER:	fx = new FxWowFlutter (&buffer, params, pads, &framesPerStep);
							break;

		case FX_BITCRUSH:	fx = new FxBitcrush (&buffer, params, pads);
							break;

		case FX_DECIMATE:	fx = new FxDecimate (&buffer, params, pads);
							break;

		case FX_DISTORTION:	fx = new FxDistortion (&buffer, params, pads);
							break;

		case FX_FILTER:		fx = new FxFilter (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_NOISE:		fx = new FxNoise (&buffer, params, pads);
							break;

		case FX_CRACKLES:	fx = new FxCrackles (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000);
							break;

		case FX_STUTTER:	fx = new FxStutter (&buffer, params, pads, &framesPerStep);
							break;

		case FX_FLANGER:	fx = new FxFlanger (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000);
							break;

		case FX_PHASER:		fx = new FxPhaser (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000);
							break;

		case FX_RINGMOD:	fx = new FxRingModulator (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000);
							break;

		case FX_OOPS:		fx = new FxOops (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000, plugin ? plugin->pluginPath : nullptr);
							break;

		case FX_WAH:		fx = new FxWah (&buffer, params, pads, plugin ? plugin->host.rate : 48000, &shape);
							break;

		case FX_REVERB:		fx = new FxReverb (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_GALACTIC:	fx = new FxGalactic (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_INFINITY:	fx = new FxInfinity (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_TREMOLO:	fx = new FxTremolo (&buffer, params, pads, &framesPerStep, plugin ? plugin->host.rate : 48000);
							break;

		case FX_WAVESHAPER:	fx = new FxWaveshaper (&buffer, params, pads, &shape);
							break;

		case FX_TESLACOIL:	fx = new FxTestlaCoil (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_BANGER:		fx = new FxBanger (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_EQ:			fx = new FxEQ (&buffer, params, pads, plugin ? plugin->host.rate : 48000);
							break;

		case FX_INVALID:	fx = nullptr;
							break;

		default: 			fx = new Fx (&buffer, params, pads);
	}

	if (fx != 0) fx->init (0.0);

	return fx;
}

void Slot::init (const double position) 
{
	initPos = position;
	lastPos = position;
	patchPos = 0.0;
	shapePaused = true;
	if (fx) fx->init (position);
}

void Slot::end ()
{
	shapePaused = true;
	if (fx) fx->end ();
}

void Slot::setSlotShape (const Shape<SHAPE_MAXNODES>& source) 
{
	slotShape = source;
	if (slotMode != MODE_KEYS) slotMode = ((slotShape != Shape<SHAPE_MAXNODES>()) ? MODE_SHAPE : MODE_PATTERN);
	if (fx && (slotMode != MODE_PATTERN)) fx->init (0.0);
}

void Slot::setSlotKeys (const std::array<bool, NR_PIANO_KEYS + 1>& source)
{
	slotKeys = source;
	if (source[NR_PIANO_KEYS]) slotMode = MODE_KEYS;
	else slotMode = ((slotShape != Shape<SHAPE_MAXNODES>()) ? MODE_SHAPE : MODE_PATTERN);
	if (fx && (slotMode != MODE_PATTERN)) fx->init (0.0);
}

void Slot::addMidiKey (const MidiKey& midiKey)
{
	// Search and replace already existing note
	for (MidiKey** iit = midis.begin(); iit < midis.end(); ++iit)
	{
		if (midiKey.note == (**iit).note)
		{
			**iit = midiKey;
			return;
		}
	}

	// Not found? Append
	midis.push_back (midiKey);
}

void Slot::removeMidiKey (const MidiKey& midiKey)
{
	// Search and remove already existing note
	for (MidiKey** iit = midis.begin(); iit < midis.end(); ++iit)
	{
		if (midiKey.note == (**iit).note)
		{
			midis.erase (iit);
			return;
		}
	}
}

MidiKey Slot::findMidiKey (const uint8_t note)
{
	for (MidiKey** iit = midis.begin(); iit < midis.end(); ++iit)
	{
		if (note == (**iit).note) return **iit;
	}

	return {0, 0, 0, 0, 0.0, 0.0};
}

Stereo Slot::play (const double position)
{
	if (slotMode == MODE_SHAPE) return play (position, slotShape.getMapValue (position / size));
	if ((!fx) || (!buffer)) return Stereo();
	if (!params[SLOTS_PLAY]) return (*buffer).front();
	if (!fx->isPlaying()) return (*buffer).front();
	if (!isPadSet(position)) return (*buffer).front();

	const int index = startPos[int(position)];
	const double relpos = position - double (index);
	const Stereo s0 = (*buffer).front();
	const Stereo s1 = fx->playPad (relpos, pads[index].size, pads[index].mix);
	return BUtilities::mix<Stereo> (s0, s1, mixf);
}

Stereo Slot::play (const double position, const float mx)
{
	if ((!fx) || (!buffer)) return Stereo();
	if (!params[SLOTS_PLAY]) return (*buffer).front();

	if (shapePaused && (mx >= 0.0001)) init (position);
	shapePaused = (mx < 0.0001);
	if (shapePaused) 
	{
		end();
		return (*buffer).front();
	}

	if ((position < lastPos) && (position < 1.0)) patchPos += std::ceil (lastPos);
	lastPos = position;

	const Stereo s0 = (*buffer).front();
	const Stereo s1 = fx->play (std::max (position - initPos + patchPos, 0.0), size, mx, mixf);
	return BUtilities::mix<Stereo> (s0, s1, mixf);
}
