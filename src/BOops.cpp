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

#include <cstdio>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "lv2/core/lv2_util.h"
#include "BOops.hpp"
#include "ControllerLimits.hpp"
#include "BUtilities/stof.hpp"
#include "getURIs.hpp"

#ifndef SF_FORMAT_MP3
#ifndef MINIMP3_IMPLEMENTATION
#define MINIMP3_IMPLEMENTATION
#endif
#endif
#include "Sample.hpp"

#ifndef LIMIT
#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))
#endif /* LIMIT */

inline double floorfrac (const double value) {return value - floor (value);}


BOops::BOops (double samplerate, const char* bundle_path, const LV2_Feature* const* features) :
	map(NULL), workerSchedule (NULL), pluginPath {0}, urids (),
	host {samplerate, 120.0f, 1.0f, 0ul, 0.0f, 4.0f, 4},
	activated (false),
	positions (),
	transportGateKeys {false},
	controlPort(NULL), notifyPort(NULL),
	audioInput1(NULL), audioInput2(NULL), audioOutput1(NULL), audioOutput2(NULL),
	new_controllers {NULL}, globalControllers {0},
	forge (), notify_frame (),
	sample (NULL), sampleAmp (1.0f),
	waveform {0}, waveformCounter (0), lastWaveformCounter (0),
	message (), ui_on(false), scheduleNotifySlot {false},
	scheduleNotifyStatus (false), scheduleResizeBuffers (false), scheduleSetFx {false},
	scheduleNotifyWaveformToGui (false), scheduleNotifyTransportGateKeys (false),
	scheduleNotifySamplePathToGui (false)

{
	if (bundle_path) strncpy (pluginPath, bundle_path, 1023);

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
	std::fill (slots, slots + NR_SLOTS, Slot (this, FX_NONE, nullptr, nullptr, 16, 1.0f, 0.25 * samplerate));

	// Initialize positions
	positions.clear();
	positions.push_back ({0.0, 0, 0.0, 0, {samplerate, 120.0f, 1.0f, 0ul, 0.0f, 4.0f, 4}, 1.0, true});
}

BOops::~BOops ()
{
	if (sample) delete sample;
}

void BOops::connect_port(uint32_t port, void *data)
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

double BOops::getPositionFromBeats (const Transport& transport, const double beats)
{
	if (globalControllers[BASE_VALUE] == 0.0) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS: 	return (transport.bpm ? beats / (globalControllers[BASE_VALUE] * (transport.bpm / 60.0)) : 0.0);
		case BEATS:	return beats / globalControllers[BASE_VALUE];
		case BARS:	return (transport.beatsPerBar ? beats / (globalControllers[BASE_VALUE] * transport.beatsPerBar) : 0.0);
		default:	return 0.0;
	}
}

double BOops::getPositionFromFrames (const Transport& transport, const uint64_t frames)
{
	if ((globalControllers[BASE_VALUE] == 0.0) || (transport.rate == 0)) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS: 	return frames * (1.0 / transport.rate) / globalControllers[BASE_VALUE] ;
		case BEATS:	return (transport.bpm ? frames * (transport.speed / (transport.rate / (transport.bpm / 60))) / globalControllers[BASE_VALUE] : 0.0);
		case BARS:	return
				(
					transport.bpm && transport.beatsPerBar ?
					frames * (transport.speed / (transport.rate / (transport.bpm / 60))) / (globalControllers[BASE_VALUE] * transport.beatsPerBar) :
					0.0
				);
		default:	return 0.0;
	}
}

uint64_t BOops::getFramesFromPosition (const Transport& transport, const double position) const
{
	if (transport.bpm < 1.0) return 0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS :	return position * transport.rate * globalControllers[BASE_VALUE];
		case BEATS:	return position * (60.0 / transport.bpm) * transport.rate * globalControllers[BASE_VALUE];
		case BARS:	return position * transport.beatsPerBar * (60.0 / transport.bpm) * transport.rate * globalControllers[BASE_VALUE];
		default:	return 0;
	}
}

double BOops::getPositionFromSeconds (const Transport& transport, const double seconds)
{
	if (globalControllers[BASE_VALUE] == 0.0) return 0.0;

	switch (int (globalControllers[BASE]))
	{
		case SECONDS :	return seconds / globalControllers[BASE_VALUE];
		case BEATS:	return seconds * (transport.bpm / 60.0) / globalControllers[BASE_VALUE];
		case BARS:	return (transport.beatsPerBar ? seconds * (transport.bpm / 60.0 / transport.beatsPerBar) / globalControllers[BASE_VALUE] : 0.0);
		default:	return 0;
	}
}

double BOops::getFramesPerStep (const Transport& transport)
{
	double bps = (globalControllers[PLAY_MODE] == 0 ? globalControllers[AUTOPLAY_BPM] : transport.bpm) / 60.0;
	double bpB = (globalControllers[PLAY_MODE] == 0 ? globalControllers[AUTOPLAY_BPB] :transport. beatsPerBar);
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
	return transport.rate * s / globalControllers[STEPS];
}

void BOops::activate() {activated = true;}

void BOops::deactivate() {activated = false;}

void BOops::run (uint32_t n_samples)
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
			float oldValue = globalControllers[i];
			globalControllers[i] = newValue;

			if (i == PLAY_MODE)
			{
				Position np = positions.back();

				switch (int (newValue))
				{
					case AUTOPLAY:
						np.playing = true;
						np.transport.bpm = globalControllers[AUTOPLAY_BPM];
						np.transport.speed = 1.0;
						np.transport.beatsPerBar = globalControllers[AUTOPLAY_BPB];
						break;

					case HOST_CONTROLLED:
						np.playing = true;
						np.transport = host;
						break;

					default:
						np.playing = false;
						np.transport = host;
				}

				positions.push_back (np);

				resizeSteps ();
			}

			else if (i == STEPS)
			{
				for (int i = 0; i < NR_SLOTS; ++i) slots[i].size = newValue;
				resizeSteps ();
			}

			else if (i == AUTOPLAY_BPM)
			{
				if (globalControllers[PLAY_MODE] == AUTOPLAY) positions.back().transport.bpm = globalControllers[AUTOPLAY_BPM];
				resizeSteps ();
			}

			else if (i == AUTOPLAY_BPB)
			{
				if (globalControllers[PLAY_MODE] == AUTOPLAY) positions.back().transport.beatsPerBar = globalControllers[AUTOPLAY_BPB];
				resizeSteps ();
			}

			else if (i == AUTOPLAY_POSITION)
			{
				Position np = positions.back();
				np.position = floorfrac (np.position + 1.0 + newValue - oldValue);
				np.refFrame = 0;
				positions.push_back (np);
				scheduleNotifyStatus = true;
			}

			else if
			((i == BASE) || (i == BASE_VALUE)) resizeSteps ();
		}
	}

	for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
	{
		if (!scheduleSetFx[slotNr])
		{
			if (slots[slotNr].effect != *new_controllers[SLOTS + slotNr * (SLOTS_PARAMS + NR_PARAMS)])
			{
				LV2_Atom_Int msg = {{sizeof (int), urids.bOops_allocateFx}, slotNr};
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
			if (obj->body.otype == urids.bOops_uiOn)
			{
				ui_on = true;
				std::fill (scheduleNotifySlot, scheduleNotifySlot + NR_SLOTS, true);
				std::fill (scheduleNotifyShape, scheduleNotifyShape + NR_SLOTS, true);
				scheduleNotifyTransportGateKeys = true;
				scheduleNotifySamplePathToGui = true;
			}

			// Process GUI off status data
			else if (obj->body.otype == urids.bOops_uiOff) ui_on = false;

			// Process transportGateKey data
			else if (obj->body.otype == urids.bOops_transportGateKeyEvent)
			{
				LV2_Atom *oKeys = NULL;
				lv2_atom_object_get (obj,
						     urids.bOops_transportGateKeys, &oKeys,
						     NULL);

				if (oKeys && (oKeys->type == urids.atom_Vector))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oKeys;
					if (vec->body.child_type == urids.atom_Int)
					{
						const int keysize = LIMIT ((int) ((oKeys->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (int)), 0, NR_PIANO_KEYS);
						const int* keys = (int*) (&vec->body + 1);
						std::fill (transportGateKeys, transportGateKeys + NR_PIANO_KEYS, false);
						for (int i = 0; i < keysize; ++i)
						{
							int keyNr = keys[i];
							if ((keyNr >=0) && (keyNr < NR_PIANO_KEYS)) transportGateKeys[keyNr] = true;
						}
						scheduleNotifyTransportGateKeys = true;
					}
				}
			}

			// Process slot pads data
			else if (obj->body.otype == urids.bOops_slotEvent)
			{
				LV2_Atom *oSl = NULL, *oPd = NULL;
				int slot = -1;
				lv2_atom_object_get (obj,
					 	     urids.bOops_slot, &oSl,
						     urids.bOops_pads, &oPd,
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
						for (unsigned int i = 0; (i < size) && (i < NR_STEPS); ++i) slots[slot].setPad (i, pad[i]);
					}
				}
			}

			// Process single pad data
			else if (obj->body.otype == urids.bOops_padEvent)
			{
				LV2_Atom *oSl = NULL, *oSt = NULL, *oPd = NULL;
				int slot = -1;
				int step = -1;
				lv2_atom_object_get (obj,
					 	     urids.bOops_slot, &oSl,
						     urids.bOops_step, &oSt,
						     urids.bOops_pads, &oPd,
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
							slots[slot].setPad (step, *pad);
						}
					}
				}
			}

			// Process slot shape data
			else if (obj->body.otype == urids.bOops_shapeEvent)
			{
				LV2_Atom *oSl = NULL, *oSh = NULL;
				int slot = -1;
				lv2_atom_object_get (obj,
						     urids.bOops_slot, &oSl,
						     urids.bOops_shapeData, &oSh,
						     NULL);

				// Slot nr notification
				if (oSl && (oSl->type == urids.atom_Int) && (((LV2_Atom_Int*)oSl)->body >= 0) && (((LV2_Atom_Int*)oSl)->body < NR_SLOTS))
				{
					slot = ((LV2_Atom_Int*)oSl)->body;
				}

				// Shape notification
				if (oSh && (oSh->type == urids.atom_Vector) && (slot >= 0))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oSh;
					if (vec->body.child_type == urids.atom_Float)
					{
						slots[slot].shape.clearShape ();
						const uint32_t vecSize = (uint32_t) ((oSh->size - sizeof(LV2_Atom_Vector_Body)) / (7 * sizeof (float)));
						float* data = (float*) (&vec->body + 1);
						for (unsigned int i = 0; (i < vecSize) && (i < SHAPE_MAXNODES); ++i)
						{
							Node node;
							node.nodeType = NodeType (int (data[i * 7]));
							node.point.x = data[i * 7 + 1];
							node.point.y = data[i * 7 + 2];
							node.handle1.x = data[i * 7 + 3];
							node.handle1.y = data[i * 7 + 4];
							node.handle2.x = data[i * 7 + 5];
							node.handle2.y = data[i * 7 + 6];
							slots[slot].shape.appendNode (node);
						}
						slots[slot].shape.validateShape();
						scheduleNotifyShape[slot] = true;
					}
				}
			}

			// Sample path notification -> forward to worker
			else if (obj->body.otype ==urids.bOops_samplePathEvent)
			{
				const LV2_Atom* oPath = NULL, *oStart = NULL, *oEnd = NULL, *oAmp = NULL, *oLoop = NULL;
				lv2_atom_object_get
				(
					obj,
					urids.bOops_samplePath, &oPath,
					urids.bOops_sampleStart, &oStart,
					urids.bOops_sampleEnd, &oEnd,
					urids.bOops_sampleAmp, &oAmp,
					urids.bOops_sampleLoop, &oLoop,
					0
				);

				// New sample
				if (oPath && (oPath->type == urids.atom_Path))
				{
					workerSchedule->schedule_work (workerSchedule->handle, lv2_atom_total_size((LV2_Atom*)obj), obj);
				}

				// Only start / end /amp / loop changed
				else if (sample)
				{
					if (oStart && (oStart->type == urids.atom_Long)) sample->start = LIMIT (((LV2_Atom_Long*)oStart)->body, 0, sample->info.frames - 1);
					if (oEnd && (oEnd->type == urids.atom_Long)) sample->end = LIMIT (((LV2_Atom_Long*)oEnd)->body, 0, sample->info.frames);
					if (oAmp && (oAmp->type == urids.atom_Float)) sampleAmp = LIMIT (((LV2_Atom_Float*)oAmp)->body, 0.0f, 1.0f);
					if (oLoop && (oLoop->type == urids.atom_Bool)) sample->loop = bool (((LV2_Atom_Bool*)oLoop)->body);
				}
			}

			// Process time / position data
			else if (obj->body.otype == urids.time_Position)
			{
				bool scheduleUpdatePosition = false;
				bool scheduleResizeSteps = false;

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

						scheduleResizeSteps = true;
					}
				}

				// Beats per bar changed?
				if (oBpb && (oBpb->type == urids.atom_Float) && (((LV2_Atom_Float*)oBpb)->body > 0) && (((LV2_Atom_Float*)oBpb)->body != host.beatsPerBar))
				{
					host.beatsPerBar = ((LV2_Atom_Float*)oBpb)->body;
					scheduleNotifyStatus = true;
					scheduleResizeSteps = true;
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
							// TODO Clear all audio buffers
							// if (speed == 0)
							// {
							//
							// }

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

				// Bar or beat position data changed in host or midi controlled mode: fade
				if (scheduleUpdatePosition)
				{
					if (globalControllers[PLAY_MODE] != AUTOPLAY)
					{
						Position np = positions.back();
						double pos = getPositionFromBeats (host, host.barBeat + host.beatsPerBar * host.bar);
						double npos = floorfrac (pos - np.offset);

						// Fade only if jump > 1 ms
						if (fabs (npos - positions.back().position) > getPositionFromSeconds (host, 0.001))
						{
							np.fader = 0.0;
							np.transport = host;
							np.position = npos;
							np.step = npos * npos * globalControllers[STEPS];
							np.refFrame = ev->time.frames;
							if (np.step != positions.back().step) scheduleNotifyStatus = true;

							positions.push_back (np);
						}

						// Otherwise update at least the transport data
						else positions.back().transport = host;
					}
				}

				// Other data changed in host or midi controlled mode: copy
				else if (globalControllers[PLAY_MODE] != AUTOPLAY) positions.back().transport = host;

				if (scheduleResizeSteps && (globalControllers[PLAY_MODE] != AUTOPLAY)) resizeSteps();
			}
		}

		// Read incoming MIDI events
		if (ev->body.type == urids.midi_Event)
		{
			const uint8_t* const msg = (const uint8_t*)(ev + 1);

			// Analyze MIDI event
			if (globalControllers[PLAY_MODE] == MIDI_CONTROLLED)
			{
				const uint8_t typ = lv2_midi_message_type(msg);
				// uint8_t chn = msg[0] & 0x0F;
				const uint8_t note = msg[1];
				const bool isTransportGateKey = ((note < NR_PIANO_KEYS) && transportGateKeys[note]);


				switch (typ)
				{
					case LV2_MIDI_MSG_NOTE_ON:
					{
						if (isTransportGateKey)
						{
							Position p = positions.back();

							switch (int (globalControllers[ON_MIDI]))
							{
								case 0:	// Restart
									p.offset = floorfrac (p.position + p.offset);
									p.position = 0;
									p.refFrame = ev->time.frames;
									break;

								case 2: // Restart & sync
									{
										double steppos = fmod (p.position, 1.0 / double (globalControllers[STEPS]));
										p.offset = floorfrac (1.0 + p.position + p.offset - steppos);
										p.position = steppos;
										p.refFrame = ev->time.frames;
									}
									break;

								default:// Continue
									break;
							}

							p.playing = true;
							positions.push_back (p);
							scheduleNotifyStatus = true;
						}
					}
					break;

					case LV2_MIDI_MSG_NOTE_OFF:
					{
						if (isTransportGateKey)
						{
							Position p = positions.back();
							p.playing = false;
							positions.push_back (p);
							scheduleNotifyStatus = true;
						}
					}
					break;

					case LV2_MIDI_MSG_CONTROLLER:
					{
						if ((note == LV2_MIDI_CTL_ALL_NOTES_OFF) ||
						    (note == LV2_MIDI_CTL_ALL_SOUNDS_OFF))
						{
							Position p = positions.back();
							p.playing = false;
							positions.push_back (p);
							scheduleNotifyStatus = true;
						}
					}
					break;

					default: break;
				}
			}
		}

		uint32_t next_t = (ev->time.frames < n_samples ? ev->time.frames : n_samples);
		play (last_t, next_t);
		last_t = next_t;
	}

	// Play remaining samples
	if (last_t < n_samples) play (last_t, n_samples);

	// Update position in case of no new barBeat submitted on next call
	for (unsigned int i = 0; i < positions.size; ++i)
	{
		const uint64_t diff = n_samples - positions[i].refFrame;
		const double relpos = getPositionFromFrames (positions[i].transport, diff);	// Position relative to reference frame
		const double npos = floorfrac (positions[i].position + relpos);
		const int nstep = LIMIT (npos * globalControllers[STEPS], 0, globalControllers[STEPS] - 1);
		if (nstep != int (positions[i].position * globalControllers[STEPS])) scheduleNotifyStatus = true;
		positions[i].position = npos;
		positions[i].refFrame = 0;

		if (i < positions.size - 1) positions[i].fader -= diff / (FADINGTIME * positions[i].transport.rate);
		else if (positions[i].fader + diff / (FADINGTIME * positions.back().transport.rate) < 1.0) positions[i].fader += diff / (0.01 * positions.back().transport.rate);
		else positions[i].fader = 1.0;
	}

	// Cleanup position data
	for (Position** p = positions.begin(); (p < positions.end()) && (positions.size > 1); )
	{
		if ((**p).fader <= 0) p = positions.erase (p);
		else ++p;
	}

	// Keep at least one free positions
	if (positions.size >= MAXFADERS - 1)
	{
		Position** killPos = positions.begin();
		for (Position** p = positions.begin(); p < positions.end(); ++p)
		{
			if ((**p).fader < (**killPos).fader) killPos = p;
		}
		positions.erase (killPos);
	}

	// Send collected data to GUI
	if (waveformCounter != lastWaveformCounter) scheduleNotifyWaveformToGui = true;
	if (ui_on)
	{
		if (message.isScheduled ()) notifyMessageToGui ();
		if (scheduleNotifyStatus) notifyStatusToGui ();
		for (int i = 0; i < NR_SLOTS; ++i) {if (scheduleNotifySlot[i]) notifySlotToGui (i);}
		for (int i = 0; i < NR_SLOTS; ++i) {if (scheduleNotifyShape[i]) notifyShapeToGui (i);}
		if (scheduleNotifyTransportGateKeys) notifyTransportGateKeysToGui();
		if (scheduleNotifyWaveformToGui) notifyWaveformToGui (lastWaveformCounter, waveformCounter);
		if (scheduleNotifySamplePathToGui) notifySamplePathToGui();
	}
	lv2_atom_forge_pop (&forge, &notify_frame);
}

void BOops::resizeSteps ()
{
	double fpst = getFramesPerStep (positions.back().transport);
	for (int i = 0; i < NR_SLOTS; ++i) slots[i].framesPerStep = fpst;

	if (!scheduleResizeBuffers)
	{
		scheduleResizeBuffers = true;
		LV2_Atom msg = {0, urids.bOops_allocateBuffers};
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (msg), &msg);
	}
}

void BOops::notifySlotToGui (const int slot)
{
	Pad pads[NR_STEPS];
	for (unsigned int i = 0; i < NR_STEPS; ++i) pads[i] = slots[slot].getPad (i);

	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	forgePads (&forge, &frame, slot, pads, NR_STEPS);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifySlot[slot] = false;
}

void BOops::notifyShapeToGui (const int slot)
{
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	forgeShape (&forge, &frame, slot, &slots[slot].shape);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifyShape[slot] = false;
}

void BOops::notifyMessageToGui()
{
	uint32_t messageNr = message.loadMessage ();

	// Send notifications
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_messageEvent);
	lv2_atom_forge_key(&forge, urids.bOops_message);
	lv2_atom_forge_int(&forge, messageNr);
	lv2_atom_forge_pop(&forge, &frame);
}

void BOops::notifyStatusToGui()
{
	Position& p = positions.back();
	double pos =
	(
		(globalControllers[PLAY] != PLAY_OFF) && p.playing && ((p.transport.speed != 0.0f) || (globalControllers[BASE] == SECONDS)) && (p.transport.bpm >= 1.0f)?
		floorfrac (p.position) * globalControllers[STEPS] :
		-1
	);
	// Send notifications
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_statusEvent);
	lv2_atom_forge_key(&forge, urids.time_beatsPerBar);
	lv2_atom_forge_float(&forge, p.transport.beatsPerBar);
	lv2_atom_forge_key(&forge, urids.time_beatUnit);
	lv2_atom_forge_int(&forge, p.transport.beatUnit);
	lv2_atom_forge_key(&forge, urids.time_beatsPerMinute);
	lv2_atom_forge_float(&forge, p.transport.bpm);
	lv2_atom_forge_key(&forge, urids.bOops_position);
	lv2_atom_forge_double(&forge, pos);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifyStatus = false;
}

void BOops::notifyWaveformToGui (const int start, const int end)
{
	int p1 = (start <= end ? end : WAVEFORMSIZE - 1);

	// Notify shapeBuffer (position to end)
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);
	lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_waveformEvent);
	lv2_atom_forge_key(&forge, urids.bOops_waveformStart);
	lv2_atom_forge_int(&forge, start);
	lv2_atom_forge_key(&forge, urids.bOops_waveformData);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, (uint32_t) (p1 + 1 - start), &waveform[start]);
	lv2_atom_forge_pop(&forge, &frame);

	// Additional notification if position exceeds end
	if (start > waveformCounter)
	{
		LV2_Atom_Forge_Frame frame;
		lv2_atom_forge_frame_time(&forge, 0);
		lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_waveformEvent);
		lv2_atom_forge_key(&forge, urids.bOops_waveformStart);
		lv2_atom_forge_int(&forge, 0);
		lv2_atom_forge_key(&forge, urids.bOops_waveformData);
		lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, (uint32_t) (end), &waveform[0]);
		lv2_atom_forge_pop(&forge, &frame);
	}

	scheduleNotifyWaveformToGui = false;
	lastWaveformCounter = end;
}

void BOops::notifyTransportGateKeysToGui()
{
	// Create buffer
	int keys[NR_PIANO_KEYS];
	int keysize = 0;
	std::fill (keys, keys + NR_PIANO_KEYS, 0);
	for (int i = 0; i < NR_PIANO_KEYS; ++i)
	{
		if (transportGateKeys[i])
		{
			keys[keysize] = i;
			++keysize;
		}
	}

	// Send notifications
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time (&forge, 0);
	forgeTransportGateKeys (&forge, &frame, keys, keysize);
	lv2_atom_forge_pop(&forge, &frame);

	scheduleNotifyTransportGateKeys = false;
}

void BOops::notifySamplePathToGui ()
{
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_frame_time(&forge, 0);

	if (sample && sample->path && (sample->path[0] != 0)) forgeSamplePath (&forge, &frame, sample->path, sample->start, sample->end, sampleAmp, int32_t (sample->loop));
	else
	{
		const char* path = ".";
		forgeSamplePath (&forge, &frame, path, 0, 0, sampleAmp, false);
	}

	lv2_atom_forge_pop(&forge, &frame);
	scheduleNotifySamplePathToGui = false;
}

LV2_Atom_Forge_Ref BOops::forgeSamplePath (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const char* path, const int64_t start, const int64_t end, const float amp, const int32_t loop)
{
	const LV2_Atom_Forge_Ref msg = lv2_atom_forge_object (forge, frame, 0, urids.bOops_samplePathEvent);
	if (msg)
	{
		lv2_atom_forge_key (forge, urids.bOops_samplePath);
		lv2_atom_forge_path (forge, path, strlen (path) + 1);
		lv2_atom_forge_key (forge, urids.bOops_sampleStart);
		lv2_atom_forge_long (forge, start);
		lv2_atom_forge_key (forge, urids.bOops_sampleEnd);
		lv2_atom_forge_long (forge, end);
		lv2_atom_forge_key (forge, urids.bOops_sampleAmp);
		lv2_atom_forge_float (forge, amp);
		lv2_atom_forge_key (forge, urids.bOops_sampleLoop);
		lv2_atom_forge_bool (forge, loop);
	}
	return msg;
}

LV2_Atom_Forge_Ref BOops::forgeTransportGateKeys (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int* keys, const size_t size)
{
	const LV2_Atom_Forge_Ref msg = lv2_atom_forge_object (forge, frame, 0, urids.bOops_transportGateKeyEvent);
	if (msg)
	{
		lv2_atom_forge_key (forge, urids.bOops_transportGateKeys);
		lv2_atom_forge_vector (forge, sizeof(int), urids.atom_Int, size, (void*) keys);
	}
	return msg;
}

LV2_Atom_Forge_Ref  BOops::forgeShape (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int slot, const Shape<SHAPE_MAXNODES>* shape)
{
	float nodes[SHAPE_MAXNODES][7];
	for (unsigned int i = 0; i < shape->size(); ++i)
	{
		Node n = shape->getNode (i);
		nodes[i][0] = n.nodeType;
		nodes[i][1] = n.point.x;
		nodes[i][2] = n.point.y;
		nodes[i][3] = n.handle1.x;
		nodes[i][4] = n.handle1.y;
		nodes[i][5] = n.handle2.x;
		nodes[i][6] = n.handle2.y;
	}

	const LV2_Atom_Forge_Ref msg = lv2_atom_forge_object (forge, frame, 0, urids.bOops_shapeEvent);
	if (msg)
	{
		lv2_atom_forge_key(forge, urids.bOops_slot);
		lv2_atom_forge_int(forge, slot);
		lv2_atom_forge_key(forge, urids.bOops_shapeData);
		lv2_atom_forge_vector(forge, sizeof(float), urids.atom_Float, 7 * shape->size(), nodes);
	}
	return msg;
}

LV2_Atom_Forge_Ref BOops::forgePads (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int slot, const Pad* pads, const size_t size)
{
	const LV2_Atom_Forge_Ref msg = lv2_atom_forge_object(forge, frame, 0, urids.bOops_slotEvent);
	if (msg)
	{
		lv2_atom_forge_key(forge, urids.bOops_slot);
		lv2_atom_forge_int(forge, slot);
		lv2_atom_forge_key(forge, urids.bOops_pads);
		lv2_atom_forge_vector(forge, sizeof(float), urids.atom_Float, sizeof(Pad) / sizeof(float) * NR_STEPS, (void*) pads);
	}
	return msg;
}

void BOops::play (uint32_t start, uint32_t end)
{
	if (end < start) return;

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
			Position& p = positions.back();
			double relpos = getPositionFromFrames (p.transport, i - p.refFrame);	// Position relative to reference frame
			double pos = floorfrac (p.position + relpos);				// 0..1 position sequence

			// Input signal
			Stereo input = Stereo (audioInput1[i], audioInput2[i]);
			if (globalControllers[SOURCE] == SOURCE_SAMPLE)
			{
				if (sample)
				{
					if (sample->end <= sample->start) input = Stereo();

					else
					{
						const uint64_t f0 = getFramesFromPosition (p.transport, pos);
						const int64_t frame =
						(
							sample->loop ?
							(f0  % (sample->end - sample->start)) + sample->start :
							f0 + sample->start
						);

						if (frame < sample->end) input = Stereo (sample->get (frame, 0, host.rate), sample->get (frame, 1, host.rate)) * sampleAmp;
						else input = Stereo();
					}
				}
				else input = Stereo();
			}

			// Load samples to buffer
			for (int j = 0; j < NR_SLOTS; ++j) slots[j].buffer->push_front (input);

			// Waveform
			waveformCounter = int (pos * WAVEFORMSIZE) % WAVEFORMSIZE;
			waveform[waveformCounter] = (input.left + input.right) / 2;

			// Bypass to output
			audioOutput1[i] = input.left;
			audioOutput2[i] = input.right;
		}

		return;
	}

	for (uint32_t i = start; i < end; ++i)
	{
		double sumFaders = 0;

		for (unsigned int j = 0; j < positions.size; ++j)
		{
			Position& p = positions[j];
			double fader =
			(
				j < positions.size - 1 ?
				p.fader - (j - p.refFrame) / (FADINGTIME * p.transport.rate) :
				p.fader + (j - p.refFrame) / (FADINGTIME * p.transport.rate)
			);
			fader = LIMIT (fader, 0.0, 1.0);

			// Interpolate position within the loop
			double relpos = getPositionFromFrames (p.transport, i - p.refFrame);	// Position relative to reference frame
			double pos = floorfrac (p.position + relpos);				// 0..1 position sequence

			// Input
			Stereo input = Stereo (audioInput1[i], audioInput2[i]);
			if (globalControllers[SOURCE] == SOURCE_SAMPLE)
			{
				if (sample)
				{
					if (sample->end <= sample->start) input = Stereo();

					else
					{
						const uint64_t f0 = getFramesFromPosition (p.transport, pos);
						const int64_t frame =
						(
							sample->loop ?
							(f0  % (sample->end - sample->start)) + sample->start :
							f0 + sample->start
						);

						if (frame < sample->end) input = Stereo (sample->get (frame, 0, host.rate), sample->get (frame, 1, host.rate)) * sampleAmp;
						else input = Stereo();
					}
				}
				else input = Stereo();
			}
			Stereo output = input;

			// Waveform
			if (j == positions.size - 1)
			{
				waveformCounter = int (pos * WAVEFORMSIZE) % WAVEFORMSIZE;
				waveform[waveformCounter] = (input.left + input.right) / 2;
			}

			if
			(
				(p.playing) &&
				((p.transport.speed != 0.0f) || (globalControllers[BASE] == SECONDS)) &&
				(p.transport.bpm >= 1.0f)
			)
			{
				double step = pos * globalControllers[STEPS];
				int iStep = LIMIT (step, 0, globalControllers[STEPS] - 1);

				for (int k = 0; k < NR_SLOTS; ++k)
				{
					input = output;
					Slot& iSlot = slots[k];
					iSlot.buffer->push_front (input);
					if ((iSlot.effect == FX_INVALID) || (iSlot.effect == FX_NONE)) break;

					// Next step ?
					if (positions[j].step != iStep)
					{
						// Old pad ended?
						const int iStart = iSlot.startPos[iStep];
						if (iSlot.startPos[positions[j].step] != iStart)
						{
							// Stop old pad
							iSlot.end ();

							// Start new pad (if set)
							if (iStart >= 0) iSlot.init (iStart);
						}
					}

					// Play music :-)
					output = (iSlot.params[SLOTS_PLAY] ? iSlot.play (step) : input);
					iSlot.mixf = 1.0f;
				}

				positions[j].step = iStep;
			}

			audioOutput1[i] = (sumFaders * audioOutput1[i] + fader * output.left) / (sumFaders + fader);
			audioOutput2[i] = (sumFaders * audioOutput2[i] + fader * output.right) / (sumFaders + fader);;

			sumFaders += fader;
		}
	}
}

LV2_State_Status BOops::state_save (LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{
	// Store sample path
	if (sample && sample->path && (sample->path[0] != 0) && (globalControllers[SOURCE] == SOURCE_SAMPLE))
	{
		LV2_State_Map_Path* mapPath = NULL;
		LV2_State_Free_Path* freePath = NULL;
		const char* missing  = lv2_features_query
		(
			features,
			LV2_STATE__mapPath, &mapPath, true,
			LV2_STATE__freePath, &freePath, false,
			nullptr
		);

		if (missing)
		{
			fprintf (stderr, "BOops.lv2: Host doesn't support required features.\n");
			return LV2_STATE_ERR_NO_FEATURE;
		}

		if (mapPath)
		{
			char* abstrPath = mapPath->abstract_path(mapPath->handle, sample->path);

			if (abstrPath)
			{
				fprintf(stderr, "BOops.lv2: Save abstr_path:%s\n", abstrPath);

				store(handle, urids.bOops_samplePath, abstrPath, strlen (abstrPath) + 1, urids.atom_Path, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
				store(handle, urids.bOops_sampleStart, &sample->start, sizeof (sample->start), urids.atom_Long, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
				store(handle, urids.bOops_sampleEnd, &sample->end, sizeof (sample->end), urids.atom_Long, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
				store(handle, urids.bOops_sampleAmp, &sampleAmp, sizeof (sampleAmp), urids.atom_Float, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
				const int32_t sloop = int32_t (sample->loop);
				store(handle, urids.bOops_sampleLoop, &sloop, sizeof (sloop), urids.atom_Bool, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

				if (freePath) freePath->free_path (freePath->handle, abstrPath);
				else free (abstrPath);
			}

			else fprintf(stderr, "BOops.lv2: Can't generate abstr_path from %s\n", sample->path);
		}
		else fprintf (stderr, "BOops.lv2: Feature map_path not available! Can't save sample!\n" );
	}

	// Store transportGateKeys
	{
		// Create atom:Vector

		AtomKeys atom;
		int keysize = 0;
		std::fill (atom.keys, atom.keys + NR_PIANO_KEYS, 0);
		for (int i = 0; i < NR_PIANO_KEYS; ++i)
		{
			if (transportGateKeys[i])
			{
				atom.keys[keysize] = i;
				++keysize;
			}
		}
		atom.body.child_type = urids.atom_Int;
		atom.body.child_size = sizeof(int);

		store (handle, urids.bOops_transportGateKeys, &atom, keysize * sizeof (int) + sizeof(LV2_Atom_Vector_Body), urids.atom_Vector, LV2_STATE_IS_POD);
	}

	// Store pads
	{
		char padDataString[0x8010] = "\nMatrix data:\n";

		for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
		{
			if ((slots[slotNr].effect == FX_NONE) || (slots[slotNr].effect == FX_INVALID)) continue;

			for (int stepNr = 0; stepNr < NR_STEPS; ++stepNr)
			{
				Pad p = slots[slotNr].getPad (stepNr);
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
		store (handle, urids.bOops_statePad, padDataString, strlen (padDataString) + 1, urids.atom_String, LV2_STATE_IS_POD);
	}

	// Store shapes
	{
		char shapesDataString[0x10010] = "Shape data:\n";

		for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
		{
			if (!slots[slotNr].shape.isDefault())
			{
				for (unsigned int nodeNr = 0; nodeNr < slots[slotNr].shape.size (); ++nodeNr)
				{
					char valueString[160];
					Node node = slots[slotNr].shape.getNode (nodeNr);
					snprintf
					(
						valueString,
						126,
						"slo:%d; typ:%d; ptx:%f; pty:%f; h1x:%f; h1y:%f; h2x:%f; h2y:%f",
						slotNr,
						int (node.nodeType),
						node.point.x,
						node.point.y,
						node.handle1.x,
						node.handle1.y,
						node.handle2.x,
						node.handle2.y
					);
					if ((slotNr < NR_SLOTS - 1) || (nodeNr < slots[slotNr].shape.size ())) strcat (valueString, ";\n");
					else strcat(valueString, "\n");
					strcat (shapesDataString, valueString);
				}
			}
		}
		store (handle, urids.bOops_shapeData, shapesDataString, strlen (shapesDataString) + 1, urids.atom_String, LV2_STATE_IS_POD);
	}

	return LV2_STATE_SUCCESS;
}

LV2_State_Status BOops::state_restore (LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
			const LV2_Feature* const* features)
{

	// Get host features
	LV2_Worker_Schedule* schedule = nullptr;
	LV2_State_Map_Path* mapPath = nullptr;
	LV2_State_Free_Path* freePath = nullptr;
	const char* missing  = lv2_features_query
	(
		features,
		LV2_STATE__mapPath, &mapPath, true,
		LV2_STATE__freePath, &freePath, false,
		LV2_WORKER__schedule, &schedule, false,
		nullptr
	);

	if (missing)
	{
		fprintf (stderr, "BOops.lv2: Host doesn't support required features.\n");
		return LV2_STATE_ERR_NO_FEATURE;
	}

	size_t   size;
	uint32_t type;
	uint32_t valflags;

	// Retireve sample data
	char samplePath[PATH_MAX] = {0};
	int64_t sampleStart = 0;
	int64_t sampleEnd = 0;
	float sampleAmp = 1.0;
	int32_t sampleLoop = false;

	const void* pathData = retrieve (handle, urids.bOops_samplePath, &size, &type, &valflags);
	if (pathData)
	{
		char* absPath  = mapPath->absolute_path (mapPath->handle, (char*)pathData);
	        if (absPath)
		{
			if (strlen (absPath) < PATH_MAX) strcpy (samplePath, absPath);
			else
			{
				fprintf (stderr, "BOops.lv2: Sample path too long.\n");
				message.setMessage (CANT_OPEN_SAMPLE);
			}

			fprintf(stderr, "BOops.lv2: Restore abs_path:%s\n", absPath);

			if (freePath) freePath->free_path (freePath->handle, absPath);
			else free (absPath);
	        }
	}

	const void* startData = retrieve (handle, urids.bOops_sampleStart, &size, &type, &valflags);
        if (startData && (type == urids.atom_Long)) sampleStart = *(int64_t*)startData;
	const void* endData = retrieve (handle, urids.bOops_sampleEnd, &size, &type, &valflags);
        if (endData && (type == urids.atom_Long)) sampleEnd = *(int64_t*)endData;
	const void* ampData = retrieve (handle, urids.bOops_sampleAmp, &size, &type, &valflags);
        if (ampData && (type == urids.atom_Float)) sampleAmp = *(float*)ampData;
	const void* loopData = retrieve (handle, urids.bOops_sampleLoop, &size, &type, &valflags);
        if (loopData && (type == urids.atom_Bool)) sampleLoop = *(int32_t*)loopData;

	if (activated && schedule)
	{
		LV2_Atom_Forge forge;
		lv2_atom_forge_init(&forge, map);
		uint8_t buf[1200];
		lv2_atom_forge_set_buffer(&forge, buf, sizeof(buf));
		LV2_Atom_Forge_Frame frame;
		LV2_Atom* msg = (LV2_Atom*)forgeSamplePath (&forge, &frame, samplePath, sampleStart, sampleEnd, sampleAmp, sampleLoop);
		lv2_atom_forge_pop(&forge, &frame);
		if (msg) schedule->schedule_work(schedule->handle, lv2_atom_total_size(msg), msg);
	}

	else
	{
		// Free old sample
		if (sample)
		{
			delete sample;
			sample = nullptr;
			sampleAmp = 1.0;
		}

		// Load new sample
		message.deleteMessage (CANT_OPEN_SAMPLE);
		try {sample = new Sample (samplePath);}
		catch (std::bad_alloc &ba)
		{
			fprintf (stderr, "BOops.lv2: Can't allocate enoug memory to open sample file.\n");
			message.setMessage (CANT_OPEN_SAMPLE);
		}
		catch (std::invalid_argument &ia)
		{
			fprintf (stderr, "%s\n", ia.what());
			message.setMessage (CANT_OPEN_SAMPLE);
		}

		// Set new sample properties
		if  (sample)
		{
			sample->start = sampleStart;
			sample->end = sampleEnd;
			sample->loop = bool (sampleLoop);
			this->sampleAmp = sampleAmp;
		}

		scheduleNotifySamplePathToGui = true;
	}

	// Retrieve transportGateKeys
	const void* transportGateKeysData = retrieve(handle, urids.bOops_transportGateKeys, &size, &type, &valflags);
	if (transportGateKeysData && (type == urids.atom_Vector))
	{
		const AtomKeys* atom = (const AtomKeys*) transportGateKeysData;
		const int nr = LIMIT ((size - sizeof (LV2_Atom_Vector_Body)) / sizeof(int), 0, NR_PIANO_KEYS);

		std::fill (transportGateKeys, transportGateKeys + NR_PIANO_KEYS, false);
		for (int i = 0; i < nr; ++i)
		{
			const int keyNr = atom->keys[i];
			if ((keyNr >= 0) && (keyNr < NR_PIANO_KEYS)) transportGateKeys[keyNr] = true;
		}
		scheduleNotifyTransportGateKeys = true;
        }


	// Retrieve pattern
	const void* padData = retrieve(handle, urids.bOops_statePad, &size, &type, &valflags);
	if (padData && (type == urids.atom_String))
	{
		for (int slotNr = 0; slotNr < NR_SLOTS; ++slotNr)
		{
			for (unsigned int i = 0; i < NR_STEPS; ++i) slots[slotNr].setPad (i, Pad());
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
				fprintf (stderr, "BOops.lv2: Restore pad state incomplete. Can't parse slot nr from \"%s...\"", padDataString.substr (0, 63).c_str());
				break;
			}

			if (nextPos > 0) padDataString.erase (0, nextPos);
			if ((slotNr < 0) || (slotNr >= NR_SLOTS))
			{
				fprintf (stderr, "BOops.lv2: Restore pad state incomplete. Invalid slot nr %i.\n", slotNr);
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
				fprintf (stderr, "BOops.lv2: Restore pad state incomplete. Can't parse step nr from \"%s...\"", padDataString.substr (0, 63).c_str());
				break;
			}

			if (nextPos > 0) padDataString.erase (0, nextPos);
			if ((stepNr < 0) || (stepNr >= NR_STEPS))
			{
				fprintf (stderr, "BOops.lv2: Restore pad state incomplete. Invalid step nr %i.\n", slotNr);
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
					fprintf (stderr, "BOops.lv2: Restore padstate incomplete. Can't parse %s from \"%s...\"",
							 keywords[i].substr(0,2).c_str(), padDataString.substr (0, 63).c_str());
					break;
				}

				if (nextPos > 0) padDataString.erase (0, nextPos);

				Pad p = slots[slotNr].getPad (stepNr);
				switch (i)
				{
					case 2:	p.gate = LIMIT (val, 0, 1);
						break;

					case 3:	p.size = LIMIT (val, 1, NR_STEPS - stepNr);
						break;

					case 4:	p.mix = LIMIT (val, 0, 1);
						break;

					default:break;
				}
				slots[slotNr].setPad (stepNr, p);
			}
		}

		std::fill (scheduleNotifySlot, scheduleNotifySlot + NR_SLOTS, true);
	}

	// Retrieve shapes
	const void* shapesData = retrieve(handle, urids.bOops_shapeData, &size, &type, &valflags);
	if (shapesData && (type == urids.atom_String))
	{
		Shape<SHAPE_MAXNODES> shapes[NR_SLOTS];
		for (int sl = 0; sl < NR_SLOTS; ++sl) shapes[sl].clearShape();

		// Parse retrieved data
		std::string shapesDataString = (char*) shapesData;
		const std::string keywords[8] = {"slo:", "typ:", "ptx:", "pty:", "h1x:", "h1y:", "h2x:", "h2y:"};
		while (!shapesDataString.empty())
		{
			// Look for next "slo:"
			size_t strPos = shapesDataString.find ("slo:");
			size_t nextPos = 0;
			if (strPos == std::string::npos) break;	// No "shp:" found => end
			if (strPos + 4 > shapesDataString.length()) break;	// Nothing more after id => end
			shapesDataString.erase (0, strPos + 4);

			int sl;
			try {sl = BUtilities::stof (shapesDataString, &nextPos);}
			catch  (const std::exception& e)
			{
				fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Can't parse shape number from \"%s...\"", shapesDataString.substr (0, 63).c_str());
				break;
			}

			if (nextPos > 0) shapesDataString.erase (0, nextPos);
			if ((sl < 0) || (sl >= NR_SLOTS))
			{
				fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Invalid matrix data block loaded for shape %i.\n", sl);
				break;
			}

			// Look for shape data
			Node node = {NodeType::POINT_NODE, {0, 0}, {0, 0}, {0, 0}};
			bool isTypeDef = false;
			for (int i = 1; i < 8; ++i)
			{
				strPos = shapesDataString.find (keywords[i]);
				if (strPos == std::string::npos) continue;	// Keyword not found => next keyword
				if (strPos + 4 >= shapesDataString.length())	// Nothing more after keyword => end
				{
					shapesDataString ="";
					break;
				}
				if (strPos > 0) shapesDataString.erase (0, strPos + 4);
				float val;
				try {val = BUtilities::stof (shapesDataString, &nextPos);}
				catch  (const std::exception& e)
				{
					fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Can't parse %s from \"%s...\"",
							 keywords[i].substr(0,3).c_str(), shapesDataString.substr (0, 63).c_str());
					break;
				}

				if (nextPos > 0) shapesDataString.erase (0, nextPos);
				switch (i)
				{
					case 1: node.nodeType = (NodeType)((int)val);
						isTypeDef = true;
						break;
					case 2: node.point.x = val;
						break;
					case 3:	node.point.y = val;
						break;
					case 4:	node.handle1.x = val;
						break;
					case 5:	node.handle1.y = val;
						break;
					case 6:	node.handle2.x = val;
						break;
					case 7:	node.handle2.y = val;
						break;
					default:break;
				}
			}

			// Set data
			if (isTypeDef) shapes[sl].appendNode (node);
		}

		// Validate all shapes
		for (int sl = 0; sl < NR_SLOTS; ++sl)
		{
			if (shapes[sl].size () < 2) shapes[sl].setDefaultShape ();
			else if (!shapes[sl].validateShape ()) slots[sl].shape.setDefaultShape ();
		}

		// Install new shape
		for (int sl = 0; sl < NR_SLOTS; ++sl)
		{
			slots[sl].shape = shapes[sl];
			scheduleNotifyShape[sl] = true;
		}
	}

	return LV2_STATE_SUCCESS;
}

LV2_Worker_Status BOops::work (LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle, uint32_t size, const void* data)
{
	const LV2_Atom* atom = (const LV2_Atom*)data;
	if (!atom) return LV2_WORKER_ERR_UNKNOWN;

	// Free old buffers
        else if (atom->type == urids.bOops_freeBuffers)
	{
		const Atom_BufferList* bAtom = (const Atom_BufferList*) data;
		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (bAtom->buffers[i]) delete (bAtom->buffers[i]);
		}
        }

	// Free old Fx
        else if (atom->type == urids.bOops_freeFx)
	{
		const Atom_Fx* fAtom = (const Atom_Fx*) data;
		if (fAtom->fx) delete (fAtom->fx);
        }

	// Free old sample
        else if (atom->type == urids.bOops_sampleFreeEvent)
	{
		const AtomSample* sAtom = (AtomSample*) atom;
		if (sAtom->sample) delete sAtom->sample;
        }

	// Load sample
	else if ((atom->type == urids.atom_Object) && (((LV2_Atom_Object*)atom)->body.otype == urids.bOops_samplePathEvent))
	{
                const LV2_Atom_Object* obj = (const LV2_Atom_Object*)data;

		const LV2_Atom* oPath = NULL, *oStart = NULL, *oEnd = NULL, *oAmp = NULL, *oLoop = NULL;
		lv2_atom_object_get
		(
			obj,
			urids.bOops_samplePath, &oPath,
			urids.bOops_sampleStart, &oStart,
			urids.bOops_sampleEnd, &oEnd,
			urids.bOops_sampleAmp, &oAmp,
			urids.bOops_sampleLoop, &oLoop,
			0
		);

		// New sample
		if (oPath && (oPath->type == urids.atom_Path))
		{
			message.deleteMessage (CANT_OPEN_SAMPLE);
			Sample* s = nullptr;

			const char* pathName = (const char*)LV2_ATOM_BODY_CONST(oPath);
			if (pathName && (pathName[0] != 0))
			{
				try {s = new Sample (pathName);}
				catch (std::bad_alloc &ba)
				{
					fprintf (stderr, "BOops.lv2: Can't allocate enough memory to open sample file.\n");
					message.setMessage (CANT_OPEN_SAMPLE);
					return LV2_WORKER_ERR_NO_SPACE;
				}
				catch (std::invalid_argument &ia)
				{
					fprintf (stderr, "%s\n", ia.what());
					message.setMessage (CANT_OPEN_SAMPLE);
					return LV2_WORKER_ERR_UNKNOWN;
				}
			}

			AtomSample sAtom;
			sAtom.atom = {sizeof (s), urids.bOops_installSample};
			sAtom.sample = s;
			sAtom.start = (oStart && (oStart->type == urids.atom_Long) && s ? ((LV2_Atom_Long*)oStart)->body : 0);
			sAtom.end = (s ? (oEnd && (oEnd->type == urids.atom_Long) ? ((LV2_Atom_Long*)oEnd)->body : s->info.frames) : 0);
			sAtom.amp = (oAmp && (oAmp->type == urids.atom_Float) ? ((LV2_Atom_Float*)oAmp)->body : 1.0f);
			sAtom.loop = (oLoop && (oLoop->type == urids.atom_Bool && s) ? ((LV2_Atom_Bool*)oLoop)->body : 0);
			respond (handle, sizeof(sAtom), &sAtom);
		}

		else return LV2_WORKER_ERR_UNKNOWN;
        }

	// Allocate new buffers
	else if (atom->type == urids.bOops_allocateBuffers)
	{
		//Required buffer size
		double fpst = getFramesPerStep (positions.back().transport);
		size_t bSize = slots[0].buffer->size();

		if ((bSize < globalControllers[STEPS] * fpst) || (bSize > 2.0 * globalControllers[STEPS] * fpst))
		{
			Atom_BufferList bAtom;
			bAtom.atom = {sizeof (bAtom.buffers), urids.bOops_installBuffers};
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
					fprintf (stderr, "BOops.lv2: Can't allocate enough memory to resize audio buffers.\n");
					//message.setMessage (MEMORY_ERR);
					return LV2_WORKER_ERR_NO_SPACE;
				}
			}

			respond (handle, sizeof (bAtom) , &bAtom);
		}

		// Resize not needed
		else scheduleResizeBuffers = false;
	}

	else if (atom->type == urids.bOops_allocateFx)
	{
		const LV2_Atom_Int* iAtom = (const LV2_Atom_Int*)data;
		int slotNr = iAtom->body;

		BOopsEffectsIndex effect = (BOopsEffectsIndex) *new_controllers[SLOTS + slotNr * (SLOTS_PARAMS + NR_PARAMS)];
		Fx* fx = slots[slotNr].newFx (effect);
		Atom_Fx fAtom = {{sizeof (int) + sizeof (BOopsEffectsIndex) + sizeof (Fx*), urids.bOops_installFx}, slotNr, effect, fx};
		respond (handle, sizeof (fAtom) , &fAtom);
	}

	else return LV2_WORKER_ERR_UNKNOWN;

	return LV2_WORKER_SUCCESS;
}

LV2_Worker_Status BOops::work_response (uint32_t size, const void* data)
{
	const LV2_Atom* atom = (const LV2_Atom*)data;
	if (!atom) return LV2_WORKER_ERR_UNKNOWN;

	// Install slot audio buffers
	if (atom->type == urids.bOops_installBuffers)
	{
		// Schedule worker to free old buffers
		Atom_BufferList bAtom;
		bAtom.atom = {sizeof (bAtom.buffers), urids.bOops_freeBuffers};
		for (int i = 0; i < NR_SLOTS; ++i) bAtom.buffers[i] = slots[i].buffer;
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (bAtom), &bAtom);

		// Install new buffer
		const Atom_BufferList* nAtom = (const Atom_BufferList*) data;
		for (int i = 0; i < NR_SLOTS; ++i) slots[i].buffer = nAtom->buffers[i];
		scheduleResizeBuffers = false;
	}

	// Install Fx
	else if (atom->type == urids.bOops_installFx)
	{
		const Atom_Fx* nAtom = (const Atom_Fx*) data;

		// Schedule worker to free old Fx
		Atom_Fx fAtom;
		fAtom = {{sizeof (int) + sizeof (BOopsEffectsIndex) + sizeof (Fx*), urids.bOops_freeFx}, nAtom->index, slots[nAtom->index].effect, slots[nAtom->index].fx};
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (fAtom), &fAtom);

		// Install new Fx
		slots[nAtom->index].fx = nAtom->fx;
		slots[nAtom->index].effect = BOopsEffectsIndex (nAtom->effect);
		scheduleSetFx[nAtom->index] = false;
	}

	// Install sample
	else if (atom->type == urids.bOops_installSample)
	{
		const AtomSample* nAtom = (const AtomSample*)data;
		// Schedule worker to free old sample
		AtomSample sAtom = {{sizeof (Sample*), urids.bOops_sampleFreeEvent}, sample};
		workerSchedule->schedule_work (workerSchedule->handle, sizeof (sAtom), &sAtom);

		// Install new sample from data
		sample = nAtom->sample;
		if (sample)
		{
			sample->start = LIMIT (nAtom->start, 0, sample->info.frames - 1);
			sample->end = LIMIT (nAtom->end, sample->start, sample->info.frames);
			sampleAmp = LIMIT (nAtom->amp, 0.0f, 1.0f);
			sample->loop = bool (nAtom->loop);
		}
		scheduleNotifySamplePathToGui = true;
	}

	return LV2_WORKER_SUCCESS;
}

LV2_Handle instantiate (const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
	// New instance
	BOops* instance;
	try {instance = new BOops(samplerate, bundle_path, features);}
	catch (std::exception& exc)
	{
		fprintf (stderr, "BOops.lv2: Plugin instantiation failed. %s\n", exc.what ());
		return NULL;
	}

	if (!instance)
	{
		fprintf(stderr, "BOops.lv2: Plugin instantiation failed.\n");
		return NULL;
	}

	if (!instance->map)
	{
		fprintf(stderr, "BOops.lv2: Host does not support urid:map.\n");
		delete (instance);
		return NULL;
	}

	return (LV2_Handle)instance;
}

void connect_port (LV2_Handle instance, uint32_t port, void *data)
{
	BOops* inst = (BOops*) instance;
	if (inst) inst->connect_port (port, data);
}

void activate (LV2_Handle instance)
{
	BOops* inst = (BOops*) instance;
	if (inst) inst->activate();
}

void run (LV2_Handle instance, uint32_t n_samples)
{
	BOops* inst = (BOops*) instance;
	if (inst) inst->run (n_samples);
}

void deactivate (LV2_Handle instance)
{
	BOops* inst = (BOops*) instance;
	if (inst) inst->deactivate();
}

void cleanup (LV2_Handle instance)
{
	BOops* inst = (BOops*) instance;
	if (inst) delete inst;
}

static LV2_State_Status state_save(LV2_Handle instance, LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BOops* inst = (BOops*)instance;
	if (!inst) return LV2_STATE_SUCCESS;

	inst->state_save (store, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static LV2_State_Status state_restore(LV2_Handle instance, LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags,
           const LV2_Feature* const* features)
{
	BOops* inst = (BOops*)instance;
	if (inst) inst->state_restore (retrieve, handle, flags, features);
	return LV2_STATE_SUCCESS;
}

static LV2_Worker_Status work (LV2_Handle instance, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle,
	uint32_t size, const void* data)
{
	BOops* inst = (BOops*)instance;
	if (!inst) return LV2_WORKER_SUCCESS;

	return inst->work (respond, handle, size, data);
}

static LV2_Worker_Status work_response (LV2_Handle instance, uint32_t size,  const void* data)
{
	BOops* inst = (BOops*)instance;
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
	BOOPS_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
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
