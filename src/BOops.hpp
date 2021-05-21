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

class Sample; 	// Forward declaration

struct Transport
{
	double rate;
	float bpm;
	float speed;
	uint64_t bar;
	float barBeat;
	float beatsPerBar;
};

struct Position
{
	double sequence;	// Position within a sequencer loop.
	int step;		// Current step number.
	double offset;		// Offset if sequence didn't start at 0.0.
	uint64_t refFrame;	// Internal reference. Frame at which sequence updated.
	Transport transport;	// Transport data for calculation of position.
	double fader;		// 0.0..1.0. Mix ratio. Last fader is count up to 1.0, all others to 0.0. 0.0 faders will be deleted.
	bool playing;		// Status.
};

struct PageControls
{
	int status;
	int channel;
	int message;
	int value;
};

struct Page
{
	PageControls controls;
	std::array<std::array<Pad, NR_STEPS>, NR_SLOTS> pads;
};

class BOops
{
public:
	BOops (double samplerate, const char* bundle_path, const LV2_Feature* const* features);
	~BOops();
	void connect_port (uint32_t port, void *data);
	void run (uint32_t n_samples);
	void activate();
	void deactivate();
	LV2_State_Status state_save(LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);
	LV2_State_Status state_restore(LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t flags, const LV2_Feature* const* features);
	LV2_Worker_Status work (LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle handle, uint32_t size, const void* data);
	LV2_Worker_Status work_response (uint32_t size, const void* data);

	LV2_URID_Map* map;
	LV2_Worker_Schedule* workerSchedule;

private:
	Stereo getSample (const Position& p, const double pos);
	void play(uint32_t start, uint32_t end);
	void resizeSteps ();
	void notifyAllSlotsToGui ();
	void notifyShapeToGui (const int slot);
	void notifyMessageToGui ();
	void notifyStatusToGui ();
	void notifyWaveformToGui (const int start, const int end);
	void notifyTransportGateKeysToGui ();
	void notifySamplePathToGui ();
	void notifyStateChanged ();
	void notifyPageControls (const int pageId);
	void notifyMidiLearnedToGui ();
	LV2_Atom_Forge_Ref forgeSamplePath (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame,  const char* path, const int64_t start, const int64_t end, const float amp, const int32_t loop);
	LV2_Atom_Forge_Ref forgeShape (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int slot, const Shape<SHAPE_MAXNODES>* shape);
	LV2_Atom_Forge_Ref forgeTransportGateKeys (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int* keys, const size_t size);
	LV2_Atom_Forge_Ref forgePads (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int page, const int slot, const size_t size);
	LV2_Atom_Forge_Ref forgePageControls (LV2_Atom_Forge* forge, LV2_Atom_Forge_Frame* frame, const int pageId);
	double getPositionFromBeats (const Transport& transport, const double beats);
	double getPositionFromFrames (const Transport& transport, const uint64_t frames);
	uint64_t getFramesFromPosition (const Transport& transport, const double position) const;
	double getPositionFromSeconds (const Transport& transport, const double seconds);
	double getFramesPerStep (const Transport& transport);
	Position& backPosition();
	size_t sizePosition();
	void pushBackPosition (Position& p);
	void popBackPosition();
	void popFrontPosition();

public:	char pluginPath[1024];
private:
	BOopsURIDs urids;

public:	Transport host;
private:
	bool activated;
	Position positions[2];
	bool transportGateKeys[NR_PIANO_KEYS];

	std::array<Page, NR_PAGES> pages;
	int pageNr;
	int pageMax;
	bool midiLearn;
	uint8_t midiLearned[4];

	int editorPage;
	int editorSlot;

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
public:	std::array<Slot, NR_SLOTS> slots;
private:
	Sample* sample;
	float sampleAmp;
	float waveform[WAVEFORMSIZE];
	int waveformCounter;
	int lastWaveformCounter;

	Message message;
	bool ui_on;
	bool scheduleNotifyAllSlots;
	bool scheduleNotifyPageControls[NR_PAGES];
	bool scheduleNotifyShape[NR_SLOTS];
	bool scheduleNotifyStatus;
	bool scheduleResizeBuffers;
	bool scheduleSetFx[NR_SLOTS];
	bool scheduleNotifyWaveformToGui;
	bool scheduleNotifyTransportGateKeys;
	bool scheduleNotifySamplePathToGui;
	bool scheduleNotifyMidiLearnedToGui;
	bool scheduleStateChanged;

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

	struct AtomPageControls
	{
		LV2_Atom_Vector_Body body;
		PageControls data[NR_PAGES];
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
		int64_t start;
		int64_t end;
		float amp;
		int32_t loop;
	};
};

#endif /* BOOPS_HPP_ */
