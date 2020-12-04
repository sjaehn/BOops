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

#ifndef GETURIS_HPP_
#define GETURIS_HPP_

#include "Urids.hpp"
#include "Definitions.hpp"

void getURIs (LV2_URID_Map* m, BOopsURIDs* uris)
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
	uris->atom_Path = m->map(m->handle, LV2_ATOM__Path);
	uris->atom_Bool = m->map(m->handle, LV2_ATOM__Bool);
	uris->midi_Event = m->map(m->handle, LV2_MIDI__MidiEvent);
	uris->time_Position = m->map(m->handle, LV2_TIME__Position);
	uris->time_barBeat = m->map(m->handle, LV2_TIME__barBeat);
	uris->time_bar = m->map(m->handle, LV2_TIME__bar);
	uris->time_beatsPerMinute = m->map(m->handle, LV2_TIME__beatsPerMinute);
	uris->time_beatUnit = m->map(m->handle, LV2_TIME__beatUnit);
	uris->time_beatsPerBar = m->map(m->handle, LV2_TIME__beatsPerBar);
	uris->time_speed = m->map(m->handle, LV2_TIME__speed);
	uris->bOops_uiOn = m->map(m->handle, BOOPS_URI "#UIon");
	uris->bOops_uiOff = m->map(m->handle, BOOPS_URI "#UIoff");
	uris->bOops_messageEvent = m->map(m->handle, BOOPS_URI "#messageEvent");
	uris->bOops_message = m->map(m->handle, BOOPS_URI "#message");
	uris->bOops_statusEvent = m->map(m->handle, BOOPS_URI "#statusEvent");
	uris->bOops_position = m->map(m->handle, BOOPS_URI "#position");
	uris->bOops_slotEvent = m->map(m->handle, BOOPS_URI "#slotEvent");
	uris->bOops_slot = m->map(m->handle, BOOPS_URI "#slot");
	uris->bOops_pads = m->map(m->handle, BOOPS_URI "#pads");
	uris->bOops_padEvent = m->map(m->handle, BOOPS_URI "#padEvent");
	uris->bOops_step = m->map(m->handle, BOOPS_URI "#step");
	uris->bOops_pad = m->map(m->handle, BOOPS_URI "#pad");
	uris->bOops_shapeEvent = m->map(m->handle, BOOPS_URI "#shapeEvent");
	uris->bOops_shapeData = m->map(m->handle, BOOPS_URI "#shapeData");
	uris->bOops_allocateBuffers = m->map(m->handle, BOOPS_URI "#allocateBuffers");
	uris->bOops_installBuffers = m->map(m->handle, BOOPS_URI "#installBuffers");
	uris->bOops_freeBuffers = m->map(m->handle, BOOPS_URI "#freeBuffers");
	uris->bOops_allocateFx = m->map(m->handle, BOOPS_URI "#allocateFx");
	uris->bOops_installFx = m->map(m->handle, BOOPS_URI "#installFx");
	uris->bOops_freeFx = m->map(m->handle, BOOPS_URI "#freeFx");
	uris->bOops_statePad = m->map(m->handle, BOOPS_URI "#statePad");
	uris->bOops_waveformEvent = m->map(m->handle, BOOPS_URI "#waveformEvent");
	uris->bOops_waveformStart = m->map(m->handle, BOOPS_URI "#waveformStart");
	uris->bOops_waveformData = m->map(m->handle, BOOPS_URI "#notify_waveformData");
	uris->bOops_transportGateKeyEvent = m->map(m->handle, BOOPS_URI "#transportGateKeyEvent");
	uris->bOops_transportGateKeys = m->map(m->handle, BOOPS_URI "#transportGateKeys");
	uris->bOops_samplePathEvent = m->map(m->handle, BOOPS_URI "#samplePathEvent");
	uris->bOops_samplePath = m->map(m->handle, BOOPS_URI "#samplePath");
	uris->bOops_sampleStart = m->map(m->handle, BOOPS_URI "#sampleStart");
	uris->bOops_sampleEnd = m->map(m->handle, BOOPS_URI "#sampleEnd");
	uris->bOops_sampleAmp = m->map(m->handle, BOOPS_URI "#sampleAmp");
	uris->bOops_sampleLoop = m->map(m->handle, BOOPS_URI "#sampleLoop");
	uris->bOops_installSample = m->map(m->handle, BOOPS_URI "#installSample");
	uris->bOops_sampleFreeEvent = m->map(m->handle, BOOPS_URI "#sampleFreeEvent");
}

#endif /* GETURIS_HPP_ */
