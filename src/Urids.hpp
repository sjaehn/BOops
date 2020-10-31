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

#ifndef URIDS_HPP_
#define URIDS_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>

struct BNoname01URIDs
{
	LV2_URID atom_Float;
	LV2_URID atom_Double;
	LV2_URID atom_Int;
	LV2_URID atom_Long;
	LV2_URID atom_Object;
	LV2_URID atom_Blank;
	LV2_URID atom_eventTransfer;
	LV2_URID atom_Vector;
	LV2_URID atom_String;
	LV2_URID midi_Event;
	LV2_URID time_Position;
	LV2_URID time_barBeat;
	LV2_URID time_bar;
	LV2_URID time_beatsPerMinute;
	LV2_URID time_beatsPerBar;
	LV2_URID time_beatUnit;
	LV2_URID time_speed;
	LV2_URID bNoname01_uiOn;
	LV2_URID bNoname01_uiOff;
	LV2_URID bNoname01_messageEvent;
	LV2_URID bNoname01_message;
	LV2_URID bNoname01_statusEvent;
	LV2_URID bNoname01_position;
	LV2_URID bNoname01_slotEvent;
	LV2_URID bNoname01_slot;
	LV2_URID bNoname01_pads;
	LV2_URID bNoname01_padEvent;
	LV2_URID bNoname01_step;
	LV2_URID bNoname01_pad;
	LV2_URID bNoname01_shapeEvent;
	LV2_URID bNoname01_shapeData;
	LV2_URID bNoname01_allocateBuffers;
	LV2_URID bNoname01_installBuffers;
	LV2_URID bNoname01_freeBuffers;
	LV2_URID bNoname01_allocateFx;
	LV2_URID bNoname01_installFx;
	LV2_URID bNoname01_freeFx;
	LV2_URID bNoname01_statePad;
	LV2_URID bNoname01_waveformEvent;
	LV2_URID bNoname01_waveformStart;
	LV2_URID bNoname01_waveformData;
	LV2_URID bNoname01_transportGateKeyEvent;
	LV2_URID bNoname01_transportGateKeys;
};

#endif /* URIDS_HPP_ */
