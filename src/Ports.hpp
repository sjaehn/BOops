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

#ifndef PORTS_HPP_
#define PORTS_HPP_

enum BNoname01PortIndex
{
	CONTROL		= 0,
	NOTIFY		= 1,
	AUDIO_IN_1	= 2,
	AUDIO_IN_2	= 3,
	AUDIO_OUT_1	= 4,
	AUDIO_OUT_2	= 5,

	CONTROLLERS	= 6,

	PLAY		= 0,
	PLAY_MODE	= 1,
	ON_MIDI		= 2,
	AUTOPLAY_BPM	= 3,
	AUTOPLAY_BPB	= 4,
	STEPS		= 5,
	BASE		= 6,
	BASE_VALUE	= 7,

	SLOTS		= 8,
	SLOTS_EFFECT	= 0,
	SLOTS_PARAMS	= 1,
	SLOTS_PLAY	= 0,
	SLOTS_ATTACK	= 1,
	SLOTS_DECAY	= 2,
	SLOTS_SUSTAIN	= 3,
	SLOTS_RELEASE	= 4,
	SLOTS_PAN	= 5,
	SLOTS_MIX	= 6,
	SLOTS_OPTPARAMS	= 7,
	NR_OPTPARAMS	= 12,
	NR_PARAMS	= SLOTS_OPTPARAMS + NR_OPTPARAMS,
	NR_SLOTS	= 12,

	NR_CONTROLLERS	= SLOTS + (SLOTS_PARAMS + SLOTS_OPTPARAMS + NR_OPTPARAMS) * NR_SLOTS
};

enum BNoname01PlayIndex
{
	PLAY_OFF	= 0,
	PLAY_ON		= 1,
	PLAY_BYPASS	= 2
};

enum BNoname01PlayModeIndex
{
	AUTOPLAY	= 0,
	MIDI_CONTROLLED	= 1,
	HOST_CONTROLLED	= 2
};

enum BNoname01BaseIndex
{
	SECONDS		= 0,
	BEATS		= 1,
	BARS		= 2
};

enum BNoname01EffectsIndex
{
	FX_NONE		= 0,
	FX_SURPRISE	= 1,
	FX_AMP		= 2,
	FX_BALANCE	= 3,
	FX_WIDTH	= 4,
	FX_PITCH	= 5,
	FX_DELAY	= 6,
	FX_REVERSER	= 7,
	FX_CHOPPER	= 8,
	FX_JUMBLER	= 9,
	FX_TAPE_STOP	= 10,
	FX_TAPE_START	= 11,
	FX_TAPE_SPEED	= 12,
	FX_SCRATCH	= 13,
	FX_WOWFLUTTER	= 14,
	FX_BITCRUSH	= 15,
	FX_DECIMATE	= 16,
	FX_DISTORTION	= 17,
	FX_FILTER	= 18,
	FX_NOISE	= 19,
	FX_CRACKLES	= 20,
	FX_STUTTER	= 21,
	NR_FX		= 22,
	FX_INVALID	= 255
};

enum BNoname01DistortionIndex
{
	HARDCLIP	= 0,
	SOFTCLIP	= 1,
	FOLDBACK	= 2,
	OVERDRIVE	= 3,
	FUZZ		= 4
};

#define BNONAME01FXNAMES \
{ \
	{1, "Surprise box (TODO)"}, {2, "Amp"}, {3, "Balance"}, {4, "Width"}, {5, "Pitch (TODO)"}, \
	{6, "Delay"}, {7, "Reverse"}, {8, "Chop"}, {21, "Stutter"}, {9, "Jumble"}, \
	{10, "Tape stop"}, {11, "Tape start (TODO)"}, {12, "Tape speed"}, {13, "Scratch (TODO)"}, {14, "Wow & flutter"}, \
	{15, "Crush"}, {16, "Decimate"}, {17, "Distortion"}, {18, "Filter"}, \
	{19, "Noise"}, {20, "Crackles (TODO)"} \
}

#endif /* PORTS_HPP_ */
