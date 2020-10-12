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

#ifndef GETURIS_HPP_
#define GETURIS_HPP_

#include "Urids.hpp"
#include "Definitions.hpp"

void getURIs (LV2_URID_Map* m, BNoname01URIDs* uris)
{
	uris->atom_Float = m->map(m->handle, LV2_ATOM__Float);
	uris->atom_Double = m->map(m->handle, LV2_ATOM__Double);
	uris->atom_Int = m->map(m->handle, LV2_ATOM__Int);
	uris->atom_Long = m->map(m->handle, LV2_ATOM__Long);
	uris->atom_Object = m->map(m->handle, LV2_ATOM__Object);
	uris->atom_Blank = m->map(m->handle, LV2_ATOM__Blank);
	uris->atom_eventTransfer = m->map(m->handle, LV2_ATOM__eventTransfer);
	uris->atom_Vector = m->map(m->handle, LV2_ATOM__Vector);
	uris->atom_String = m->map(m->handle, LV2_ATOM__String);
	uris->midi_Event = m->map(m->handle, LV2_MIDI__MidiEvent);
	uris->time_Position = m->map(m->handle, LV2_TIME__Position);
	uris->time_barBeat = m->map(m->handle, LV2_TIME__barBeat);
	uris->time_bar = m->map(m->handle, LV2_TIME__bar);
	uris->time_beatsPerMinute = m->map(m->handle, LV2_TIME__beatsPerMinute);
	uris->time_beatUnit = m->map(m->handle, LV2_TIME__beatUnit);
	uris->time_beatsPerBar = m->map(m->handle, LV2_TIME__beatsPerBar);
	uris->time_speed = m->map(m->handle, LV2_TIME__speed);
	uris->bNoname01_uiOn = m->map(m->handle, BNONAME01_URI "#UIon");
	uris->bNoname01_uiOff = m->map(m->handle, BNONAME01_URI "#UIoff");
	uris->bNoname01_messageEvent = m->map(m->handle, BNONAME01_URI "#messageEvent");
	uris->bNoname01_message = m->map(m->handle, BNONAME01_URI "#message");
	uris->bNoname01_statusEvent = m->map(m->handle, BNONAME01_URI "#statusEvent");
	uris->bNoname01_position = m->map(m->handle, BNONAME01_URI "#position");
	uris->bNoname01_slotEvent = m->map(m->handle, BNONAME01_URI "#slotEvent");
	uris->bNoname01_slot = m->map(m->handle, BNONAME01_URI "#slot");
	uris->bNoname01_pads = m->map(m->handle, BNONAME01_URI "#pads");
	uris->bNoname01_padEvent = m->map(m->handle, BNONAME01_URI "#padEvent");
	uris->bNoname01_step = m->map(m->handle, BNONAME01_URI "#step");
	uris->bNoname01_pad = m->map(m->handle, BNONAME01_URI "#pad");
	uris->bNoname01_shapeEvent = m->map(m->handle, BNONAME01_URI "#shapeEvent");
	uris->bNoname01_shapeData = m->map(m->handle, BNONAME01_URI "#shapeData");
	uris->bNoname01_allocateBuffers = m->map(m->handle, BNONAME01_URI "#allocateBuffers");
	uris->bNoname01_installBuffers = m->map(m->handle, BNONAME01_URI "#installBuffers");
	uris->bNoname01_freeBuffers = m->map(m->handle, BNONAME01_URI "#freeBuffers");
	uris->bNoname01_allocateFx = m->map(m->handle, BNONAME01_URI "#allocateFx");
	uris->bNoname01_installFx = m->map(m->handle, BNONAME01_URI "#installFx");
	uris->bNoname01_freeFx = m->map(m->handle, BNONAME01_URI "#freeFx");
	uris->bNoname01_statePad = m->map(m->handle, BNONAME01_URI "#statePad");
}

#endif /* GETURIS_HPP_ */
