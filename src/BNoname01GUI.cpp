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

#include "BNoname01GUI.hpp"
#include "BUtilities/to_string.hpp"
#include "MessageDefinitions.hpp"
#include "FxDefaults.hpp"

#include "OptionWidget.hpp"
#include "OptionAmp.hpp"
#include "OptionBalance.hpp"
#include "OptionWidth.hpp"
#include "OptionDelay.hpp"
#include "OptionChopper.hpp"
#include "OptionTapeStop.hpp"
#include "OptionTapeSpeed.hpp"
#include "OptionBitcrush.hpp"
#include "OptionDecimate.hpp"
#include "OptionDistortion.hpp"
#include "OptionNoise.hpp"

inline double floorfrac (const double value) {return value - floor (value);}
inline double floormod (const double numer, const double denom) {return numer - floor(numer / denom) * denom;}

BNoname01GUI::BNoname01GUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeWindow parentWindow) :
	Window (1240, 608, "B.Noname01", parentWindow, true),
	controller (NULL), write_function (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) : std::string ("")),
	sz (1.0), bgImageSurface (nullptr),
	urids (), forge (),
	pattern (),
	clipBoard (),
	cursor (0), wheelScrolled (false), padPressed (false), deleteMode (false),
	actSlot (-1),

	mContainer (0, 0, 1240, 608, "main"),
	messageLabel (400, 45, 600, 20, "ctlabel", ""),

	settingsContainer (10, 90, 1220, 40, "widget"),
	playButton (8, 8, 24, 24, "widget", "Play"),
	bypassButton (38, 8, 24, 24, "widget", "Bypass"),
	stopButton (68, 8, 24, 24, "widget", "Stop"),
	playModeListBox (300, 10, 220, 20, 220, 80, "menu", BItems::ItemList ({{0, "Autoplay"}, {2, "Host-controlled playback"} , {1, "MIDI-controlled playback"}})),
	onMidiListBox (540, 10, 120, 20, 120, 80, "menu", BItems::ItemList ({{0, "Restart"}, {2, "Restart & sync"}, {1, "Continue"}})),
	midiButton (680, 10, 20, 20, "button"),
	autoplayBpmLabel (540, 0, 80, 8, "smlabel", "bpm:"),
	autoplayBpmSlider (540, 10, 80, 20, "slider", 120, 1, 300, 0, "%1.0f"),
	autoplayBpbLabel (640, 0, 80, 8, "smlabel", "bpBar:"),
	autoplayBpbSlider (640, 10, 80, 20, "slider", 4, 1, 16, 1, "%1.0f"),
	sequenceSizeSelect (910, 12, 80, 16, "select", 1, 1, 16, 0.01),
	sequenceBaseListBox (1010, 10, 90, 20, 0, 20, 90, 80, "menu", BItems::ItemList ({{0, "Seconds"}, {1, "Beats"}, {2, "Bars"}}), 1),
	stepsListBox (1120, 10, 90, 20, 0, 20, 100, 240, "menu",
		     BItems::ItemList ({{2, "2 Steps"}, {3, "3 Steps"}, {4, "4 Steps"}, {6, "6 Steps"}, {8, "8 Steps"}, {9, "9 Steps"},
		     			{12, "12 Steps"}, {16, "16 Steps"}, {18, "18 Steps"}, {24, "24 Steps"}, {32, "32 Steps"}}), 16),


	padSurface (310, 130, 910, 288, "padsurface"),
	editContainer (658, 426, 284, 24, "widget"),

	padParamContainer (1040, 458, 180, 130, "widget"),
	padGateLabel (20, 90, 60, 20, "ctlabel", "Gate"),
	padGateDial (20, 20, 60, 60, "dial", 1.0, 0.0, 1.0, 0.0, "%1.2f"),
	padMixLabel (100, 90, 60, 20, "ctlabel", "Mix"),
	padMixDial (100, 20, 60, 60, "dial", 1.0, 0.0, 1.0, 0.0, "%1.2f")
{
	// Init slots
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		slots[i].container = BWidgets::ValueWidget (20, 130 + i * 24, 280, 24, "padSurface", FX_NONE);
		slots[i].addPad = PadButton (0, 0, 20, 24, "pad0", ADDSYMBOL);
		slots[i].delPad = PadButton (20, 0, 20, 24, "pad0", CLOSESYMBOL);
		slots[i].upPad = PadButton (40, 0, 20, 24, "pad0", UPSYMBOL);
		slots[i].downPad = PadButton (60, 0, 20, 24, "pad0", DOWNSYMBOL);
		slots[i].effectPad = IconPadButton (80, 0, 180, 24, "pad0", pluginPath + "inc/Menu.png", "");
		slots[i].effectsListbox = BWidgets::ListBox (80, 24, 180, 160, "listbox", BItems::ItemList (BNONAME01FXNAMES));
		slots[i].playPad = PadToggleButton (260, 0, 20, 24, "pad0", PLAYSYMBOL);
	}

	// Init editButtons
	for (int i = 0; i < EDIT_RESET; ++i) edit1Buttons[i] = HaloToggleButton (i * 30, 0, 24, 24, "widget", editLabels[i]);
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i) edit2Buttons[i] = HaloButton (170 + i * 30, 0, 24, 24, "widget", editLabels[i + EDIT_RESET]);

	// Init slot params
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		slotParams[i].container = BWidgets::Widget (20, 438, 1000, 150, "widget");
		slotParams[i].nrIcon = BWidgets::ImageIcon (20, 8, 40, 20, "widget", "");
		slotParams[i].nameIcon = BWidgets::ImageIcon (60, 8, 160, 20, "widget", "");
		slotParams[i].attackLabel = BWidgets::Label (190, 30, 20, 20, "ctlabel", "A");
		slotParams[i].decayLabel = BWidgets::Label (190, 60, 20, 20, "ctlabel", "D");
		slotParams[i].sustainLabel = BWidgets::Label (190, 90, 20, 20, "ctlabel", "S");
		slotParams[i].releaseLabel = BWidgets::Label (190, 120, 20, 20, "ctlabel", "R");
		slotParams[i].attackSlider = HSlider (210, 30, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].decaySlider = HSlider (210, 60, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].sustainSlider = HSlider (210, 90, 60, 20, "slider", 1.0, 0.0, 1.0, 0.0, "%1.2f");
		slotParams[i].releaseSlider = HSlider (210, 120, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].adsrDisplay = CurveChart (10, 30, 170, 110, "slider");
		slotParams[i].panLabel = BWidgets::Label (280, 110, 60, 20, "ctlabel", "Pan");
		slotParams[i].panDial = Dial (280, 40, 60, 60, "dial", 0.0, -1.0, 1.0, 0.0, "%1.2f");
		slotParams[i].mixLabel = BWidgets::Label (360, 110, 60, 20, "ctlabel", "Mix");
		slotParams[i].mixDial = Dial (360, 40, 60, 60, "dial", 0.0, 0.0, 1.0, 0.0, "%1.2f");
		for (int j = 0; j < NR_OPTPARAMS; ++j) slotParams[i].options[j] = Dial (0, 0, 0, 0, "widget", 0.0, 0.0, 1.0, 0.0);
		slotParams[i].optionWidget = nullptr;
	}

	// Link controllerWidgets
	controllerWidgets[PLAY] = (BWidgets::ValueWidget*) &playButton;
	controllerWidgets[PLAY_MODE] = (BWidgets::ValueWidget*) &playModeListBox;
	controllerWidgets[ON_MIDI] = (BWidgets::ValueWidget*) &onMidiListBox;
	controllerWidgets[AUTOPLAY_BPM] = (BWidgets::ValueWidget*) &autoplayBpmSlider;
	controllerWidgets[AUTOPLAY_BPB] = (BWidgets::ValueWidget*) &autoplayBpbSlider;
	controllerWidgets[STEPS] = (BWidgets::ValueWidget*) &stepsListBox;
	controllerWidgets[BASE] = (BWidgets::ValueWidget*) &sequenceBaseListBox;
	controllerWidgets[BASE_VALUE] = (BWidgets::ValueWidget*) &sequenceSizeSelect;
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_EFFECT] = (BWidgets::ValueWidget*) &slots[i].container;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_PLAY] = (BWidgets::ValueWidget*) &slots[i].playPad;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_ATTACK] = (BWidgets::ValueWidget*) &slotParams[i].attackSlider;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_DECAY] = (BWidgets::ValueWidget*) &slotParams[i].decaySlider;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_SUSTAIN] = (BWidgets::ValueWidget*) &slotParams[i].sustainSlider;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_RELEASE] = (BWidgets::ValueWidget*) &slotParams[i].releaseSlider;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_PAN] = (BWidgets::ValueWidget*) &slotParams[i].panDial;
		controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_MIX] = (BWidgets::ValueWidget*) &slotParams[i].mixDial;
		for (int j = 0; j < NR_OPTPARAMS; ++j)
		{
			controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + SLOTS_OPTPARAMS + j] = (BWidgets::ValueWidget*) &slotParams[i].options[j];
		}
	}

	// Set callback functions
	for (int i = 0; i < NR_CONTROLLERS; ++i) controllerWidgets[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
	bypassButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, playStopBypassChangedCallback);
	stopButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, playStopBypassChangedCallback);

	for (Slot& s : slots)
	{
		s.addPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, addClickedCallback);
		s.delPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, delClickedCallback);
		s.upPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, upClickedCallback);
		s.downPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, downClickedCallback);
		s.effectPad.button.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, menuClickedCallback);
		s.effectPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, effectClickedCallback);
		s.effectsListbox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, effectChangedCallback);
	}

	for (int i = 0; i < EDIT_RESET; ++i) edit1Buttons[i].setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, edit1ChangedCallback);
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i) edit2Buttons[i].setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, edit2ChangedCallback);

	// Configure widgets
	onMidiListBox.hide();
	midiButton.hide ();

	pattern.clear();
	padSurface.setDraggable (true);
	padSurface.setCallbackFunction (BEvents::BUTTON_PRESS_EVENT, padsPressedCallback);
	padSurface.setCallbackFunction (BEvents::BUTTON_RELEASE_EVENT, padsPressedCallback);
	padSurface.setCallbackFunction (BEvents::POINTER_DRAG_EVENT, padsPressedCallback);
	padSurface.setScrollable (true);
	padSurface.setCallbackFunction (BEvents::WHEEL_SCROLL_EVENT, padsScrolledCallback);
	padSurface.setFocusable (true);
	padSurface.setCallbackFunction (BEvents::FOCUS_IN_EVENT, padsFocusedCallback);
	padSurface.setCallbackFunction (BEvents::FOCUS_OUT_EVENT, padsFocusedCallback);

	slots[0].delPad.hide();
	slots[0].upPad.hide();
	slots[0].downPad.hide();
	slots[0].effectPad.hide();
	slots[0].playPad.hide();
	for (Slot& s : slots)
	{
		s.effectsListbox.setStacking (BWidgets::STACKING_OVERSIZE);
		s.effectsListbox.hide();
	}
	for (int i = 1; i < NR_SLOTS; ++i) slots[i].container.hide();

	for (SlotParam& s : slotParams)
	{
		s.container.hide();
		s.adsrDisplay.setFill (true);
		s.adsrDisplay.setScaleParameters (0.05, 0, 1.1);
	}

	// Load background & apply theme
	bgImageSurface = cairo_image_surface_create_from_png ((pluginPath + BG_FILE).c_str());
	widgetBg.loadFillFromCairoSurface (bgImageSurface);
	applyTheme (theme);

	// Pack widgets
	settingsContainer.add (playButton);
	settingsContainer.add (bypassButton);
	settingsContainer.add (stopButton);
	settingsContainer.add (playModeListBox);
	settingsContainer.add (onMidiListBox);
	settingsContainer.add (midiButton);
	settingsContainer.add (autoplayBpmLabel);
	settingsContainer.add (autoplayBpmSlider);
	settingsContainer.add (autoplayBpbLabel);
	settingsContainer.add (autoplayBpbSlider);
	settingsContainer.add (sequenceSizeSelect);
	settingsContainer.add (sequenceBaseListBox);
	settingsContainer.add (stepsListBox);

	for (Slot& s : slots)
	{
		s.container.add (s.addPad);
		s.container.add (s.delPad);
		s.container.add (s.upPad);
		s.container.add (s.downPad);
		s.container.add (s.effectPad);
		s.container.add (s.playPad);
		s.container.add (s.effectsListbox);
	}

	for (HaloToggleButton& e1 : edit1Buttons) editContainer.add (e1);
	for (HaloButton& e2 : edit2Buttons) editContainer.add (e2);

	for (SlotParam& s : slotParams)
	{
		s.container.add (s.nrIcon);
		s.container.add (s.nameIcon);
		s.container.add (s.attackLabel);
		s.container.add (s.decayLabel);
		s.container.add (s.sustainLabel);
		s.container.add (s.releaseLabel);
		s.container.add (s.attackSlider);
		s.container.add (s.decaySlider);
		s.container.add (s.sustainSlider);
		s.container.add (s.releaseSlider);
		s.container.add (s.adsrDisplay);
		s.container.add (s.panLabel);
		s.container.add (s.panDial);
		s.container.add (s.mixLabel);
		s.container.add (s.mixDial);
		for (BWidgets::ValueWidget& o : s.options) s.container.add (o);
	}

	padParamContainer.add (padGateLabel);
	padParamContainer.add (padGateDial);
	padParamContainer.add (padMixLabel);
	padParamContainer.add (padMixDial);

	mContainer.add (padParamContainer);
	for (SlotParam& s : slotParams) mContainer.add (s.container);
	mContainer.add (editContainer);
	for (Slot& s : slots) mContainer.add (s.container);
	mContainer.add (padSurface);
	mContainer.add (settingsContainer);
	mContainer.add (messageLabel);

	drawPad();
	add (mContainer);

	//Scan host features for URID map
	LV2_URID_Map* map = NULL;
	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0)
		{
			map = (LV2_URID_Map*) features[i]->data;
		}
	}
	if (!map) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	getURIs (map, &urids);

	// Initialize forge
	lv2_atom_forge_init (&forge, map);
}

BNoname01GUI::~BNoname01GUI ()
{
	for (SlotParam& s : slotParams)
	{
		if (s.optionWidget) delete (s.optionWidget);
	}

	sendUiOff ();
}

void BNoname01GUI::Pattern::clear ()
{
	Pad pad0 = Pad ();

	changes.oldMessage.clear ();
	changes.newMessage.clear ();
	journal.clear ();

	for (int r = 0; r < NR_SLOTS; ++r)
	{
		for (int s = 0; s < NR_STEPS; ++s)
		{
			setPad (r, s, pad0);
		}
	}

	store ();
}

Pad BNoname01GUI::Pattern::getPad (const size_t row, const size_t step) const
{
	return pads[LIMIT (row, 0, NR_SLOTS)][LIMIT (step, 0, NR_STEPS)];
}
void BNoname01GUI::Pattern::setPad (const size_t row, const size_t step, const Pad& pad)
{
	size_t r = LIMIT (row, 0, NR_SLOTS);
	size_t s = LIMIT (step, 0, NR_STEPS);
	changes.oldMessage.push_back (PadMessage (r, s, pads[r][s]));
	changes.newMessage.push_back (PadMessage (r, s, pad));
	pads[r][s] = pad;
}

std::vector<PadMessage> BNoname01GUI::Pattern::undo ()
{
	if (!changes.newMessage.empty ()) store ();

	std::vector<PadMessage> padMessages = journal.undo ();
	std::reverse (padMessages.begin (), padMessages.end ());
	for (PadMessage const& p : padMessages)
	{
		size_t r = LIMIT (p.row, 0, NR_SLOTS);
		size_t s = LIMIT (p.step, 0, NR_STEPS);
		pads[r][s] = Pad (p);
	}

	return padMessages;
}

std::vector<PadMessage> BNoname01GUI::Pattern::redo ()
{
	if (!changes.newMessage.empty ()) store ();

	std::vector<PadMessage> padMessages = journal.redo ();
	for (PadMessage const& p : padMessages)
	{
		size_t r = LIMIT (p.row, 0, NR_SLOTS);
		size_t s = LIMIT (p.step, 0, NR_STEPS);
		pads[r][s] = Pad (p);
	}

	return padMessages;
}

void BNoname01GUI::Pattern::store ()
{
	if (changes.newMessage.empty ()) return;

	journal.push (changes.oldMessage, changes.newMessage);
	changes.oldMessage.clear ();
	changes.newMessage.clear ();
}

void BNoname01GUI::port_event(uint32_t port, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	// Notify port
	if ((format == urids.atom_eventTransfer) && (port == NOTIFY))
	{
		const LV2_Atom* atom = (const LV2_Atom*) buffer;
		if ((atom->type == urids.atom_Blank) || (atom->type == urids.atom_Object))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*) atom;

			// Slot notification
			if (obj->body.otype == urids.bNoname01_slotEvent)
			{
				LV2_Atom *oSl = NULL, *oPd = NULL;
				int slot = -1;
				lv2_atom_object_get(obj,
						    urids.bNoname01_slot, &oSl,
						    urids.bNoname01_pads, &oPd,
						    NULL);

				if (oSl && (oSl->type == urids.atom_Int))
				{
					slot = ((LV2_Atom_Int*)oSl)->body;
				}

				if (oPd && (oPd->type == urids.atom_Vector) && (slot >= 0) && (slot < NR_SLOTS))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oPd;
					if (vec->body.child_type == urids.atom_Float)
					{
						if (wheelScrolled)
						{
							pattern.store ();
							wheelScrolled = false;
						}

						uint32_t size = (uint32_t) ((oPd->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (Pad));
						Pad* p = (Pad*)(&vec->body + 1);
						for (unsigned int i = 0; (i < size) && (i < NR_STEPS); ++i)
						{
							pattern.setPad (slot, i, p[i]);
						}
						pattern.store ();
						drawPad (slot);
					}
				}
			}

			// Message notification
			else if (obj->body.otype == urids.bNoname01_messageEvent)
			{
				const LV2_Atom* data = NULL;
				lv2_atom_object_get(obj, urids.bNoname01_message, &data, 0);
				if (data && (data->type == urids.atom_Int))
				{
					const int messageNr = ((LV2_Atom_Int*)data)->body;
					std::string msg = ((messageNr >= NO_MSG) && (messageNr < MAX_MSG) ? messageStrings[messageNr] : "");
					messageLabel.setText (msg);
				}
			}

			// Status notifications
			else if (obj->body.otype == urids.bNoname01_statusEvent)
			{
				LV2_Atom *oBpb = NULL, *oBu = NULL, *oBpm = NULL, *oPos = NULL;
				lv2_atom_object_get
				(
					obj,
					urids.time_beatsPerBar, &oBpb,
					urids.time_beatUnit, &oBu,
					urids.time_beatsPerMinute, &oBpm,
					urids.bNoname01_position, &oPos,
					NULL
				);

				if (oBpb && (oBpb->type == urids.atom_Float))
				{
					// TODO bpb = ((LV2_Atom_Float*)oBpb)->body;
				}

				if (oBu && (oBu->type == urids.atom_Int))
				{
					// TODO bU = (((LV2_Atom_Int*)oSched)->body);
				}

				if (oBpm && (oBpm->type == urids.atom_Float))
				{
					// TODO bpm = ((LV2_Atom_Float*)oBpm)->body;
				}

				if (oPos && (oPos->type == urids.atom_Double))
				{
					const double oCursor = cursor;
					const double nCursor = ((LV2_Atom_Double*)oPos)->body;
					cursor = nCursor;
					if (int (nCursor) != int (oCursor)) drawPad();
				}
			}
		}
	}

	// Scan remaining ports
	else if ((format == 0) && (port >= CONTROLLERS))
	{
		float* pval = (float*) buffer;
		controllerWidgets[port - CONTROLLERS]->setValue (*pval);
	}

}

void BNoname01GUI::resize ()
{
	hide ();
	//Scale fonts
	ctLabelFont.setFontSize (12 * sz);
	tLabelFont.setFontSize (12 * sz);
	tgLabelFont.setFontSize (12 * sz);
	lfLabelFont.setFontSize (12 * sz);
	smLabelFont.setFontSize (8 * sz);

	//Background
	cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1240 * sz, 608 * sz);
	cairo_t* cr = cairo_create (surface);
	cairo_scale (cr, sz, sz);
	cairo_set_source_surface(cr, bgImageSurface, 0, 0);
	cairo_paint(cr);
	widgetBg.loadFillFromCairoSurface(surface);
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	//Scale widgets
	RESIZE (mContainer, 0, 0, 1240, 608, sz);
	RESIZE (messageLabel, 400, 45, 600, 20, sz);

	// TODO The rest

	applyTheme (theme);
	drawPad ();
	show ();
}

void BNoname01GUI::applyTheme (BStyles::Theme& theme)
{
	mContainer.applyTheme (theme);
	messageLabel.applyTheme (theme);

	settingsContainer.applyTheme (theme);
	playButton.applyTheme (theme);
	bypassButton.applyTheme (theme);
	stopButton.applyTheme (theme);
	playModeListBox.applyTheme (theme);
	onMidiListBox.applyTheme (theme);
	midiButton.applyTheme (theme);
	autoplayBpmLabel.applyTheme (theme);
	autoplayBpmSlider.applyTheme (theme);
	autoplayBpbLabel.applyTheme (theme);
	autoplayBpbSlider.applyTheme (theme);
	sequenceSizeSelect.applyTheme (theme);
	sequenceBaseListBox.applyTheme (theme);
	stepsListBox.applyTheme (theme);

	for (Slot& s : slots)
	{
		s.container.applyTheme (theme);
		s.addPad.applyTheme (theme);
		s.delPad.applyTheme (theme);
		s.upPad.applyTheme (theme);
		s.downPad.applyTheme (theme);
		s.effectPad.applyTheme (theme);
		s.playPad.applyTheme (theme);
	};

	padSurface.applyTheme (theme);

	editContainer.applyTheme (theme);
	for (HaloToggleButton& e1 : edit1Buttons) e1.applyTheme (theme);
	for (HaloButton& e2 : edit2Buttons) e2.applyTheme (theme);;

	for (SlotParam& s : slotParams)
	{
		s.container.applyTheme (theme);
		s.nrIcon.applyTheme (theme);
		s.nameIcon.applyTheme (theme);
		s.attackLabel.applyTheme (theme);
		s.decayLabel.applyTheme (theme);
		s.sustainLabel.applyTheme (theme);
		s.releaseLabel.applyTheme (theme);
		s.attackSlider.applyTheme (theme);
		s.decaySlider.applyTheme (theme);;
		s.sustainSlider.applyTheme (theme);
		s.releaseSlider.applyTheme (theme);
		s.adsrDisplay.applyTheme (theme);
		s.panLabel.applyTheme (theme);
		s.panDial.applyTheme (theme);
		s.mixLabel.applyTheme (theme);
		s.mixDial.applyTheme (theme);
		for (BWidgets::ValueWidget& o : s.options) o.applyTheme (theme);
		if (s.optionWidget) s.optionWidget->applyTheme (theme);
	};

	padParamContainer.applyTheme (theme);
	padGateLabel.applyTheme (theme);
	padGateDial.applyTheme (theme);
	padMixLabel.applyTheme (theme);
	padMixDial.applyTheme (theme);
}

void BNoname01GUI::onConfigureRequest (BEvents::ExposeEvent* event)
{
	Window::onConfigureRequest (event);

	sz = (getWidth() / 1020 > getHeight() / 620 ? getHeight() / 620 : getWidth() / 1020);
	resize ();
}

void BNoname01GUI::sendUiOn ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_uiOn);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BNoname01GUI::sendUiOff ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_uiOff);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BNoname01GUI::sendSlot (const int slot)
{
	Pad pads[NR_STEPS];
	for (int i = 0; i < NR_STEPS; ++i) pads[i] = pattern.getPad (slot, i);
	uint8_t obj_buf[1024];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_padEvent);
	lv2_atom_forge_key(&forge, urids.bNoname01_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bNoname01_pads);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, NR_STEPS * sizeof(Pad) / sizeof(float), (void*) pads);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BNoname01GUI::sendPad (const int slot, const int step)
{
	Pad pad (pattern.getPad (slot, step));

	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bNoname01_padEvent);
	lv2_atom_forge_key(&forge, urids.bNoname01_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bNoname01_step);
	lv2_atom_forge_int(&forge, step);
	lv2_atom_forge_key(&forge, urids.bNoname01_pads);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, sizeof(Pad) / sizeof(float), (void*) &pad);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

int BNoname01GUI::getSlotsSize () const
{
	int slotSize = 0;
	while ((slotSize < NR_SLOTS) && (slots[slotSize].container.getValue() > FX_NONE) && (slots[slotSize].container.getValue() < NR_FX)) ++slotSize;
	return slotSize;
}

void BNoname01GUI::clearSlot (int slot)
{
	slots[slot].effectsListbox.hide();
	controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_EFFECT]->setValue (FX_NONE);
	for (int j = 0; j < NR_PARAMS; ++j)
	{
		controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + j]->setValue (fxDefaultValues[FX_NONE][j]);
	}

	for (int j = 0; j < NR_STEPS; ++j) pattern.setPad (slot, j, Pad());
	sendSlot (slot);
	drawPad (slot);
}

void BNoname01GUI::copySlot (int dest, int source)
{
	slots[dest].effectsListbox.hide();
	for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j)
	{
		controllerWidgets[SLOTS + dest * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (controllerWidgets[SLOTS + source * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue());
	}

	for (int j = 0; j < NR_STEPS; ++j) pattern.setPad (dest, j, pattern.getPad (source, j));
	sendSlot (dest);
	drawPad (dest);
}

void BNoname01GUI::insertSlot (int slot, const BNoname01EffectsIndex effect)
{
	if ((slot < 0) || (slot >= NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	if (slotSize >= NR_SLOTS) return;
	slot = LIMIT (slot, 0, slotSize);
	slots[slot].effectsListbox.hide();

	// Move slots, slotParams, pads
	for (int i = slotSize - 1; i >= slot; --i) copySlot (i + 1, i);

	// Cleanup: Clear slots, slotParams, pads
	for (int i = slotSize + 1; i < NR_SLOTS; ++i) clearSlot (i);

	// Set new slot, slotParams, pads with defaults
	controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_EFFECT]->setValue (effect);
	for (int j = 0; j < NR_PARAMS; ++j) controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + j]->setValue (fxDefaultValues[effect][j]);
	for (int j = 0; j < NR_STEPS; ++j) pattern.setPad (slot, j, Pad());

	pattern.store();
	updateSlots();
	sendSlot (slot);
	drawPad (slot);
}

void BNoname01GUI::deleteSlot (int slot)
{
	if ((slot < 0) || (slot >= NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	slot = LIMIT (slot, 0, slotSize - 1);

	// Move slots, slotParams, pads
	for (int i = slot; i < slotSize - 1; ++i) copySlot (i, i + 1);

	// Cleanup: Clear slots, slotParams, pads
	for (int i = slotSize - 1; i < NR_SLOTS; ++i) clearSlot (i);

	pattern.store();
	updateSlots();
}

void BNoname01GUI::swapSlots (int slot1, int slot2)
{
	if ((slot1 < 0) || (slot1 >= NR_SLOTS) || (slot2 < 0) || (slot2 >= NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	slot1 = LIMIT (slot1, 0, slotSize - 1);
	slot2 = LIMIT (slot2, 0, slotSize - 1);
	slots[slot1].effectsListbox.hide();
	slots[slot2].effectsListbox.hide();

	// Swap pads
	for (int j = 0; j < NR_STEPS; ++j)
	{
		Pad slot1Pad = pattern.getPad (slot1, j);
		pattern.setPad (slot1, j, pattern.getPad (slot2, j));
		pattern.setPad (slot2, j, slot1Pad);
	}

	// Swap slots
	for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j)
	{
		double slot1Value = controllerWidgets[SLOTS + slot1 * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue();
		controllerWidgets[SLOTS + slot1 * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (controllerWidgets[SLOTS + slot2 * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue());
		controllerWidgets[SLOTS + slot2 * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (slot1Value);
	}

	pattern.store();
	updateSlot (slot1);
	sendSlot (slot1);
	drawPad (slot1);
	updateSlot (slot2);
	sendSlot (slot2);
	drawPad (slot2);
}

void BNoname01GUI::updateSlot (const int slot)
{
	const int slotSize = getSlotsSize();

	if (slot <= slotSize)
	{
		if (slotSize < NR_SLOTS)
		{
			slots[slot].addPad.setClickable (true);
			slots[slot].addPad.setSymbol (ADDSYMBOL);
		}
		else
		{
			slots[slot].addPad.setClickable (false);
			slots[slot].addPad.setSymbol (NOSYMBOL);
		}

		slots[slot].addPad.show();
	}
	else slots[slot].addPad.hide();

	if (slot < slotSize)
	{
		if (slot == 0)
		{
			slots[slot].upPad.setClickable (false);
			slots[slot].upPad.setSymbol (NOSYMBOL);
		}
		else
		{
			slots[slot].upPad.setClickable (true);
			slots[slot].upPad.setSymbol (UPSYMBOL);
		}

		if (slot == slotSize - 1)
		{
			slots[slot].downPad.setClickable (false);
			slots[slot].downPad.setSymbol (NOSYMBOL);
		}
		else
		{
			slots[slot].downPad.setClickable (true);
			slots[slot].downPad.setSymbol (DOWNSYMBOL);
		}

		slots[slot].upPad.show();
		slots[slot].downPad.show();

		slots[slot].delPad.show();
		slots[slot].effectPad.show();
		slots[slot].playPad.show();
	}
	else
	{
		slots[slot].upPad.hide();
		slots[slot].downPad.hide();
		slots[slot].delPad.hide();
		slots[slot].effectPad.hide();
		slots[slot].effectsListbox.hide();
		slots[slot].playPad.hide();
	}

	if (slot <= slotSize) slots[slot].container.show();
	else slots[slot].container.hide();
}

void BNoname01GUI::updateSlots ()
{
	for (int i = 0; i < NR_SLOTS; ++i) updateSlot (i);
}

void BNoname01GUI::gotoSlot (const int slot)
{
	actSlot = slot;
	const int slotSize = getSlotsSize();
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if ((i == slot) && (i < slotSize)) slotParams[i].container.show();
		else slotParams[i].container.hide();
	}
	drawPad();
}

void BNoname01GUI::setOptionWidget (const int slot)
{
	// Firstly delete old optionWidget
	if (slotParams[slot].optionWidget)
	{
		delete slotParams[slot].optionWidget;
		slotParams[slot].optionWidget = nullptr;
	}

	// Create new optionWidget
	const double v = slots[slot].container.getValue();
	const int fxnr = ((v >= FX_NONE) && (v < NR_FX) ? v : FX_NONE);

	switch (fxnr)
	{
		case FX_AMP:		slotParams[slot].optionWidget = new OptionAmp (430, 20, 80, 130, "widget");
					break;

		case FX_BALANCE:	slotParams[slot].optionWidget = new OptionBalance (430, 20, 80, 130, "widget");
					break;

		case FX_WIDTH:		slotParams[slot].optionWidget = new OptionWidth (430, 20, 80, 130, "widget");
					break;

		case FX_DELAY:		slotParams[slot].optionWidget = new OptionDelay (430, 20, 160, 130, "widget");
					break;

		case FX_CHOPPER:	slotParams[slot].optionWidget = new OptionChopper (430, 20, 480, 130, "widget");
					break;

		case FX_TAPE_STOP:	slotParams[slot].optionWidget = new OptionTapeStop (430, 20, 160, 130, "widget");
					break;

		case FX_TAPE_SPEED:	slotParams[slot].optionWidget = new OptionTapeSpeed (430, 20, 80, 130, "widget");
					break;

		case FX_BITCRUSH:	slotParams[slot].optionWidget = new OptionBitcrush (430, 20, 160, 130, "widget");
					break;

		case FX_DECIMATE:	slotParams[slot].optionWidget = new OptionDecimate (430, 20, 80, 130, "widget");
					break;

		case FX_DISTORTION:	slotParams[slot].optionWidget = new OptionDistortion (430, 20, 240, 130, "widget");
					break;

		case FX_NOISE:		slotParams[slot].optionWidget = new OptionNoise (430, 20, 80, 130, "widget");
					break;

		default:		slotParams[slot].optionWidget = new OptionWidget (0, 0, 0, 0, "widget");
	}
}

void BNoname01GUI::loadOptions (const int slot)
{
	// Copy parameters and add optionWidget
	if (slotParams[slot].optionWidget)
	{
		slotParams[slot].container.add (*slotParams[slot].optionWidget);

		for (int i = 0; i < NR_OPTPARAMS; ++i)
		{
			if (slotParams[slot].optionWidget->getWidget (i))
			{
				slotParams[slot].optionWidget->setOption (i, slotParams[slot].options[i].getValue());
				const std::string oName = slotParams[slot].optionWidget->getWidget (i)->getName();
				if (oName.substr (0, 3) == "pad") slotParams[slot].optionWidget->getWidget (i)->rename (slotParams[slot].adsrDisplay.getName());
			}
		}
	}

}

void BNoname01GUI::optionChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	double value = widget->getValue();
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	for (SlotParam& s : ui->slotParams)
	{
		if (s.optionWidget)
		{
			for (int i = 0; i < NR_OPTPARAMS; ++i)
			{
				if (s.optionWidget->getWidget (i) && ((BWidgets::ValueWidget*)s.optionWidget->getWidget (i) == widget))
				{
					s.options[i].setValue (value);
					return;
				}
			}
		}
	}
}

void BNoname01GUI::valueChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int controllerNr = -1;

	// Identify controller
	for (int i = 0; i < NR_CONTROLLERS; ++i)
	{
		if (widget == ui->controllerWidgets[i])
		{
			controllerNr = i;
			break;
		}
	}

	// Controllers
	if (controllerNr >= 0)
	{
		ui->write_function(ui->controller, CONTROLLERS + controllerNr, sizeof(float), 0, &value);

		switch (controllerNr)
		{
			case PLAY:		ui->bypassButton.setValue (value == 2.0 ? 1 : 0);
						break;

			case PLAY_MODE:		if  (value == AUTOPLAY)
						{
							ui->autoplayBpmLabel.show();
							ui->autoplayBpmSlider.show();
							ui->autoplayBpbLabel.show();
							ui->autoplayBpbSlider.show();
						}
						else
						{
							ui->autoplayBpmLabel.hide();
							ui->autoplayBpmSlider.hide();
							ui->autoplayBpbLabel.hide();
							ui->autoplayBpbSlider.hide();
						}

						if  (value == MIDI_CONTROLLED)
						{
							ui->onMidiListBox.show();
							ui->midiButton.show();
						}
						else
						{
							ui->onMidiListBox.hide();
							ui->midiButton.hide();
						}

						break;

			case STEPS:		ui->drawPad();
						break;

			default:		if (controllerNr >= SLOTS)
						{
							int slot = (controllerNr - SLOTS) / (SLOTS_PARAMS + NR_PARAMS);
							int param = (controllerNr - SLOTS) % (SLOTS_PARAMS + NR_PARAMS);

							if (param == SLOTS_EFFECT)
							{
								if (value == UNSELECTED)
								{
									ui->clearSlot (slot);
									value = FX_NONE;
								}
								else
								{
									// Keep in limits
									const int fxnr = LIMIT (value, FX_NONE, NR_FX - 1);
									if (value != fxnr) ui->slots[slot].container.setValue (fxnr);

									// Change widget colors
									const std::string padstr = "pad" + std::to_string (fxnr);
									ui->slots[slot].effectsListbox.hide();
									ui->slots[slot].effectPad.loadImage (BColors::NORMAL, ui->pluginPath + "inc/" + fxIconFileNames[fxnr] + ".png");
									ui->slots[slot].addPad.rename (padstr);
									ui->slots[slot].delPad.rename (padstr);
									ui->slots[slot].upPad.rename (padstr);
									ui->slots[slot].downPad.rename (padstr);
									ui->slots[slot].effectPad.rename (padstr);
									ui->slots[slot].playPad.rename (padstr);
									ui->slotParams[slot].adsrDisplay.rename (padstr);
									ui->slotParams[slot].attackSlider.rename (padstr);
									ui->slotParams[slot].decaySlider.rename (padstr);
									ui->slotParams[slot].sustainSlider.rename (padstr);
									ui->slotParams[slot].releaseSlider.rename (padstr);
									ui->slotParams[slot].panDial.rename (padstr);
									ui->slotParams[slot].mixDial.rename (padstr);

									// Set new optionWidget
									ui->setOptionWidget (slot);

									// Change by effectsListbox ? Set defauls
									if (fxnr == ui->slots[slot].effectsListbox.getValue())
									{
										for (int i = 0; i < NR_PARAMS; ++i)
										{
											ui->controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + i]->setValue
											(fxDefaultValues[fxnr][i]);
										}
									}
									else ui->slots[slot].effectsListbox.setValue (fxnr);

									ui->slotParams[slot].nrIcon.loadImage (BColors::NORMAL, ui->pluginPath + "inc/Nr" + std::to_string (slot + 1) + ".png");
									ui->slotParams[slot].nameIcon.loadImage (BColors::NORMAL, ui->pluginPath + "inc/" + fxIconFileNames[fxnr] + "16.png");

									// Load options to optionWidget
									ui->loadOptions (slot);

									ui->applyTheme(ui->theme);
									ui->drawPad (slot);
								}
							}

							else if
							(
								(param == SLOTS_PARAMS + SLOTS_ATTACK) ||
								(param == SLOTS_PARAMS + SLOTS_DECAY) ||
								(param == SLOTS_PARAMS + SLOTS_SUSTAIN) ||
								(param == SLOTS_PARAMS + SLOTS_RELEASE)
							)
							{
								const double a = ui->slotParams[slot].attackSlider.getValue();
								const double d = ui->slotParams[slot].decaySlider.getValue();
								const double s = ui->slotParams[slot].sustainSlider.getValue();
								const double r = ui->slotParams[slot].releaseSlider.getValue();
								const double adr = (a + d + r <= 1.0 ? 1.0 : a + d + r);
								ui->slotParams[slot].adsrDisplay.setPoints
								(
									std::vector<BUtilities::Point>
									({
										BUtilities::Point (0, 0),
										BUtilities::Point (a / adr, 1.0),
										BUtilities::Point ((a + d) / adr, s),
										BUtilities::Point (1.0 - r / adr, s),
										BUtilities::Point (1, 0)
									})
								);
							}

							else if
							(
								(param >= SLOTS_PARAMS + SLOTS_OPTPARAMS) &&
								(param < SLOTS_PARAMS + SLOTS_OPTPARAMS + NR_OPTPARAMS) &&
								ui->slotParams[slot].optionWidget &&
								ui->slotParams[slot].optionWidget->getWidget (param - SLOTS_PARAMS - SLOTS_OPTPARAMS)
							) ui->slotParams[slot].optionWidget->setOption (param - SLOTS_PARAMS - SLOTS_OPTPARAMS, value);

						}
						break;
		}

		ui->write_function(ui->controller, CONTROLLERS + controllerNr, sizeof(float), 0, &value);
	}
}

void BNoname01GUI::playStopBypassChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	if (widget == &ui->bypassButton)
	{
		if ((value == 0.0) && (ui->playButton.getValue() == 2.0)) ui->playButton.setValue (0.0);
		else if (value == 1.0) ui->playButton.setValue (2.0);
	}

	else if (widget == &ui->stopButton)
	{
		if (value == 1.0)
		{
			ui->playButton.setValue (0.0);
			ui->bypassButton.setValue (0.0);
		}
	}
}

void BNoname01GUI::effectChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ListBox* widget = (BWidgets::ListBox*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].effectsListbox)
		{
			if (value != ui->slots[i].container.getValue())
			{
				ui->slots[i].container.setValue (value);
			}
			break;
		}
	}
}

void BNoname01GUI::addClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].addPad)
		{
			slot = i;
			break;
		}
	}

	// Add slot
	if (slot >= 0)
	{
		ui->insertSlot (slot, FX_AMP);
		ui->gotoSlot (slot);
	}
}

void BNoname01GUI::delClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].delPad)
		{
			slot = i;
			break;
		}
	}

	// Delete slot
	if (slot >= 0)
	{
		ui->deleteSlot (slot);
		ui->gotoSlot (slot);
	}
}

void BNoname01GUI::upClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].upPad)
		{
			slot = i;
			break;
		}
	}

	// Swap slots
	if (slot >= 1)
	{
		ui->swapSlots (slot - 1, slot);
		ui->gotoSlot (slot - 1);
	}
}



void BNoname01GUI::downClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].downPad)
		{
			slot = i;
			break;
		}
	}

	// Swap slots
	if ((slot >= 0) && (slot < ui->getSlotsSize() - 1))
	{
		ui->swapSlots (slot, slot + 1);
		ui->gotoSlot (slot + 1);
	}
}

void BNoname01GUI::menuClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ImageIcon* widget = (BWidgets::ImageIcon*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].effectPad.button)
		{
			slot = i;
			break;
		}
	}

	// Show / hide menu
	if (slot >= 0)
	{
		ui->slots[slot].container.raiseToTop();

		if (ui->slots[slot].effectsListbox.isVisible()) ui->slots[slot].effectsListbox.hide();
		else ui->slots[slot].effectsListbox.show();

		ui->gotoSlot (slot);
	}
}



void BNoname01GUI::effectClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	IconPadButton* widget = (IconPadButton*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	int slot = -1;

	// Identify slot
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].effectPad)
		{
			slot = i;
			break;
		}
	}

	// Change slot
	if (slot >= 0) ui->gotoSlot (slot);
}


void BNoname01GUI::edit1ChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	if (value != 1.0) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	// Identify editButtons: CUT ... PASTE
	int widgetNr = -1;
	for (int i = 0; i < EDIT_RESET; ++i)
	{
		if (widget == &ui->edit1Buttons[i])
		{
			widgetNr = i;
			break;
		}
	}

	// Untoggle all other edit1Buttons
	if (widgetNr >= 0)
	{
		// Allow only one button pressed
		for (int i = 0; i < EDIT_RESET; ++i)
		{
			if (i != widgetNr) ui->edit1Buttons[i].setValue (0.0);
		}
	}
}

void BNoname01GUI::edit2ChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	if (value != 1.0) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	// Identify editButtons: RESET ... REDO
	int widgetNr = -1;
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i)
	{
		if (widget == &ui->edit2Buttons[i])
		{
			widgetNr = i + EDIT_RESET;
			break;
		}
	}

	// RESET ... REDO
	switch (widgetNr)
	{
		case EDIT_RESET:
		{
			if (ui->wheelScrolled)
			{
				ui->pattern.store ();
				ui->wheelScrolled = false;
			}

			for (int r = 0; r < NR_SLOTS; ++r)
			{
				for (int s = 0; s < NR_STEPS; ++s)
				{
					if (s == r) ui->pattern.setPad (r, s, Pad ());
					ui->sendPad (r, s);
				}
			}

			ui->drawPad ();
			ui->pattern.store ();
		}
		break;

		case EDIT_UNDO:
		{
			std::vector<PadMessage> padMessages = ui->pattern.undo ();
			for (PadMessage const& p : padMessages)
			{
				size_t r = LIMIT (p.row, 0, NR_SLOTS);
				size_t s = LIMIT (p.step, 0, NR_STEPS);
				ui->sendPad (r, s);
			}
			ui->drawPad ();
		}
		break;

		case EDIT_REDO:
		{
			std::vector<PadMessage> padMessages = ui->pattern.redo ();
			for (PadMessage const& p : padMessages)
			{
				size_t r = LIMIT (p.row, 0, NR_SLOTS);
				size_t s = LIMIT (p.step, 0, NR_STEPS);
				ui->sendPad (r, s);
			}
			ui->drawPad ();
		}
		break;

		default:	break;
	}
}

void BNoname01GUI::padsPressedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::PointerEvent* pointerEvent = (BEvents::PointerEvent*) event;
	BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	if
	(
		(event->getEventType () == BEvents::BUTTON_PRESS_EVENT) ||
		(event->getEventType () == BEvents::BUTTON_RELEASE_EVENT) ||
		(event->getEventType () == BEvents::POINTER_DRAG_EVENT)
	)
	{
		if (ui->wheelScrolled)
		{
			ui->pattern.store ();
			ui->wheelScrolled = false;
		}

		// Get size of drawing area
		const double width = ui->padSurface.getEffectiveWidth ();
		const double height = ui->padSurface.getEffectiveHeight ();

		const int maxstep = ui->controllerWidgets[STEPS]->getValue ();
		const int step =  int ((pointerEvent->getPosition ().x - widget->getXOffset()) / (width / double (maxstep)));
		const int row = int (pointerEvent->getPosition ().y - widget->getYOffset()) / (height / double (NR_SLOTS));

		if ((event->getEventType () == BEvents::BUTTON_PRESS_EVENT) || (event->getEventType () == BEvents::POINTER_DRAG_EVENT))
		{

			if ((row >= 0) && (row < NR_SLOTS) && (step >= 0) && (step < maxstep))
			{
				const Pad oldPad = ui->pattern.getPad (row, step);

				// Left button
				if (pointerEvent->getButton() == BDevices::LEFT_BUTTON)
				{
					// Check if edit mode
					int editNr = -1;
					for (int i = 0; i < EDIT_RESET; ++i)
					{
						if (ui->edit1Buttons[i].getValue() != 0.0)
						{
							editNr = i;
							break;
						}
					}

					// Edit
					if (editNr >= 0)
					{
						if ((editNr == EDIT_CUT) || (editNr == EDIT_COPY) || (editNr == EDIT_XFLIP) || (editNr == EDIT_YFLIP))
						{
							if (ui->clipBoard.ready)
							{
								ui->clipBoard.origin = std::make_pair (row, step);
								ui->clipBoard.extends = std::make_pair (0, 0);
								ui->clipBoard.ready = false;
								ui->drawPad (row, step);
							}

							else
							{
								std::pair<int, int> newExtends = std::make_pair (row - ui->clipBoard.origin.first, step - ui->clipBoard.origin.second);
								if (newExtends != ui->clipBoard.extends)
								{
									ui->clipBoard.extends = newExtends;
									ui->drawPad ();
								}
							}
						}

						else if (editNr == EDIT_PASTE)
						{
							if (!ui->clipBoard.data.empty ())
							{
								for (int r = 0; r < int (ui->clipBoard.data.size ()); ++r)
								{
									for (int s = 0; s < int (ui->clipBoard.data[r].size ()); ++s)
									{
										if
										(
											(row + r >= 0) &&
											(row + r < maxstep) &&
											(step + s >= 0) &&
											(step + s < maxstep)
										)
										{
											ui->pattern.setPad (row + r, step + s, ui->clipBoard.data.at(r).at(s));
											ui->sendPad (row + r, step + s);
											ui->drawPad (row + r, step + s);
										}
									}
								}
							}
						}

					}

					// Set (or unset) pad
					else
					{
						if (!ui->padPressed) ui->deleteMode =
						(
							(oldPad.gate == float (ui->padGateDial.getValue())) &&
							(oldPad.mix == float (ui->padMixDial.getValue()))
						);
						Pad newPad = (ui->deleteMode ? Pad () : Pad (ui->padGateDial.getValue(), 1.0, ui->padMixDial.getValue()));
						ui->pattern.setPad (row, step, newPad);
						ui->sendPad (row, step);
						ui->drawPad (row,step);
					}

					ui->padPressed = true;
				}

				else if (pointerEvent->getButton() == BDevices::RIGHT_BUTTON)
				{
					ui->padGateDial.setValue (ui->pattern.getPad (row, step).gate);
					ui->padMixDial.setValue (ui->pattern.getPad (row, step).mix);
				}
			}
		}

		else if ((event->getEventType () == BEvents::BUTTON_RELEASE_EVENT) && (pointerEvent->getButton() == BDevices::LEFT_BUTTON))
		{
			// Check if edit mode
			int editNr = -1;
			for (int i = 0; i < EDIT_RESET; ++i)
			{
				if (ui->edit1Buttons[i].getValue() != 0.0)
				{
					editNr = i;
					break;
				}
			}

			// Edit mode
			if (editNr >= 0)
			{
				if ((editNr == EDIT_CUT) || (editNr == EDIT_COPY) || (editNr == EDIT_XFLIP) || (editNr == EDIT_YFLIP))
				{
					int clipRMin = ui->clipBoard.origin.first;
					int clipRMax = ui->clipBoard.origin.first + ui->clipBoard.extends.first;
					if (clipRMin > clipRMax) std::swap (clipRMin, clipRMax);
					int clipSMin = ui->clipBoard.origin.second;
					int clipSMax = ui->clipBoard.origin.second + ui->clipBoard.extends.second;
					if (clipSMin > clipSMax) std::swap (clipSMin, clipSMax);

					// XFLIP
					if (editNr == EDIT_XFLIP)
					{
						for (int r = clipRMin; r <= clipRMax; ++r)
						{
							for (int ds = 0; ds < int ((clipSMax + 1 - clipSMin) / 2); ++ds)
							{

								const Pad pd = ui->pattern.getPad (r, clipSMin + ds);
								ui->pattern.setPad (r, clipSMin + ds, ui->pattern.getPad (r, clipSMax - ds));
								ui->sendPad (r, clipSMin + ds);
								ui->pattern.setPad (r, clipSMax - ds, pd);
								ui->sendPad (r, clipSMax - ds);
							}
						}
						ui->pattern.store ();
						ui->drawPad();
					}

					// YFLIP
					if (editNr == EDIT_YFLIP)
					{
						for (int dr = 0; dr <= int ((clipRMax + 1 - clipRMin) / 2); ++dr)
						{
							for (int s = 0; s < clipSMax; ++s)
							{

								const Pad pd = ui->pattern.getPad (clipRMin + dr, s);
								ui->pattern.setPad (clipRMin + dr, s, ui->pattern.getPad (clipRMax - dr, s));
								ui->sendPad (clipRMin + dr, s);
								ui->pattern.setPad (clipRMax - dr, s, pd);
								ui->sendPad (clipRMax - dr, s);
							}
						}
						ui->pattern.store ();
						ui->drawPad();
					}

					// Store selected data in clipboard after flip (XFLIP, YFLIP)
					// Or store selected data in clipboard before deletion (CUT)
					// Or store selected data anyway (COPY)
					ui->clipBoard.data.clear ();
					for (int r = clipRMin; r <= clipRMax; ++r)
					{
						std::vector<Pad> padRow;
						padRow.clear ();
						for (int s = clipSMin; s <= clipSMax; ++s) padRow.push_back (ui->pattern.getPad (r, s));
						ui->clipBoard.data.push_back (padRow);
					}

					// CUT
					if (editNr == EDIT_CUT)
					{
						for (int s = clipSMin; s <= clipSMax; ++s)
						{
							for (int r = clipRMin; r <= clipRMax; ++r)
							{
								// Limit action to not empty pads
								if (ui->pattern.getPad (r, s) != Pad())
								{
									ui->pattern.setPad (r, s,  Pad ());
									ui->sendPad (r, s);
								}
							}
						}
						ui->pattern.store ();
					}

					ui->clipBoard.ready = true;
					ui->drawPad ();
				}
			}

			else
			{
				ui->padPressed = false;
				ui->pattern.store ();
			}
		}
	}
}

void BNoname01GUI::padsScrolledCallback (BEvents::Event* event)
{
	if ((event) && (event->getWidget ()) && (event->getWidget()->getMainWindow()) &&
		((event->getEventType () == BEvents::WHEEL_SCROLL_EVENT)))
	{
		BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
		BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
		BEvents::WheelEvent* wheelEvent = (BEvents::WheelEvent*) event;

		// Get size of drawing area
		const double width = ui->padSurface.getEffectiveWidth ();
		const double height = ui->padSurface.getEffectiveHeight ();

		const int maxstep = ui->controllerWidgets[STEPS]->getValue ();
		const int step =  int ((wheelEvent->getPosition ().x - widget->getXOffset()) / (width / double (maxstep)));
		const int row = int (wheelEvent->getPosition ().y - widget->getYOffset()) / (height / double (NR_SLOTS));

		if ((row >= 0) && (row < NR_SLOTS) && (step >= 0) && (step < maxstep))
		{
			Pad pd = ui->pattern.getPad (row, step);
			pd.mix = LIMIT (pd.mix + 0.01 * wheelEvent->getDelta().y, 0.0, 1.0);
			ui->pattern.setPad (row, step, pd);
			ui->sendPad (row, step);
			ui->drawPad (row, step);
			ui->wheelScrolled = true;
		}
	}
}

void BNoname01GUI::padsFocusedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::FocusEvent* focusEvent = (BEvents::FocusEvent*) event;
	BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
	if (!widget) return;
	BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
	if (!ui) return;

	// Get size of drawing area
	const double width = ui->padSurface.getEffectiveWidth ();
	const double height = ui->padSurface.getEffectiveHeight ();

	const int maxstep = ui->controllerWidgets[STEPS]->getValue ();
	const int step =  int ((focusEvent->getPosition ().x - widget->getXOffset()) / (width / double (maxstep)));
	const int row = int (focusEvent->getPosition ().y - widget->getYOffset()) / (height / double (NR_SLOTS));

	if ((row >= 0) && (row < NR_SLOTS) && (step >= 0) && (step < maxstep))
	{
		const Pad pd = ui->pattern.getPad (row, step);

		ui->padSurface.focusText.setText
		(
			"Row: " + std::to_string (row + 1) + "\n" +
			"Step: " + std::to_string (step + 1) + "\n" +
			"Size: " + BUtilities::to_string (pd.size, "%1.0f \n") +
			"Gate: " + BUtilities::to_string (pd.gate, "%1.2f \n") +
			"Mix: " + BUtilities::to_string (pd.mix, "%1.2f")
		);
	}
}

void BNoname01GUI::drawPad ()
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	int maxstep = controllerWidgets[STEPS]->getValue ();
	for (int row = 0; row < NR_SLOTS; ++row)
	{
		for (int step = 0; step < maxstep; ++step) drawPad (cr, row, step);
	}
	cairo_destroy (cr);
	padSurface.update();
}

void BNoname01GUI::drawPad (const int slot)
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	int maxstep = controllerWidgets[STEPS]->getValue ();
	for (int step = 0; step < maxstep; ++step) drawPad (cr, slot, step);
	cairo_destroy (cr);
	padSurface.update();
}

void BNoname01GUI::drawPad (const int row, const int step)
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	drawPad (cr, row, step);
	cairo_destroy (cr);
	padSurface.update();
}

void BNoname01GUI::drawPad (cairo_t* cr, const int row, const int step)
{
	int maxstep = controllerWidgets[STEPS]->getValue ();
	if ((!cr) || (cairo_status (cr) != CAIRO_STATUS_SUCCESS) || (row < 0) || (row >= NR_SLOTS) || (step < 0) || (step >= maxstep)) return;

	// Get size of drawing area
	const double width = padSurface.getEffectiveWidth ();
	const double height = padSurface.getEffectiveHeight ();
	const double w = width / maxstep;
	const double h = height / NR_SLOTS;
	const double x = step * w;
	const double y = row * h;
	const double xr = round (x);
	const double yr = round (y);
	const double wr = round (x + w) - xr;
	const double hr = round (y + h) - yr;

	// Draw background
	// Odd or even?
	BColors::Color bg = ((int (step / 4) % 2) ? oddPadBgColor : evenPadBgColor);
	if (actSlot == row) bg.applyBrightness (0.2);

	// Highlight selection
	int clipRMin = clipBoard.origin.first;
	int clipRMax = clipBoard.origin.first + clipBoard.extends.first;
	if (clipRMin > clipRMax) std::swap (clipRMin, clipRMax);
	int clipSMin = clipBoard.origin.second;
	int clipSMax = clipBoard.origin.second + clipBoard.extends.second;
	if (clipSMin > clipSMax) std::swap (clipSMin, clipSMax);
	if ((!clipBoard.ready) && (row >= clipRMin) && (row <= clipRMax) && (step >= clipSMin) && (step <= clipSMax)) bg.applyBrightness (0.75);

	cairo_set_source_rgba (cr, CAIRO_RGBA (bg));
	cairo_set_line_width (cr, 0.0);
	cairo_rectangle (cr, xr, yr, wr, hr);
	cairo_fill (cr);

	// Draw pad
	const int fxnr = LIMIT (slots[row].container.getValue(), FX_NONE, NR_FX - 1);
	BColors::Color color = *padColors[fxnr].getColor(BColors::NORMAL);
	color.applyBrightness (pattern.getPad (row, step).mix - 1.0);
	if (step == int (cursor)) color.applyBrightness (0.75);
	drawButton (cr, xr + 1, yr + 1, wr - 2, hr - 2, color);
}

LV2UI_Handle instantiate (const LV2UI_Descriptor *descriptor,
						  const char *plugin_uri,
						  const char *bundle_path,
						  LV2UI_Write_Function write_function,
						  LV2UI_Controller controller,
						  LV2UI_Widget *widget,
						  const LV2_Feature *const *features)
{
	PuglNativeWindow parentWindow = 0;
	LV2UI_Resize* resize = NULL;

	if (strcmp(plugin_uri, BNONAME01_URI) != 0)
	{
		std::cerr << "BNoname01.lv2#GUI: GUI does not support plugin with URI " << plugin_uri << std::endl;
		return NULL;
	}

	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = (PuglNativeWindow) features[i]->data;
		else if (!strcmp(features[i]->URI, LV2_UI__resize)) resize = (LV2UI_Resize*)features[i]->data;
	}
	if (parentWindow == 0) std::cerr << "BNoname01.lv2#GUI: No parent window.\n";

	// New instance
	BNoname01GUI* ui;
	try {ui = new BNoname01GUI (bundle_path, features, parentWindow);}
	catch (std::exception& exc)
	{
		std::cerr << "BNoname01.lv2#GUI: Instantiation failed. " << exc.what () << std::endl;
		return NULL;
	}

	ui->controller = controller;
	ui->write_function = write_function;

	// Reduce min GUI size for small displays
	double sz = 1.0;
	int screenWidth  = getScreenWidth ();
	int screenHeight = getScreenHeight ();
	if ((screenWidth < 860) || (screenHeight < 450)) sz = 0.5;
	else if ((screenWidth < 1280) || (screenHeight < 650)) sz = 0.66;

	if (resize) resize->ui_resize(resize->handle, 1240 * sz, 608 * sz);

	*widget = (LV2UI_Widget) puglGetNativeWindow (ui->getPuglView ());

	ui->sendUiOn();

	return (LV2UI_Handle) ui;
}

void cleanup(LV2UI_Handle ui)
{
	BNoname01GUI* self = (BNoname01GUI*) ui;
	delete self;
}

void port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	BNoname01GUI* self = (BNoname01GUI*) ui;
	self->port_event(port_index, buffer_size, format, buffer);
}

static int call_idle (LV2UI_Handle ui)
{
	BNoname01GUI* self = (BNoname01GUI*) ui;
	self->handleEvents ();
	return 0;
}

static const LV2UI_Idle_Interface idle = { call_idle };

static const void* extension_data(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) return &idle;
	else return NULL;
}

const LV2UI_Descriptor guiDescriptor = {
		BNONAME01_GUI_URI,
		instantiate,
		cleanup,
		port_event,
		extension_data
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
	switch (index) {
	case 0: return &guiDescriptor;
	default:return NULL;
    }
}

/* End of LV2 specific declarations
 *
 * *****************************************************************************
 *
 *
 */
