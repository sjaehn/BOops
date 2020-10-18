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

#ifndef CONTROLLERLIMITS_HPP_
#define CONTROLLERLIMITS_HPP_

#include "Ports.hpp"
#include "Limit.hpp"

const Limit controllerLimits[NR_CONTROLLERS] =
{
	{0, 2, 1},		// PLAY
	{0, 2, 1},		// PLAY_MODE
	{0, 2, 1},		// ON_MIDI
	{1.0, 300.0, 0.0},	// AUTOPLAY_BPM
	{1, 16, 1},		// AUTOPLAY_BPB
	{1, 32, 1},		// STEPS
	{0, 2, 1},		// BASE
	{1.0, 16.0, 0.0},	// BASE_VALUE
	{0, 23, 1},		// SLOTS = 1; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 2; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 3; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 4; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 5; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 6; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 7; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 8; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 9; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 10; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 11; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0, 23, 1},		// SLOTS = 12; SLOTS_EFFECT
	{0, 1, 1},		// SLOTS_PLAY
	{0.0, 1.0, 0.0},	// SLOTS_ATTACK
	{0.0, 1.0, 0.0},	// SLOTS_DECAY
	{0.0, 1.0, 0.0},	// SLOTS_SUSTAIN
	{0.0, 1.0, 0.0},	// SLOTS_RELEASE
	{-1.0, 1.0, 0.0},	// SLOTS_PAN
	{0.0, 1.0, 0.0},	// SLOTS_MIX
	{0.0, 1.0, 0.0},	// SLOTS_OPTPARAMS
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 1.0, 0.0}
};


#endif /* CONTROLLERLIMITS_HPP_ */
