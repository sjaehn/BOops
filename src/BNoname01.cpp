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

#include <cstdio>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "BNoname01.hpp"
#include "ControllerLimits.hpp"
#include "BUtilities/stof.hpp"
#include "getURIs.hpp"

#ifndef LIMIT
#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))
#endif /* LIMIT */

inline double floorfrac (const double value) {return value - floor (value);}


BNoname01::BNoname01 (double samplerate, const LV2_Feature* const* features) :
	Transport {samplerate, 120.0f, 1.0f, 0ul, 0.0f, 4.0f, 4},
	map(NULL), workerSchedule (NULL), urids (),
	host {samplerate, 120.0f, 1.0f, 0ul, 0.0f, 4.0f, 4},
	position(0), offset(0), refFrame(0),
	controlPort(NULL), notifyPort(NULL),
	audioInput1(NULL), audioInput2(NULL), audioOutput1(NULL), audioOutput2(NULL),
	new_controllers {NULL}, globalControllers {0},
	forge (), notify_frame (),
	oStep (0), message (),
	ui_on(false), scheduleNotifySlot {false},
	scheduleNotifyStatus (false), scheduleResizeBuffers (false), scheduleSetFx {false}

{
	//Scan host features for URID map
	LV2_URID_Map* m = NULL;
	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0)
		{
			m = (LV2_URID_Map*) features[i]->data;
		}

		else if (!strcmp(features[i]->URI, LV2_WORKER__schedule))
		{
                        workerSchedule = (LV2_Worker_Schedule*)features[i]->data;
		}
	}
	if (!m) throw std::invalid_argument ("Host does not support urid:map");
	if (!workerSchedule) throw std::invalid_argument ("BJumblr.lv2: Host does not support work:schedule.");

	//Map URIS
	map = m;
	getURIs (m, &urids);

	// Initialize forge
	lv2_atom_forge_init (&forge, map);

	// Initialize slots
	std::fill (slots, slots + NR_SLOTS, Slot (this, FX_NONE, true, 0.0f, 1.0f, nullptr, nullptr, 16, 0.25 * rate));
}

BNoname01::~BNoname01 () {}

void BNoname01::connect_port(uint32_t port, void *data)
{
	switch (port) {
	case CONTROL:
		controlPort = (LV2_Atom_Sequence*) data;
		break;

	case NOTIFY:
		notifyPort = (LV2_Atom_Sequence*) data;
		break;

	case AUDIO_IN_1:
		audioInput1 = (float*) data;
		break;

	case AUDIO_IN_2:
		audioInput2 = (float*) data;
		break;

	case AUDIO_OUT_1:
		audioOutput1 = (float*) data;
		break;
	case AUDIO_OUT_2:
		audioOutput2 = (float*) data;
		break;

	default:
		if ((port >= CONTROLLERS) && (port < CONTROLLERS + NR_CONTROLLERS)) new_controllers[port - CONTROLLERS] = (float*) data;
	}
}

double BNoname01::getPositionFromBeats (double beats)
{
	if (globalControllers[BASE_VALUE] == 0.0) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS: 	return (bpm ? beats / (globalControllers[BASE_VALUE] * (bpm / 60.0)) : 0.0);
		case BEATS:	return beats / globalControllers[BASE_VALUE];
		case BARS:	return (beatsPerBar ? beats / (globalControllers[BASE_VALUE] * beatsPerBar) : 0.0);
		default:	return 0.0;
	}
}

double BNoname01::getPositionFromFrames (uint64_t frames)
{
	if ((globalControllers[BASE_VALUE] == 0.0) || (rate == 0)) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS: 	return frames * (1.0 / rate) / globalControllers[BASE_VALUE] ;
		case BEATS:	return (bpm ? frames * (speed / (rate / (bpm / 60))) / globalControllers[BASE_VALUE] : 0.0);
		case BARS:	return (bpm && beatsPerBar ? frames * (speed / (rate / (bpm / 60))) / (globalControllers[BASE_VALUE] * beatsPerBar) : 0.0);
		default:	return 0.0;
	}
}

double BNoname01::getPositionFromSeconds (double seconds)
{
	if (globalControllers[BASE_VALUE] == 0.0) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS :	return seconds / globalControllers[BASE_VALUE];
		case BEATS:	return seconds * (bpm / 60.0) / globalControllers[BASE_VALUE];
		case BARS:	return (beatsPerBar ? seconds * (bpm / 60.0 / beatsPerBar) / globalControllers[BASE_VALUE] : 0.0);
		default:	return 0;
	}
}

double BNoname01::getFramesPerStep ()
{
	double bps = (globalControllers[PLAY_MODE] == 0 ? globalControllers[AUTOPLAY_BPM] : this->bpm) / 60.0;
	double bpB = (globalControllers[PLAY_MODE] == 0 ? globalControllers[AUTOPLAY_BPB] : beatsPerBar);
	double s =
	(
		globalControllers[BASE] == 0 ?
		globalControllers[BASE_VALUE] :
		(
			globalControllers[BASE] == 1 ?
			globalControllers[BASE_VALUE] / bps :
			globalControllers[BASE_VALUE] * bpB / bps
		)
	);
	return rate * s / globalControllers[STEPS];
}

void BNoname01::run (uint32_t n_samples)
{
	// Check ports
	if ((!controlPort) || (!notifyPort) || (!audioInput1) || (!audioInput2) || (!audioOutput1) || (!audioOutput2)) return;

	for (int i = 0; i < NR_CONTROLLERS; ++i) if (!new_controllers[i]) return;

	// Prepare forge buffer and initialize atom sequence
	const uint32_t space = notifyPort->atom.size;
	lv2_atom_forge_set_buffer(&forge, (uint8_t*) notifyPort, space);
	lv2_atom_forge_sequence_head(&forge, &notify_frame, 0);

	// Update controller values
	for (int i = 0; i < SLOTS; ++i)
	{
		if (globalControllers[i] != *new_controllers[i])
		{
			float newValue = controllerLimits[i].validate (*new_controllers[i]);
			globalControllers[i] = newValue;

			if (i == PLAY_MODE)
			{
				if (newValue == AUTOPLAY)
				{
					bpm = globalControllers[AUTOPLAY_BPM];
					speed = 1.0;
					beatsPerBar = globalControllers[AUTOPLAY_BPB];
				}
				else Transport::operator= (host);
				stepsChanged ();
			}

			else if (i == STEPS)
			{
				for (int i = 0; i < NR_SLOTS; ++i) slots[i].size = newValue;
				stepsChanged ();
			}

			else if (i == AUTOPLAY_BPM)
			{
				if (globalControllers[PLAY_MODE] == AUTOPLAY) bpm = globalControllers[AUTOPLAY_BPM];
				stepsChanged ();
			}

			else if (i == AUTOPLAY_BPB)
			{
				if (globalControllers[PLAY_MODE] == AUTOPLAY) beatsPerBar = globalControllers[AUTOPLAY_BPB];
				stepsChanged ();
			}

			else if
			((i == BASE) || (i == BASE_VALUE)) stepsChanged ();
		}
	}

	for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
	{
		if (!scheduleSetFx[slotNr])
		{
			if (slots[slotNr].effect != *new_controllers[SLOTS + slotNr * (SLOTS_PARAMS + NR_PARAMS)])
			{
				LV2_Atom_Int msg = {{sizeof (int), urids.bNoname01_allocateFx}, slotNr};
				workerSchedule->schedule_work (workerSchedule->handle, sizeof (msg), &msg);
				scheduleSetFx[slotNr] = true;
				continue;
			}


			for (int params = 0; params < NR_PARAMS; ++params)
			{
				int controllerNr = SLOTS + slotNr * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + params;
				if (slots[slotNr].params[params] != *new_controllers[controllerNr])
				{
					float newValue = controllerLimits[controllerNr].validate (*new_controllers[controllerNr]);
					slots[slotNr].params[params] = newValue;
				}
			}
		}
	}



	// Control and MIDI messages
	uint32_t last_t = 0;
	LV2_ATOM_SEQUENCE_FOREACH(controlPort, ev)
	{
		// Read host & GUI events
		if ((ev->body.type == urids.atom_Object) || (ev->body.type == urids.atom_Blank))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;

			// Process GUI on status data
			if (obj->body.otype == urids.bNoname01_uiOn)
			{
				ui_on = true;
				std::fill (scheduleNotifySlot, scheduleNotifySlot + NR_SLOTS, true);
			}

			// Process GUI off status data
			else if (obj->body.otype == urids.bNoname01_uiOff) ui_on = false;

			// Process slot pads data
			else if (obj->body.otype == urids.bNoname01_slotEvent)
			{
				LV2_Atom *oSl = NULL, *oPd = NULL;
				int slot = -1;
				lv2_atom_object_get (obj,
					 	     urids.bNoname01_slot, &oSl,
						     urids.bNoname01_pads, &oPd,
						     NULL);

				// Slot nr notification
				if (oSl && (oSl->type == urids.atom_Int) && (((LV2_Atom_Int*)oSl)->body >= 0) && (((LV2_Atom_Int*)oSl)->body < NR_SLOTS))
				{
					slot = ((LV2_Atom_Int*)oSl)->body;
				}

				// Pad notification
				if (oPd && (oPd->type == urids.atom_Vector) && (slot >= 0))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oPd;
					if (vec->body.child_type == urids.atom_Float)
					{
						const uint32_t size = (uint32_t) ((oPd->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (Pad));
						Pad* pad = (Pad*) (&vec->body + 1);
						std::copy (pad, pad + LIMIT (size, 0, NR_STEPS), slots[slot].pads);
					}
				}
			}

			// Process single pad data
			else if (obj->body.otype == urids.bNoname01_padEvent)
			{
				LV2_Atom *oSl = NULL, *oSt = NULL, *oPd = NULL;
				int slot = -1;
				int step = -1;
				lv2_atom_object_get (obj,
					 	     urids.bNoname01_slot, &oSl,
						     urids.bNoname01_step, &oSt,
						     urids.bNoname01_pads, &oPd,
						     NULL);

				// Slot nr notification
				if (oSl && (oSl->type == urids.atom_Int) && (((LV2_Atom_Int*)oSl)->body >= 0) && (((LV2_Atom_Int*)oSl)->body < NR_SLOTS))
				{
					slot = ((LV2_Atom_Int*)oSl)->body;
				}

				// Step nr notification
				if (oSt && (oSt->type == urids.atom_Int) && (((LV2_Atom_Int*)oSt)->body >= 0) && (((LV2_Atom_Int*)oSt)->body < NR_STEPS))
				{
					step = ((LV2_Atom_Int*)oSt)->body;
				}

				// Pad notification
				if (oPd && (oPd->type == urids.atom_Vector) && (slot >= 0) && (step >= 0))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oPd;
					if (vec->body.child_type == urids.atom_Float)
					{
						const uint32_t size = (uint32_t) ((oPd->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (Pad));
						if (size == 1)
						{
							Pad* pad = (Pad*) (&vec->body + 1);
							slots[slot].pads[step] = *pad;
						}
					}
				}
			}

			// Process time / position data
			else if (obj->body.otype == urids.time_Position)
			{
				bool scheduleUpdatePosition = false;
				bool scheduleStepsChanged = false;

				// Update bpm, speed, position
				LV2_Atom *oBbeat = NULL, *oBpm = NULL, *oSpeed = NULL, *oBpb = NULL, *oBu = NULL, *oBar = NULL;
				const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
				lv2_atom_object_get
				(
					obj, urids.time_bar, &oBar,
					urids.time_barBeat, &oBbeat,
					urids.time_beatsPerMinute,  &oBpm,
					urids.time_beatsPerBar,  &oBpb,
					urids.time_beatUnit,  &oBu,
					urids.time_speed, &oSpeed,
					NULL
				);

				// BPM changed?
				if (oBpm && (oBpm->type == urids.atom_Float))
				{
					float nbpm = ((LV2_Atom_Float*)oBpm)->body;

					if (nbpm != host.bpm)
					{
						host.bpm = nbpm;
						if (nbpm < 1.0) message.setMessage (JACK_STOP_MSG);
						else message.deleteMessage (JACK_STOP_MSG);

						scheduleStepsChanged = true;
					}
				}

				// Beats per bar changed?
				if (oBpb && (oBpb->type == urids.atom_Float) && (((LV2_Atom_Float*)oBpb)->body > 0) && (((LV2_Atom_Float*)oBpb)->body != host.beatsPerBar))
				{
					host.beatsPerBar = ((LV2_Atom_Float*)oBpb)->body;
					scheduleNotifyStatus = true;
					scheduleStepsChanged = true;
				}

				// BeatUnit changed?
				if (oBu && (oBu->type == urids.atom_Int) && (((LV2_Atom_Int*)oBu)->body > 0) && (((LV2_Atom_Int*)oBu)->body != host.beatUnit))
				{
					host.beatUnit = ((LV2_Atom_Int*)oBu)->body;
					scheduleNotifyStatus = true;
				}

				// Speed changed?
				if (oSpeed && (oSpeed->type == urids.atom_Float))
				{
					float nspeed = ((LV2_Atom_Float*)oSpeed)->body;

					if (nspeed != host.speed)
					{

						if (globalControllers[BASE] != SECONDS)
						{

							// Started ?
							if (speed == 0)
							{
								// TODO Clear all audio buffers
							}

							// Stopped ?
							if (nspeed == 0)
							{
								message.setMessage (JACK_STOP_MSG);
							}

							// Not stopped ?
							else message.deleteMessage (JACK_STOP_MSG);
						}

						host.speed = nspeed;
					}
				}

				// Bar position changed
				if (oBar && (oBar->type == urids.atom_Long) && (host.bar != ((uint64_t)((LV2_Atom_Long*)oBar)->body)))
				{
					host.bar = ((LV2_Atom_Long*)oBar)->body;
					scheduleUpdatePosition = true;
				}

				// Beat position changed (during playing) ?
				if (oBbeat && (oBbeat->type == urids.atom_Float) && (((LV2_Atom_Float*)oBbeat)->body != host.barBeat))
				{
					host.barBeat = ((LV2_Atom_Float*)oBbeat)->body;
					scheduleUpdatePosition = true;
				}

				if (scheduleUpdatePosition)
				{
					// Hard set new position if new data received
					if (globalControllers[PLAY_MODE] != AUTOPLAY)
					{
						Transport::operator= (host);
						double pos = getPositionFromBeats (barBeat + beatsPerBar * bar);
						double npos = floorfrac (pos - offset);
						if (int (npos * globalControllers[STEPS]) != int (position * globalControllers[STEPS])) scheduleNotifyStatus = true;
						position = npos;
						refFrame = ev->time.frames;
					}
				}

				if (scheduleStepsChanged && (globalControllers[PLAY_MODE] != AUTOPLAY)) stepsChanged();
			}
		}

		// TODO Read incoming MIDI events
		/* if (ev->body.type == urids.midi_Event)
		{
			const uint8_t* const msg = (const uint8_t*)(ev + 1);

			// Forward MIDI event
			if (controllers[MIDI_THRU] != 0.0f)
			{
				LV2_Atom midiatom;
				midiatom.type = urids.midi_Event;
				midiatom.size = ev->body.size;

				lv2_atom_forge_frame_time (&forge, ev->time.frames);
				lv2_atom_forge_raw (&forge, &midiatom, sizeof (LV2_Atom));
				lv2_atom_forge_raw (&forge, msg, midiatom.size);
				lv2_atom_forge_pad (&forge, sizeof (LV2_Atom) + midiatom.size);
			}

			// Analyze MIDI event
			if (controllers[MIDI_CONTROL] == 1.0f)
			{
				uint8_t typ = lv2_midi_message_type(msg);
				// uint8_t chn = msg[0] & 0x0F;
				uint8_t note = msg[1];
				uint32_t filter = controllers[MIDI_KEYS];

				switch (typ)
				{
					case LV2_MIDI_MSG_NOTE_ON:
					{
						if (filter & (1 << (note % 12)))
						{
							key = note;
							offset = floorfrac (position + offset);
							position = 0;
							refFrame = ev->time.frames;
						}
					}
					break;

					case LV2_MIDI_MSG_NOTE_OFF:
					{
						if (key == note)
						{
							key = 0xFF;
						}
					}
					break;

					case LV2_MIDI_MSG_CONTROLLER:
					{
						if ((note == LV2_MIDI_CTL_ALL_NOTES_OFF) ||
						    (note == LV2_MIDI_CTL_ALL_SOUNDS_OFF))
						{
							key = 0xFF;
						}
					}
					break;

					default: break;
				}
			}
		} */

		uint32_t next_t = (ev->time.frames < n_samples ? ev->time.frames : n_samples);
		play (last_t, next_t);
		last_t = next_t;
	}

	// Play remaining samples
	if (last_t < n_samples) play (last_t, n_samples);

	// Update position in case of no new barBeat submitted on next call
	double relpos = getPositionFromFrames (n_samples - refFrame);	// Position relative to reference frame
	double npos = floorfrac (position + relpos);
	if (int (npos * globalControllers[STEPS]) != int (position * globalControllers[STEPS])) scheduleNotifyStatus = true;
	position = npos;
	refFrame = 0;

	// Send collected data to GUI
	if (ui_on)
	{
		if (message.isScheduled ()) notifyMessageToGui ();
		if (scheduleNotifyStatus) notifyStatusToGui ();
		for (int i = 0; i < NR_SLOTS; ++i) {if (scheduleNotifySlot[i]) notifySlotToGui (i);}
	}
	lv2_atom_forge_pop (&forge, &notify_frame);
}

void BNoname01::stepsChanged ()
{
	double fpst = getFramesPerStep ();
	for (int i = 0; i < NR_SLOTS; ++i) slots[i].framesPerStep = fpst;

	if (!scheduleResizeBuffers)
	{
		scheduleResizeBuffers = true;
		LV2_Atom msg = {0, urids.bNoname01_allocateBuffers};
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (msg), &msg);
	}
}

void BNoname01::notifySlotToGui (const int slot)
{
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_slotEvent);
	lv2_atom_forge_key(&forge, urids.bNoname01_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bNoname01_pads);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, sizeof(Pad) / sizeof(float) * NR_STEPS, (void*) slots[slot].pads);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifySlot[slot] = false;
}

void BNoname01::notifyMessageToGui()
{
	uint32_t messageNr = message.loadMessage ();

	// Send notifications
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_messageEvent);
	lv2_atom_forge_key(&forge, urids.bNoname01_message);
	lv2_atom_forge_int(&forge, messageNr);
	lv2_atom_forge_pop(&forge, &frame);
}

void BNoname01::notifyStatusToGui()
{
	double pos = (globalControllers[PLAY] != PLAY_OFF ? floorfrac (position) * globalControllers[STEPS] : -1);
	// Send notifications
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_statusEvent);
	lv2_atom_forge_key(&forge, urids.time_beatsPerBar);
	lv2_atom_forge_float(&forge, beatsPerBar);
	lv2_atom_forge_key(&forge, urids.time_beatUnit);
	lv2_atom_forge_int(&forge, beatUnit);
	lv2_atom_forge_key(&forge, urids.time_beatsPerMinute);
	lv2_atom_forge_float(&forge, bpm);
	lv2_atom_forge_key(&forge, urids.bNoname01_position);
	lv2_atom_forge_double(&forge, pos);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifyStatus = false;
}

void BNoname01::play (uint32_t start, uint32_t end)
{
	if (end < start) return;

	// Return if halted or bpm == 0
	if (((speed == 0.0f) && (globalControllers[BASE] != SECONDS)) || (bpm < 1.0f))
	{
		memset(&audioOutput1[start], 0, (end - start) * sizeof(float));
		memset(&audioOutput2[start], 0, (end - start) * sizeof(float));
		return;
	}

	if (globalControllers[PLAY] == PLAY_OFF)
	{
		for (uint32_t i = start; i < end; ++i)
		{
			for (int j = 0; j < NR_SLOTS; ++j) slots[j].buffer->push_front (Stereo {audioInput1[i], audioInput2[i]});
		}
		memset(&audioOutput1[start], 0, (end - start) * sizeof(float));
		memset(&audioOutput2[start], 0, (end - start) * sizeof(float));
		return;
	}

	if (globalControllers[PLAY] == PLAY_BYPASS)
	{
		for (uint32_t i = start; i < end; ++i)
		{
			for (int j = 0; j < NR_SLOTS; ++j) slots[j].buffer->push_front (Stereo {audioInput1[i], audioInput2[i]});
		}
		if (audioOutput1 != audioInput1) memmove(&audioOutput1[start], &audioInput1[start], (end - start) * sizeof(float));
		if (audioOutput2 != audioInput2) memmove(&audioOutput2[start], &audioInput2[start], (end - start) * sizeof(float));
		return;
	}

	for (uint32_t i = start; i < end; ++i)
	{
		// Interpolate position within the loop
		double relpos = getPositionFromFrames (i - refFrame);	// Position relative to reference frame
		double pos = floorfrac (position + relpos);		// 0..1 position sequence
		double step = pos * globalControllers[STEPS];
		int iStep = LIMIT (step, 0, globalControllers[STEPS] - 1);

		Stereo input = Stereo {audioInput1[i], audioInput2[i]};
		Stereo output = input;

		for (int j = 0; j < NR_SLOTS; ++j)
		{
			input = output;
			Slot& iSlot = slots[j];
			iSlot.buffer->push_front (input);
			if ((iSlot.effect == FX_INVALID) || (iSlot.effect == FX_NONE)) break;

			// Next step ?
			if (oStep != iStep)
			{
				// Old pad ended?
				if (iSlot.getStart (oStep) != iSlot.getStart (iStep))
				{
					// Stop old pad
					iSlot.end ();

					// Start new pad (if set)
					iSlot.start (step);
				}
			}

			// Play music :-)
			output = (iSlot.params[SLOTS_PLAY] ? iSlot.play (step) : input);
		}

		oStep = iStep;
		audioOutput1[i] = output.left;
		audioOutput2[i] = output.right;
	}
}

LV2_State_Status BNoname01::state_save (LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{
	// Store pads
	char padDataString[0x8010] = "\nMatrix data:\n";

	for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
	{
		if ((slots[slotNr].effect == FX_NONE) || (slots[slotNr].effect == FX_INVALID)) continue;

		for (int stepNr = 0; stepNr < NR_STEPS; ++stepNr)
		{
			Pad& p = slots[slotNr].pads[stepNr];
			if ((p.gate > 0) && (p.size > 0) && (p.mix > 0))
			{
				char valueString[64];
				snprintf (valueString, 62, "sl:%d; st:%d; gt:%1.3f; sz:%d; mx:%1.3f", slotNr, stepNr, p.gate, int (p.size), p.mix);
				if ((slotNr < NR_SLOTS - 1) || (stepNr < NR_STEPS)) strcat (valueString, ";\n");
				else strcat(valueString, "\n");
				strcat (padDataString, valueString);
			}

		}
	}
	store (handle, urids.bNoname01_statePad, padDataString, strlen (padDataString) + 1, urids.atom_String, LV2_STATE_IS_POD);

	return LV2_STATE_SUCCESS;
}

LV2_State_Status BNoname01::state_restore (LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{
	size_t   size;
	uint32_t type;
	uint32_t valflags;
	const void* padData = retrieve(handle, urids.bNoname01_statePad, &size, &type, &valflags);

	if (padData && (type == urids.atom_String))
	{
		for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
		{
			Slot& s = slots[slotNr];
			std::fill (s.pads, s.pads + NR_STEPS, Pad());
		}

		std::string padDataString = (char*) padData;
		const std::string keywords[5] = {"sl:", "st:", "gt:", "sz:", "mx:"};

		// Restore pads
		// Parse retrieved data
		while (!padDataString.empty())
		{
			// Look for mandatory "sl:"
			int slotNr = -1;
			size_t strPos = padDataString.find (keywords[0]);
			size_t nextPos = 0;
			if (strPos == std::string::npos) break;	// No "sl:" found => end
			if (strPos + 3 > padDataString.length()) break;	// Nothing more after sl => end
			padDataString.erase (0, strPos + 3);
			try {slotNr = BUtilities::stof (padDataString, &nextPos);}
			catch  (const std::exception& e)
			{
				fprintf (stderr, "BGltchr.lv2: Restore pad state incomplete. Can't parse slot nr from \"%s...\"", padDataString.substr (0, 63).c_str());
				break;
			}

			if (nextPos > 0) padDataString.erase (0, nextPos);
			if ((slotNr < 0) || (slotNr >= NR_SLOTS))
			{
				fprintf (stderr, "BNoname01.lv2: Restore pad state incomplete. Invalid slot nr %i.\n", slotNr);
				break;
			}

			// Look for mandatory "st:"
			int stepNr = -1;
			strPos = padDataString.find (keywords[1]);
			nextPos = 0;
			if (strPos == std::string::npos) break;	// No "st:" found => end
			if (strPos + 3 > padDataString.length()) break;	// Nothing more after st => end
			padDataString.erase (0, strPos + 3);
			try {stepNr = BUtilities::stof (padDataString, &nextPos);}
			catch  (const std::exception& e)
			{
				fprintf (stderr, "BGltchr.lv2: Restore pad state incomplete. Can't parse step nr from \"%s...\"", padDataString.substr (0, 63).c_str());
				break;
			}

			if (nextPos > 0) padDataString.erase (0, nextPos);
			if ((stepNr < 0) || (stepNr >= NR_STEPS))
			{
				fprintf (stderr, "BNoname01.lv2: Restore pad state incomplete. Invalid step nr %i.\n", slotNr);
				break;
			}

			// Look for pad data
			for (int i = 2; i < 5; ++i)
			{
				nextPos = 0;
				strPos = padDataString.find (keywords[i]);
				if (strPos == std::string::npos) continue;	// Keyword not found => next keyword
				if (strPos + 3 >= padDataString.length())	// Nothing more after keyword => end
				{
					padDataString ="";
					break;
				}
				if (strPos > 0) padDataString.erase (0, strPos + 3);
				float val;
				try {val = BUtilities::stof (padDataString, &nextPos);}
				catch  (const std::exception& e)
				{
					fprintf (stderr, "BNoname01.lv2: Restore padstate incomplete. Can't parse %s from \"%s...\"",
							 keywords[i].substr(0,2).c_str(), padDataString.substr (0, 63).c_str());
					break;
				}

				if (nextPos > 0) padDataString.erase (0, nextPos);
				switch (i)
				{
					case 2:	slots[slotNr].pads[stepNr].gate = LIMIT (val, 0, 1);
						break;

					case 3:	slots[slotNr].pads[stepNr].size = LIMIT (val, 1, NR_STEPS - stepNr);
						break;

					case 4:	slots[slotNr].pads[stepNr].mix = LIMIT (val, 0, 1);
						break;

					default:break;
				}
			}
		}

		std::fill (scheduleNotifySlot, scheduleNotifySlot + NR_SLOTS, true);
	}

	return LV2_STATE_SUCCESS;
}

LV2_Worker_Status BNoname01::work (LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle, uint32_t size, const void* data)
{
	const LV2_Atom* atom = (const LV2_Atom*)data;

	// Free old buffers
        if (atom->type == urids.bNoname01_freeBuffers)
	{
		const Atom_BufferList* bAtom = (const Atom_BufferList*) data;
		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (bAtom->buffers[i]) delete (bAtom->buffers[i]);
		}
        }

	// Free old Fx
        if (atom->type == urids.bNoname01_freeFx)
	{
		const Atom_Fx* fAtom = (const Atom_Fx*) data;
		if (fAtom->fx) delete (fAtom->fx);
        }

	// Allocate new buffers
	else if (atom->type == urids.bNoname01_allocateBuffers)
	{
		//Required buffer size
		double fpst = getFramesPerStep ();
		size_t bSize = slots[0].buffer->size();

		if ((bSize < globalControllers[STEPS] * fpst) || (bSize > 2.0 * globalControllers[STEPS] * fpst))
		{
			Atom_BufferList bAtom;
			bAtom.atom = {sizeof (bAtom.buffers), urids.bNoname01_installBuffers};
			size_t nSize = 1.5 * globalControllers[STEPS] * fpst;

			for (int i = 0; i < NR_SLOTS; ++i)
			{
				try
				{
					RingBuffer<Stereo>* b = new RingBuffer<Stereo> (*slots[i].buffer);
					bAtom.buffers[i] = b;
					bAtom.buffers[i]->resize (nSize);
				}
		                catch (std::bad_alloc& ba)
				{
					fprintf (stderr, "BGlitch.lv2: Can't allocate enough memory to resize audio buffers.\n");
					//message.setMessage (MEMORY_ERR);
					return LV2_WORKER_ERR_NO_SPACE;
				}
			}

			respond (handle, sizeof (bAtom) , &bAtom);
		}

		// Resize not needed
		else scheduleResizeBuffers = false;
	}

	else if (atom->type == urids.bNoname01_allocateFx)
	{
		const LV2_Atom_Int* iAtom = (const LV2_Atom_Int*)data;
		int slotNr = iAtom->body;

		BNoname01EffectsIndex effect = (BNoname01EffectsIndex) *new_controllers[SLOTS + slotNr * (SLOTS_PARAMS + NR_PARAMS)];
		Fx* fx = slots[slotNr].newFx (effect);
		Atom_Fx fAtom = {{sizeof (int) + sizeof (BNoname01EffectsIndex) + sizeof (Fx*), urids.bNoname01_installFx}, slotNr, effect, fx};
		respond (handle, sizeof (fAtom) , &fAtom);
	}

	else return LV2_WORKER_ERR_UNKNOWN;

	return LV2_WORKER_SUCCESS;
}

LV2_Worker_Status BNoname01::work_response (uint32_t size, const void* data)
{
	const LV2_Atom* atom = (const LV2_Atom*)data;

	if (atom->type == urids.bNoname01_installBuffers)
	{
		// Schedule worker to free old buffers
		Atom_BufferList bAtom;
		bAtom.atom = {sizeof (bAtom.buffers), urids.bNoname01_freeBuffers};
		for (int i = 0; i < NR_SLOTS; ++i) bAtom.buffers[i] = slots[i].buffer;
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (bAtom), &bAtom);

		// Install new buffer
		const Atom_BufferList* nAtom = (const Atom_BufferList*) data;
		for (int i = 0; i < NR_SLOTS; ++i) slots[i].buffer = nAtom->buffers[i];
		scheduleResizeBuffers = false;
	}

	else if (atom->type == urids.bNoname01_installFx)
	{
		const Atom_Fx* nAtom = (const Atom_Fx*) data;

		// Schedule worker to free old Fx
		Atom_Fx fAtom;
		fAtom = {{sizeof (int) + sizeof (BNoname01EffectsIndex) + sizeof (Fx*), urids.bNoname01_freeFx}, nAtom->index, slots[nAtom->index].effect, slots[nAtom->index].fx};
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (fAtom), &fAtom);

		// Install new Fx
		slots[nAtom->index].fx = nAtom->fx;
		slots[nAtom->index].effect = BNoname01EffectsIndex (nAtom->effect);
		scheduleSetFx[nAtom->index] = false;
	}

	return LV2_WORKER_SUCCESS;
}

LV2_Handle instantiate (const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
	// New instance
	BNoname01* instance;
	try {instance = new BNoname01(samplerate, features);}
	catch (std::exception& exc)
	{
		fprintf (stderr, "BNoname01.lv2: Plugin instantiation failed. %s\n", exc.what ());
		return NULL;
	}

	if (!instance)
	{
		fprintf(stderr, "BNoname01.lv2: Plugin instantiation failed.\n");
		return NULL;
	}

	if (!instance->map)
	{
		fprintf(stderr, "BNoname01.lv2: Host does not support urid:map.\n");
		delete (instance);
		return NULL;
	}

	return (LV2_Handle)instance;
}

void connect_port (LV2_Handle instance, uint32_t port, void *data)
{
	BNoname01* inst = (BNoname01*) instance;
	inst->connect_port (port, data);
}

void run (LV2_Handle instance, uint32_t n_samples)
{
	BNoname01* inst = (BNoname01*) instance;
	inst->run (n_samples);
}

void cleanup (LV2_Handle instance)
{
	BNoname01* inst = (BNoname01*) instance;
	delete inst;
}

static LV2_State_Status state_save(LV2_Handle instance, LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BNoname01* inst = (BNoname01*)instance;
	if (!inst) return LV2_STATE_SUCCESS;

	inst->state_save (store, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static LV2_State_Status state_restore(LV2_Handle instance, LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BNoname01* inst = (BNoname01*)instance;
	inst->state_restore (retrieve, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static LV2_Worker_Status work (LV2_Handle instance, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle,
	uint32_t size, const void* data)
{
	BNoname01* inst = (BNoname01*)instance;
	if (!inst) return LV2_WORKER_SUCCESS;

	return inst->work (respond, handle, size, data);
}

static LV2_Worker_Status work_response (LV2_Handle instance, uint32_t size,  const void* data)
{
	BNoname01* inst = (BNoname01*)instance;
	if (!inst) return LV2_WORKER_SUCCESS;

	return inst->work_response (size, data);
}

static const void* extension_data(const char* uri)
{
	static const LV2_State_Interface  state  = {state_save, state_restore};
	static const LV2_Worker_Interface worker = {work, work_response, NULL};
	if (!strcmp(uri, LV2_STATE__interface)) return &state;
	if (!strcmp(uri, LV2_WORKER__interface)) return &worker;
	return NULL;
}

const LV2_Descriptor descriptor =
{
	BNONAME01_URI,
	instantiate,
	connect_port,
	NULL, //activate,
	run,
	NULL, //deactivate,
	cleanup,
	extension_data
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	switch (index)
	{
	case 0: return &descriptor;
	default: return NULL;
	}
}
