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

#ifndef BOOPS_HPP_
#define BOOPS_HPP_

#include <cmath>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"
#include "Pad.hpp"
#include "Slot.hpp"
#include "Message.hpp"
#include "StaticArrayList.hpp"
#include "Sample.hpp"

struct Transport
{
	double rate;
	float bpm;
	float speed;
	uint64_t bar;
	float barBeat;
	float beatsPerBar;
	int beatUnit;
};

struct Position
{
	double position;
	int step;
	double offset;
	uint64_t refFrame;
	Transport transport;
	double fader;
	bool playing;
};

class BOops
{
public:
	BOops (double samplerate, const char* bundle_path, const LV2_Feature* const* features);
	~BOops();
	void connect_port (uint32_t port, void *data);
	void run (uint32_t n_samples);
	LV2_State_Status state_save(LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);
	LV2_State_Status state_restore(LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);
	LV2_Worker_Status work (LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle, uint32_t size, const void* data);
	LV2_Worker_Status work_response (uint32_t size, const void* data);

	LV2_URID_Map* map;
	LV2_Worker_Schedule* workerSchedule;

private:
	void play(uint32_t start, uint32_t end);
	void resizeSteps ();
	void notifySlotToGui (const int slot);
	void notifyShapeToGui (const int slot);
	void notifyMessageToGui ();
	void notifyStatusToGui ();
	void notifyWaveformToGui (const int start, const int end);
	void notifyTransportGateKeysToGui ();
	void notifySamplePathToGui ();
	double getPositionFromBeats (const Transport& transport, const double beats);
	double getPositionFromFrames (const Transport& transport, const uint64_t frames);
	uint64_t getFramesFromPosition (const Transport& transport, const double position) const;
	double getPositionFromSeconds (const Transport& transport, const double seconds);
	double getFramesPerStep (const Transport& transport);

public:	char pluginPath[1024];
private:
	BOopsURIDs urids;

public:	Transport host;
private:
	StaticArrayList<Position, MAXFADERS> positions;
	bool transportGateKeys[NR_PIANO_KEYS];

	// Atom ports
	LV2_Atom_Sequence* controlPort;
	LV2_Atom_Sequence* notifyPort;

	// Audio ports
	float* audioInput1;
	float* audioInput2;
	float* audioOutput1;
	float* audioOutput2;

	// Controller ports
	float* new_controllers[NR_CONTROLLERS];
	float globalControllers [SLOTS];

	LV2_Atom_Forge forge;
	LV2_Atom_Forge_Frame notify_frame;

	// Internals
public:	Slot slots[NR_SLOTS];
private:
	Sample* sample;
	float waveform[WAVEFORMSIZE];
	int waveformCounter;
	int lastWaveformCounter;

	Message message;
	bool ui_on;
	bool scheduleNotifySlot[NR_SLOTS];
	bool scheduleNotifyShape[NR_SLOTS];
	bool scheduleNotifyStatus;
	bool scheduleResizeBuffers;
	bool scheduleSetFx[NR_SLOTS];
	bool scheduleNotifyWaveformToGui;
	bool scheduleNotifyTransportGateKeys;
	bool scheduleNotifySamplePathToGui;

	struct Atom_BufferList
	{
		LV2_Atom atom;
		RingBuffer<Stereo>* buffers[NR_SLOTS];
	};

	struct AtomKeys
	{
		LV2_Atom_Vector_Body body;
		int keys[NR_PIANO_KEYS];
	};

	struct Atom_Fx
	{
		LV2_Atom atom;
		int index;
		BOopsEffectsIndex effect;
		Fx* fx;
	};

	struct AtomSample
	{
		LV2_Atom atom;
		Sample* sample;
	};
};

#endif /* BOOPS_HPP_ */
