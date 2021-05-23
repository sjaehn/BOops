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

#ifndef FXDEFAULTS_HPP_
#define FXDEFAULTS_HPP_

#include <array>
#include <string>
#include "Ports.hpp"

#define STDPARAMS 1.0, 0.1, 0.1, 1.0, 0.1, 0.0, 1.0

const std::array<const std::array<const float, NR_PARAMS>, NR_FX> fxDefaultValues =
{{
	// SLOTS_PLAY, SLOTS_ATTACK, SLOTS_DECAY, SLOTS_SUSTAIN, SLOTS_RELEASE,	SLOTS_PAN, SLOTS_MIX, SLOTS_OPTPARAMS
	{{STDPARAMS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_NONE
	{{STDPARAMS, 0.5, 0, 0.5, 0, 0.5, 0, 0.5, 0, 0.5, 0, 0.5, 0}},				// FX_SURPRISE
	{{STDPARAMS, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_AMP
	{{STDPARAMS, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_BALANCE
	{{STDPARAMS, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_WIDTH
	{{STDPARAMS, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_PITCH
	{{STDPARAMS, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_DELAY
	{{STDPARAMS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_REVERSER
	{{STDPARAMS, 8, 0.1, 1.0, 0.5, 0.75, 0.25, 1.0, 0.0, 0.75, 0.0, 0.0, 0}},		// FX_CHOPPER
	{{STDPARAMS, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_JUMBLER
	{{STDPARAMS, 0.5, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_TAPE_STOP
	{{STDPARAMS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_TAPE_START
	{{STDPARAMS, 1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_TAPE_SPEED
	{{STDPARAMS, 1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_SCRATCH
	{{STDPARAMS, 0.5, 0, 1.0, 0, 0.1, 0, 0.2, 0, 0, 0, 0, 0}},				// FX_WOWFLUTTER
	{{STDPARAMS, 0.5, 0, 0.46875, 0, 0, 0, 0, 0, 0, 0, 0, 0}},				// FX_BITCRUSH
	{{STDPARAMS, 1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},					// FX_DECIMATE
	{{STDPARAMS, 0.375, 0.5, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0}},				// FX_DISTORTION
	{{STDPARAMS, 0.0, 0, 1.0, 0, 0.5, 0, 0, 0, 0, 0, 0, 0}},				// FX_FILTER
	{{1.0, 0.1, 0.1, 1.0, 0.1, 0.0, 0.5, 0.64706, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},	// FX_NOISE
	{{STDPARAMS, 0.5, 0, 0.25, 0, 0.5, 0, 0.3, 0, 0, 0, 0, 0}},				// FX_CRACKLES
	{{STDPARAMS, 2.0/7.0, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},				// FX_STUTTER
	{{1.0, 0.1, 0.1, 1.0, 0.1, 0.0, 0.5, 0.0, 0, 1.0, 0, 0.32183, 0, 0, 0, 0.5, 0, 0, 0}},	// FX_FLANGER
	{{1.0, 0.1, 0.1, 1.0, 0.1, 0.0, 0.5, 0.0, 0, 1.0, 0, 0.32183, 0, 0, 0, 0.5, 0, 0.4, 0}},// FX_PHASER
	{{STDPARAMS, 0.5, 0.0, 0.2, 0.0, 0, 0, 0, 0, 0, 0, 0, 0}},				// FX_RINGMOD
	{{STDPARAMS, 0.5, 0.0, 0.5, 0.0, 0, 0, 0, 0, 0, 0, 0, 0}},				// FX_OOPS
	{{STDPARAMS, 0.28855, 0.0, 0.6667, 0.0, 0.3333, 0.0, 0.375, 0, 0, 0, 0, 0}},		// FX_WAH
	{{STDPARAMS, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0}}				// FX_REVERB

}};

const std::array<const std::string, NR_FX> fxIconFileNames =
{{
		"",
		"Surprise",
		"Amp",
		"Balance",
		"Width",
		"Pitch",
		"Delay",
		"Reverser",
		"Chopper",
		"Jumbler",
		"Tape_stop",
		"Tape_start",
		"Tape_speed",
		"Scratch",
		"Wow_and_flutter",
		"Bitcrush",
		"Decimate",
		"Distortion",
		"Filter",
		"Noise",
		"Crackles",
		"Stutter",
		"Flanger",
		"Phaser",
		"Ringmod",
		"Oops",
		"Wah",
		"Reverb"
}};


#endif /* FXDEFAULTS_HPP_ */
