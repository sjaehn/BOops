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

#ifndef PORTS_HPP_
#define PORTS_HPP_

enum BOopsPortIndex
{
	CONTROL		= 0,
	NOTIFY		= 1,
	AUDIO_IN_1	= 2,
	AUDIO_IN_2	= 3,
	AUDIO_OUT_1	= 4,
	AUDIO_OUT_2	= 5,

	CONTROLLERS	= 6,

	PLAY			= 0,
	SOURCE			= 1,
	PLAY_MODE		= 2,
	ON_MIDI			= 3,
	AUTOPLAY_BPM		= 4,
	AUTOPLAY_BPB		= 5,
	AUTOPLAY_POSITION	= 6,
	STEPS			= 7,
	BASE			= 8,
	BASE_VALUE		= 9,

	SLOTS		= 10,
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

enum BOopsPlayIndex
{
	PLAY_OFF	= 0,
	PLAY_ON		= 1,
	PLAY_BYPASS	= 2
};

enum BOopsPlayModeIndex
{
	AUTOPLAY	= 0,
	MIDI_CONTROLLED	= 1,
	HOST_CONTROLLED	= 2
};

enum BOopsBaseIndex
{
	SECONDS		= 0,
	BEATS		= 1,
	BARS		= 2
};

enum BOopsEffectsIndex
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
	FX_FLANGER	= 22,
	FX_PHASER	= 23,
	FX_RINGMOD	= 24,
	FX_OOPS		= 25,
	FX_WAH		= 26,
	FX_REVERB	= 27,
	FX_GALACTIC	= 28,
	FX_INFINITY	= 29,
	FX_TREMOLO	= 30,
	FX_WAVESHAPER	= 31,
	FX_TESLACOIL	= 32,
	FX_BANGER	= 33,
	FX_EQ		= 34,
	NR_FX		= 35,
	FX_INVALID	= 255
};

enum BOopsSourceIndex
{
	SOURCE_STREAM	= 0,
	SOURCE_SAMPLE	= 1
};

enum BOopsDistortionIndex
{
	HARDCLIP	= 0,
	SOFTCLIP	= 1,
	FOLDBACK	= 2,
	OVERDRIVE	= 3,
	FUZZ		= 4
};

enum BOopsWaveformIndex
{
	SINE_WAVE	= 0,
	TRIANGLE_WAVE	= 1,
	SQUARE_WAVE	= 2,
	SAW_WAVE	= 3,
	REVERSE_SAW_WAVE= 4
};

#define BOOPSFXNAMES \
{ \
	{-1, "Basic"}, {2, "  Amp"}, {3, "  Balance"}, {4, "  Width"}, /*{5, "Pitch (TODO)"}, */ \
	{-1, "Temporal"}, {27, "  Reverb"}, {28, "  Galactic reverb"}, {29, "  Infinity reverb"}, {6, "  Delay"}, {7, "  Reverse"}, \
	{-1, "Pattern"}, {30, "  Tremolo"}, {8, "  Chop"}, {21, "  Stutter"}, {9, "  Jumble"}, \
	{-1, "Tape"}, {10, "  Tape stop"}, /*{11, "Tape start (TODO)"}, */{12, "  Tape speed"}, {14, "  Wow & flutter"}, \
	{-1, "Vinyl"}, {13, "  Scratch"}, {19, "  Noise"}, {20, "  Crackles"}, \
	{-1, "Distortion"}, {15, "  Crush"}, {16, "  Decimate"}, {17, "  Distortion"}, {31, "  Waveshaper"}, \
	{-1, "Filter"}, {18, "  Filter"}, {34, "  EQ"}, \
	{-1, "Modulators"}, {22, "  Flanger"}, {23, "  Phaser"}, {24, "  Ring modulator"}, {26, "  Wah"}, {33, "  Banger"}, \
	{-1, "SFX"}, {32, "  Tesla coil"}, {25, "  Oops"}, \
	{-1, "Random"}, {1, "  Surprise box"} \
}

#define BOOPSFXBGNAMES \
{ \
	"", \
	"inc/Surprise_bg.png", \
	"inc/Amp_bg.png", \
	"inc/Amp_bg.png", \
	"inc/Amp_bg.png", \
	"", \
	"inc/Delay_bg.png", \
	"inc/Delay_bg.png", \
	"inc/Chop_bg.png", \
	"inc/Chop_bg.png", \
	"inc/Tape_bg.png", \
	"", \
	"inc/Tape_bg.png", \
	"inc/Vinyl_bg.png", \
	"inc/Tape_bg.png", \
	"inc/Distortion_bg.png", \
	"inc/Distortion_bg.png", \
	"inc/Distortion_bg.png", \
	"inc/Distortion_bg.png", \
	"inc/Vinyl_bg.png", \
	"inc/Vinyl_bg.png", \
	"inc/Chop_bg.png", \
	"inc/Flanger_bg.png", \
	"inc/Flanger_bg.png", \
	"inc/Flanger_bg.png", \
	"inc/Oops_bg.png", \
	"inc/Flanger_bg.png", \
	"inc/Delay_bg.png", \
	"inc/Delay_bg.png", \
	"inc/Delay_bg.png", \
	"inc/Chop_bg.png", \
	"inc/Distortion_bg.png", \
	"inc/TeslaCoil_bg.png", \
	"inc/Flanger_bg.png", \
	"inc/Distortion_bg.png" \
}

#endif /* PORTS_HPP_ */
