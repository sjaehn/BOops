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

#include <limits.h>		// PATH_MAX
#include "BOopsGUI.hpp"
#include "BUtilities/to_string.hpp"
#include "BUtilities/vsystem.hpp"
#include "getURIs.hpp"
#include "MessageDefinitions.hpp"
#include "FxDefaults.hpp"
#include "MidiDefs.hpp"

#include "OptionWidget.hpp"
#include "OptionSurprise.hpp"
#include "OptionAmp.hpp"
#include "OptionBalance.hpp"
#include "OptionWidth.hpp"
#include "OptionDelay.hpp"
#include "OptionChopper.hpp"
#include "OptionTapeStop.hpp"
#include "OptionTapeSpeed.hpp"
#include "OptionScratch.hpp"
#include "OptionWowFlutter.hpp"
#include "OptionBitcrush.hpp"
#include "OptionDecimate.hpp"
#include "OptionDistortion.hpp"
#include "OptionFilter.hpp"
#include "OptionNoise.hpp"
#include "OptionCrackles.hpp"
#include "OptionStutter.hpp"
#include "OptionFlanger.hpp"
#include "OptionPhaser.hpp"
#include "OptionRingModulator.hpp"
#include "OptionOops.hpp"
#include "OptionWah.hpp"

inline double floorfrac (const double value) {return value - floor (value);}
inline double floormod (const double numer, const double denom) {return numer - floor(numer / denom) * denom;}

BOopsGUI::BOopsGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow) :
	Window (1240, 648, "B.Oops", parentWindow, true, PUGL_MODULE, 0),
	controller (NULL), write_function (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) + ((strlen (bundle_path) > 0) && (bundle_path[strlen (bundle_path) - 1] != BUTILITIES_PATH_SLASH[0]) ? BUTILITIES_PATH_SLASH : "") : std::string ("")),
	sz (1.0), bgImageSurface (nullptr),
	samplePath ("."), sampleStart (0), sampleEnd (0), sampleLoop (false),
	urids (), forge (),
	pageAct (0),
	pageMax (0),
	pageOffset (0),
	patterns {},
	clipBoard (),
	cursor (0), wheelScrolled (false), padPressed (false), deleteMode (false),
	actSlot (-1), dragOrigin {-1, -1},

	mContainer (0, 0, 1240, 648, "main"),
	messageLabel (400, 45, 600, 20, "ctlabel", ""),
	helpButton (1168, 18, 24, 24, "widget", BOOPS_LABEL_HELP),
	ytButton (1198, 18, 24, 24, "widget", BOOPS_LABEL_TUTORIAL),

	settingsContainer (10, 90, 1220, 40, "widget"),
	playButton (8, 8, 24, 24, "widget", BOOPS_LABEL_PLAY),
	bypassButton (38, 8, 24, 24, "widget", BOOPS_LABEL_BYPASS),
	stopButton (68, 8, 24, 24, "widget", BOOPS_LABEL_STOP),
	sourceListBox (120, 10, 80, 20, 80, 60, "menu", BItems::ItemList ({{0, BOOPS_LABEL_STREAM}, {1, BOOPS_LABEL_SAMPLE}})),
	loadButton (220, 10, 20, 20, "menu/button"),
	sampleLabel (240, 0, 140, 8, "smlabel", BOOPS_LABEL_SAMPLE),
	sampleNameLabel (240, 10, 140, 20, "boxlabel", ""),
	fileChooser (nullptr),
	sampleAmpLabel (398, 0, 24, 8, "smlabel", BOOPS_LABEL_AMP),
	sampleAmpDial (398, 8, 24, 24, "dial", 1.0, 0.0, 1.0, 0.0),
	playModeListBox (440, 10, 120, 20, 120, 80, "menu", BItems::ItemList ({{0, "Autoplay"}, {2, "Host-controlled"} , {1, "MIDI-controlled"}})),
	onMidiListBox (580, 10, 120, 20, 120, 80, "menu", BItems::ItemList ({{0, "Restart"}, {2, "Restart & sync"}, {1, "Continue"}})),
	transportGateButton (720, 10, 60, 20, "widget", 48, 59),
	autoplayBpmLabel (580, 0, 80, 8, "smlabel", "bpm"),
	autoplayBpmSlider (580, 10, 80, 20, "slider", 120, 1, 300, 0, "%1.0f"),
	autoplayBpbLabel (680, 0, 80, 8, "smlabel", "bpBar"),
	autoplayBpbSlider (680, 10, 80, 20, "slider", 4, 1, 16, 0, "%1.0f", [] (double x) {return floor (x);}),
	autoplayPositionLabel (780, 0, 110, 8, "smlabel", "Adjust position"),
	autoplayPositionSlider (780, 14, 110, 12, "slider", 0.0, -0.5, 0.5, 0.0),
	sequenceSizeSelect (910, 12, 80, 16, "select", 1, 1, 16, 0.01),
	sequenceBaseListBox (1010, 10, 90, 20, 0, 20, 90, 80, "menu", BItems::ItemList ({{0, "Seconds"}, {1, "Beats"}, {2, "Bars"}}), 1),
	stepsListBox (1120, 10, 90, 20, 0, 20, 90, 240, "menu",
		     BItems::ItemList ({{2, "2 Steps"}, {3, "3 Steps"}, {4, "4 Steps"}, {6, "6 Steps"}, {8, "8 Steps"}, {9, "9 Steps"},
		     			{12, "12 Steps"}, {16, "16 Steps"}, {18, "18 Steps"}, {24, "24 Steps"}, {32, "32 Steps"}}), 16),

	transportGateContainer (420, 130, 600, 110, "screen"),
	transportGateLabel (210, 10, 180, 20, "ctlabel", "Select keys"),
	transportGatePiano (10, 40, 580, 30, "widget", 0, NR_PIANO_KEYS - 1),
	transportGateOkButton (320, 80, 40, 20, "menu/button", "OK"),
	transportGateCancelButton (240, 80, 60, 20, "menu/button", "Cancel"),
	transportGateKeys (NR_PIANO_KEYS, false),

	slotsContainer (20, 170, 260, 288, "widget"),
	insLine (nullptr),

	pageWidget (288, 136, 824, 30, "widget", 0.0),
	pageBackSymbol (0, 0, 10, 30, "tab", LEFTSYMBOL),
	pageForwardSymbol (800, 0, 10, 30, "tab", RIGHTSYMBOL), // TODO xpos

	midiBox (390, 170, 510, 120, "screen", 0),
	midiText (20, 10, 450, 20, "tlabel", BOOPS_LABEL_MIDI_PAGE " #1"),
	midiStatusLabel (10, 30, 130, 20, "ylabel", BOOPS_LABEL_MIDI_STATUS),
	midiStatusListBox
	(
		10, 50, 130, 20, 0, 20, 130, 100, "menu",
		BItems::ItemList ({{0, BOOPS_LABEL_NONE}, {9, BOOPS_LABEL_NOTE_ON}, {8, BOOPS_LABEL_NOTE_OFF}, {11, BOOPS_LABEL_CC}}),
		0
	),
	midiChannelLabel (150, 30, 50, 20, "ylabel", BOOPS_LABEL_CHANNEL),
	midiChannelListBox
	(
		150, 50, 50, 20, 0, 20, 50, 360, "menu",
		BItems::ItemList
		({
			{0, BOOPS_LABEL_ALL}, {1, "1"}, {2, "2"}, {3, "3"},
			{4, "4"}, {5, "5"}, {6, "6"}, {7, "7"},
			{8, "8"}, {9, "9"}, {10, "10"}, {11, "11"},
			{12, "12"}, {13, "13"}, {14, "14"}, {15, "15"}, {16, "16"}
		}),
		0
	),
	midiNoteLabel (210, 30, 160, 20, "ylabel", BOOPS_LABEL_NOTE),
	midiNoteListBox (210, 50, 160, 20, 0, 20, 160, 360, "menu", BItems::ItemList ({NOTELIST}), 128),
	midiValueLabel (380, 30, 50, 20, "ylabel", BOOPS_LABEL_VALUE),
	midiValueListBox (380, 50, 50, 20, 0, 20, 50, 360, "menu", BItems::ItemList ({VALLIST}), 128),
	midiLearnButton (440, 50, 60, 20, "menu/button", BOOPS_LABEL_LEARN),
	midiCancelButton (170, 90, 60, 20, "menu/button", BOOPS_LABEL_CANCEL),
	midiOkButton (280, 90, 60, 20, "menu/button", BOOPS_LABEL_OK),

	monitor (290, 170, 820, 288, "monitor"),
	padSurface (290, 170, 820, 288, "padsurface"),
	editContainer (578, 466, 284, 24, "widget"),

	gettingstartedContainer (20, 478, 1200, 150, "widget", pluginPath + "inc/None_bg.png"),
	gettingstartedText
	(
		20, 30, 960, 110, "lflabel",
		"Getting started\n"
		" \n"
		"1) Add an effect by clicking on the [+] symbol below \"Fx\".\n"
		"2) Click on the menu symbol left to the effect name to change the effect\n"
		"3) Set a pattern right to the effect name to define the timepoint(s) to apply the effect on the incoming audio signal.\n"
		"4) Continue with point 1 to add another effects. Change the order of the effects by clicking on the respective symbol."
	),

	padParamContainer (1120, 170, 100, 288, "widget"),
	padGateLabel (00, 90, 100, 20, "ctlabel", "Probability"),
	padGateDial (20, 30, 60, 60, "dial", 1.0, 0.0, 1.0, 0.0, "%1.2f"),
	padMixLabel (20, 180, 60, 20, "ctlabel", "Mix"),
	padMixDial (20, 120, 60, 60, "dial", 1.0, 0.0, 1.0, 0.0, "%1.2f")
{
	// Init slots
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		slots[i].container = BWidgets::ValueWidget (0, i * 24, 260, 24, "padSurface", FX_NONE);
		slots[i].addPad = PadButton (0, 0, 20, 24, "pad0", ADDSYMBOL);
		slots[i].delPad = PadButton (20, 0, 20, 24, "pad0", CLOSESYMBOL);
		slots[i].upPad = PadButton (40, 0, 20, 24, "pad0", UPSYMBOL);
		slots[i].downPad = PadButton (60, 0, 20, 24, "pad0", DOWNSYMBOL);
		slots[i].effectPad = IconPadButton (80, 0, 160, 24, "pad0", pluginPath + "inc/Menu.png", "");
		slots[i].effectsListbox = BWidgets::ListBox (80, 24, 160, 160, "menu/listbox", BItems::ItemList (BOOPSFXNAMES));
		slots[i].playPad = PadToggleButton (240, 0, 20, 24, "pad0", PLAYSYMBOL);
	}

	// Init tabs
	for (int i = 0; i < NR_PAGES; ++i)
	{
		tabs[i].container = BWidgets::Widget (i * 80, 0, 78, 30, "tab");
		tabs[i].icon = BWidgets::ImageIcon (0, 8, 40, 20, "widget", pluginPath + "inc/page" + std::to_string (i + 1) + ".png");
		tabs[i].playSymbol = SymbolWidget (40, 12, 20, 16, "symbol", PLAYSYMBOL);
		tabs[i].midiSymbol = SymbolWidget (60, 10, 20, 20, "symbol", MIDISYMBOL);
		for (int j = 0; j < NR_MIDI_CTRLS; ++j) tabs[i].symbols[j] = SymbolWidget (68 - j * 10, 2, 8, 8, "symbol", SymbolIndex(j));
		for (BWidgets::ValueWidget& m : tabs[i].midiWidgets) m = BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0);
	}

	// Init editButtons
	for (int i = 0; i < EDIT_RESET; ++i) edit1Buttons[i] = HaloToggleButton (i * 30, 0, 24, 24, "widget", editLabels[i]);
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i) edit2Buttons[i] = HaloButton (170 + i * 30, 0, 24, 24, "widget", editLabels[i + EDIT_RESET]);

	// Init slot params
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		slotParams[i].container = BWidgets::ImageIcon (20, 478, 1200, 150, "widget", "");
		slotParams[i].nrIcon = BWidgets::ImageIcon (20, 8, 40, 20, "widget", "");
		slotParams[i].nameIcon = BWidgets::ImageIcon (60, 8, 160, 20, "widget", "");
		slotParams[i].attackLabel = BWidgets::Label (190, 30, 20, 20, "ctlabel", "A");
		slotParams[i].decayLabel = BWidgets::Label (190, 60, 20, 20, "ctlabel", "D");
		slotParams[i].sustainLabel = BWidgets::Label (190, 90, 20, 20, "ctlabel", "S");
		slotParams[i].releaseLabel = BWidgets::Label (190, 120, 20, 20, "ctlabel", "R");
		slotParams[i].attackSlider = HSlider (210, 30, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].decaySlider = HSlider (210, 60, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].sustainSlider = HSlider (210, 90, 60, 20, "slider", 1.0, 0.0, 1.0, 0.0, "%1.2f");
		slotParams[i].releaseSlider = HSlider (210, 120, 60, 20, "slider", 0.1, 0.0, 1.0, 0.0, "%1.2f", [] (double x) {return x;}, [] (double x) {return x;}, [] (double x) {return pow (x, 1.0 / 2.0);}, [] (double x) {return pow (x, 2.0);});
		slotParams[i].adsrDisplay = CurveChart (10, 30, 170, 110, "slider");
		slotParams[i].panLabel = BWidgets::Label (280, 110, 60, 20, "ctlabel", "Pan");
		slotParams[i].panDial = Dial (280, 40, 60, 60, "dial", 0.0, -1.0, 1.0, 0.0, "%1.2f");
		slotParams[i].mixLabel = BWidgets::Label (360, 110, 60, 20, "ctlabel", "Mix");
		slotParams[i].mixDial = Dial (360, 40, 60, 60, "dial", 0.0, 0.0, 1.0, 0.0, "%1.2f");
		for (int j = 0; j < NR_OPTPARAMS; ++j) slotParams[i].options[j] = Dial (0, 0, 0, 0, "widget", 0.0, 0.0, 1.0, 0.0);
		slotParams[i].shape = Shape<SHAPE_MAXNODES>();
		slotParams[i].shape.setDefaultShape();
		slotParams[i].optionWidget = nullptr;
	}

	// Link controllerWidgets
	controllerWidgets[PLAY] = (BWidgets::ValueWidget*) &playButton;
	controllerWidgets[SOURCE] = (BWidgets::ValueWidget*) &sourceListBox;
	controllerWidgets[PLAY_MODE] = (BWidgets::ValueWidget*) &playModeListBox;
	controllerWidgets[ON_MIDI] = (BWidgets::ValueWidget*) &onMidiListBox;
	controllerWidgets[AUTOPLAY_BPM] = (BWidgets::ValueWidget*) &autoplayBpmSlider;
	controllerWidgets[AUTOPLAY_BPB] = (BWidgets::ValueWidget*) &autoplayBpbSlider;
	controllerWidgets[AUTOPLAY_POSITION] = (BWidgets::ValueWidget*) &autoplayPositionSlider;
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
	helpButton.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, helpButtonClickedCallback);
	ytButton.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, ytButtonClickedCallback);
	bypassButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, playStopBypassChangedCallback);
	stopButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, playStopBypassChangedCallback);
	sampleAmpDial.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
	transportGateButton.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, transportGateButtonClickedCallback);
	transportGateOkButton.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, transportGateButtonClickedCallback);
	transportGateCancelButton.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, transportGateButtonClickedCallback);
	loadButton.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, loadButtonClickedCallback);
	sampleNameLabel.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, loadButtonClickedCallback);

	for (Slot& s : slots)
	{
		s.addPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, addClickedCallback);
		s.delPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, delClickedCallback);
		s.upPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, upClickedCallback);
		s.downPad.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, downClickedCallback);
		s.effectPad.button.setCallbackFunction (BEvents::BUTTON_CLICK_EVENT, menuClickedCallback);
		s.effectPad.setDraggable (true);
		s.effectPad.setCallbackFunction (BEvents::POINTER_DRAG_EVENT, effectDraggedCallback);
		s.effectPad.setCallbackFunction (BEvents::BUTTON_RELEASE_EVENT, effectReleasedCallback);
		s.effectsListbox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, effectChangedCallback);
	}

	pageWidget.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
	pageBackSymbol.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, pageScrollClickedCallback);
	pageForwardSymbol.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, pageScrollClickedCallback);

	for (Tab& t : tabs)
	{
		t.container.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, pageClickedCallback);
		t.playSymbol.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, pagePlayClickedCallback);
		t.midiSymbol.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, midiSymbolClickedCallback);
		for (SymbolWidget& s : t.symbols) s.setCallbackFunction(BEvents::BUTTON_PRESS_EVENT, pageSymbolClickedCallback);
		for (BWidgets::ValueWidget& m : t.midiWidgets) m.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
	}

	midiLearnButton.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, midiButtonClickedCallback);
	midiCancelButton.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, midiButtonClickedCallback);
	midiOkButton.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, midiButtonClickedCallback);
	midiStatusListBox.setCallbackFunction(BEvents::VALUE_CHANGED_EVENT, midiStatusChangedCallback);

	for (int i = 0; i < EDIT_RESET; ++i) edit1Buttons[i].setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, edit1ChangedCallback);
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i) edit2Buttons[i].setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, edit2ChangedCallback);

	// Configure widgets
	loadButton.hide();
	sampleLabel.hide();
	sampleNameLabel.hide();
	sampleAmpLabel.hide();
	sampleAmpDial.hide();
	onMidiListBox.hide();
	transportGateButton.hide ();
	transportGatePiano.setKeysToggleable (true);
	transportGateContainer.hide();

	for (Pattern& p : patterns) p.clear ();
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

	midiBox.hide();
	pageBackSymbol.setFocusable (false);
	pageForwardSymbol.setFocusable (false);
	pageBackSymbol.hide();
	pageForwardSymbol.hide();
	for (Tab& t : tabs)
	{
		t.playSymbol.setState (BColors::INACTIVE);
		t.midiSymbol.setState (BColors::INACTIVE);
		for (int j = 0; j < 4; ++j) t.symbols[j].setState (BColors::ACTIVE);
		t.container.hide();
		t.icon.setClickable (false);
	}
	tabs[0].container.rename ("activetab");
	tabs[0].playSymbol.setState (BColors::ACTIVE);
	tabs[0].symbols[CLOSESYMBOL].hide(); // -
	tabs[0].symbols[LEFTSYMBOL].hide(); // <
	tabs[0].symbols[RIGHTSYMBOL].hide(); // >
	tabs[0].container.show();

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
	settingsContainer.add (sourceListBox);
	settingsContainer.add (loadButton);
	settingsContainer.add (sampleLabel);
	settingsContainer.add (sampleNameLabel);
	settingsContainer.add (sampleAmpLabel);
	settingsContainer.add (sampleAmpDial);
	settingsContainer.add (playModeListBox);
	settingsContainer.add (onMidiListBox);
	settingsContainer.add (transportGateButton);
	settingsContainer.add (autoplayBpmLabel);
	settingsContainer.add (autoplayBpmSlider);
	settingsContainer.add (autoplayBpbLabel);
	settingsContainer.add (autoplayBpbSlider);
	settingsContainer.add (autoplayPositionLabel);
	settingsContainer.add (autoplayPositionSlider);
	settingsContainer.add (sequenceSizeSelect);
	settingsContainer.add (sequenceBaseListBox);
	settingsContainer.add (stepsListBox);

	transportGateContainer.add (transportGateLabel);
	transportGateContainer.add (transportGatePiano);
	transportGateContainer.add (transportGateOkButton);
	transportGateContainer.add (transportGateCancelButton);

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

	mContainer.add (pageWidget);
	pageWidget.add (pageBackSymbol);
	pageWidget.add (pageForwardSymbol);
	for (int i = NR_PAGES - 1; i >= 0; --i)
	{
		Tab& t = tabs[i];
		t.container.add (t.icon);
		for (SymbolWidget& s : t.symbols) t.container.add (s);
		for (BWidgets::ValueWidget& m : t.midiWidgets) t.container.add (m);
		t.container.add (t.playSymbol);
		t.container.add (t.midiSymbol);
		pageWidget.add (t.container);
	}

	for (HaloToggleButton& e1 : edit1Buttons) editContainer.add (e1);
	for (HaloButton& e2 : edit2Buttons) editContainer.add (e2);

	gettingstartedContainer.add (gettingstartedText);

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
	mContainer.add (gettingstartedContainer);
	mContainer.add (editContainer);
	for (Slot& s : slots) slotsContainer.add (s.container);
	mContainer.add (slotsContainer);
	mContainer.add (padSurface);
	mContainer.add (monitor);
	mContainer.add (transportGateContainer);
	mContainer.add (settingsContainer);
	mContainer.add (helpButton);
	mContainer.add (ytButton);
	mContainer.add (messageLabel);

	mContainer.add (midiBox);
	midiBox.add (midiText);
	midiBox.add (midiStatusLabel);
	midiBox.add (midiStatusListBox);
	midiBox.add (midiChannelLabel);
	midiBox.add (midiChannelListBox);
	midiBox.add (midiNoteLabel);
	midiBox.add (midiNoteListBox);
	midiBox.add (midiValueLabel);
	midiBox.add (midiValueListBox);
	midiBox.add (midiLearnButton);
	midiBox.add (midiCancelButton);
	midiBox.add (midiOkButton);

	drawPad();
	add (mContainer);

	getKeyGrabStack()->add (this);

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

BOopsGUI::~BOopsGUI ()
{
	for (SlotParam& s : slotParams)
	{
		if (s.optionWidget) delete (s.optionWidget);
	}

	if (fileChooser) delete fileChooser;
	if (insLine) delete insLine;

	sendUiOff ();
}

void BOopsGUI::Pattern::clear ()
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

Pad BOopsGUI::Pattern::getPad (const size_t row, const size_t step) const
{
	return pads[LIMIT (row, 0, NR_SLOTS)][LIMIT (step, 0, NR_STEPS)];
}
void BOopsGUI::Pattern::setPad (const size_t row, const size_t step, const Pad& pad)
{
	size_t r = LIMIT (row, 0, NR_SLOTS);
	size_t s = LIMIT (step, 0, NR_STEPS);
	changes.oldMessage.push_back (PadMessage (r, s, pads[r][s]));
	changes.newMessage.push_back (PadMessage (r, s, pad));
	pads[r][s] = pad;
}

std::vector<PadMessage> BOopsGUI::Pattern::undo ()
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

std::vector<PadMessage> BOopsGUI::Pattern::redo ()
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

void BOopsGUI::Pattern::store ()
{
	if (changes.newMessage.empty ()) return;

	journal.push (changes.oldMessage, changes.newMessage);
	changes.oldMessage.clear ();
	changes.newMessage.clear ();
}

void BOopsGUI::port_event(uint32_t port, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	if (!buffer) return;

	// Notify port
	if ((format == urids.atom_eventTransfer) && (port == NOTIFY))
	{
		const LV2_Atom* atom = (const LV2_Atom*) buffer;
		if ((atom->type == urids.atom_Blank) || (atom->type == urids.atom_Object))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*) atom;

			// transportGateKey notification
			if (obj->body.otype == urids.bOops_transportGateKeyEvent)
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
						transportGateKeys = std::vector<bool> (NR_PIANO_KEYS, false);
						for (int i = 0; i < keysize; ++i)
						{
							int keyNr = keys[i];
							if ((keyNr >=0) && (keyNr < NR_PIANO_KEYS)) transportGateKeys[keyNr] = true;
						}
						transportGatePiano.pressKeys (transportGateKeys);
					}
				}
			}

			// Slot pattern notification
			else if (obj->body.otype == urids.bOops_slotEvent)
			{
				LV2_Atom *oPg = NULL, *oSl = NULL, *oPd = NULL;
				int page = 0;
				int slot = -1;
				lv2_atom_object_get(obj,
						    urids.bOops_pageID, &oPg,
						    urids.bOops_slot, &oSl,
						    urids.bOops_pads, &oPd,
						    NULL);

				if (oPg && (oPg->type == urids.atom_Int)) page = LIMIT (((LV2_Atom_Int*)oPg)->body, 0, NR_PAGES -1);

				if (oSl && (oSl->type == urids.atom_Int)) slot = ((LV2_Atom_Int*)oSl)->body;

				if (oPd && (oPd->type == urids.atom_Vector) && (slot >= 0) && (slot < NR_SLOTS))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oPd;
					if (vec->body.child_type == urids.atom_Float)
					{
						if (wheelScrolled)
						{
							patterns[page].store ();
							wheelScrolled = false;
						}

						uint32_t size = (uint32_t) ((oPd->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (Pad));
						Pad* p = (Pad*)(&vec->body + 1);
						for (unsigned int i = 0; (i < size) && (i < NR_STEPS); ++i)
						{
							patterns[page].setPad (slot, i, p[i]);
						}
						patterns[page].store ();
						if (page == pageAct) drawPad (slot);
					}
				}
			}

			// Slot shape notification
			else if (obj->body.otype == urids.bOops_shapeEvent)
			{
				LV2_Atom *oSl = NULL, *oSh = NULL;
				int slot = -1;
				lv2_atom_object_get(obj,
						    urids.bOops_slot, &oSl,
						    urids.bOops_shapeData, &oSh,
						    NULL);

				if (oSl && (oSl->type == urids.atom_Int))
				{
					slot = ((LV2_Atom_Int*)oSl)->body;
				}

				if (oSh && (oSh->type == urids.atom_Vector) && (slot >= 0) && (slot < NR_SLOTS))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oSh;
					if (vec->body.child_type == urids.atom_Float)
					{
						slotParams[slot].shape.clearShape ();
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
							slotParams[slot].shape.appendNode (node);
						}

						// Forward to optionWidget
						if (slotParams[slot].optionWidget) slotParams[slot].optionWidget->setShape (slotParams[slot].shape);
					}
				}
			}

			// Message notification
			else if (obj->body.otype == urids.bOops_messageEvent)
			{
				const LV2_Atom* data = NULL;
				lv2_atom_object_get(obj, urids.bOops_message, &data, 0);
				if (data && (data->type == urids.atom_Int))
				{
					const int messageNr = ((LV2_Atom_Int*)data)->body;
					std::string msg = ((messageNr >= NO_MSG) && (messageNr < MAX_MSG) ? messageStrings[messageNr] : "");
					messageLabel.setText (msg);
				}
			}

			// Status notifications
			else if (obj->body.otype == urids.bOops_statusEvent)
			{
				LV2_Atom *oBpb = NULL, *oBu = NULL, *oBpm = NULL, *oPos = NULL, *oPg = NULL, *oMax = NULL, *oMid = NULL;
				lv2_atom_object_get
				(
					obj,
					urids.time_beatsPerBar, &oBpb,
					urids.time_beatUnit, &oBu,
					urids.time_beatsPerMinute, &oBpm,
					urids.bOops_position, &oPos,
					urids.bOops_pageID, &oPg,
					urids.bOops_pageMax, &oMax,
					urids.bOops_midiLearned, &oMid,
					NULL
				);

				if (oBpb && (oBpb->type == urids.atom_Float))
				{
					// TODO bpb = ((LV2_Atom_Float*)oBpb)->body;
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

				if (oMax && (oMax->type == urids.atom_Int))
				{
					int newMax = LIMIT (((LV2_Atom_Int*)oMax)->body, 0, NR_PAGES - 1);

					while (newMax > pageMax) pushPage();
					while (newMax < pageMax) popPage();
				}

				if (oPg && (oPg->type == urids.atom_Int)) pageWidget.setValue (LIMIT (((LV2_Atom_Int*)oPg)->body, 0, NR_PAGES - 1));

				if (oMid && (oMid->type == urids.atom_Int))
				{
					uint32_t newMid = (((LV2_Atom_Int*)oMid)->body);
					uint8_t st = newMid >> 24;
					uint8_t ch = (newMid >> 16) & 0xFF;
					uint8_t nt = (newMid >> 8) & 0xFF;
					uint8_t vl = newMid & 0xFF;
					midiStatusListBox.setValue ((st == 8) || (st == 9) || (st == 11) ? st : 0);
					midiChannelListBox.setValue (LIMIT (ch, 0, 15) + 1);
					midiNoteListBox.setValue (LIMIT (nt, 0, 127));
					midiValueListBox.setValue (LIMIT (vl, 0, 127));
					midiLearnButton.setValue (0.0);
				}
			}

			// Process page properties
			else if (obj->body.otype == urids.bOops_pagePropertiesEvent)
			{
				LV2_Atom *oId = NULL, *oStatus = NULL, *oChannel = NULL, *oMsg = NULL, *oValue = NULL;
				int id = -1;
				lv2_atom_object_get (obj,
						     urids.bOops_pageID, &oId,
		     				     urids.bOops_pageStatus, &oStatus,
		     				     urids.bOops_pageChannel, &oChannel,
		     				     urids.bOops_pageMessage, &oMsg,
		     				     urids.bOops_pageValue, &oValue,
						     NULL);

				if (oId && (oId->type == urids.atom_Int) && (((LV2_Atom_Int*)oId)->body >= 0) && (((LV2_Atom_Int*)oId)->body < NR_PAGES))
     				{
     					id = ((LV2_Atom_Int*)oId)->body;

					if (oStatus && (oStatus->type == urids.atom_Int))
					{
						tabs[id].midiWidgets[PAGE_CONTROLS_STATUS].setValue (LIMIT (((LV2_Atom_Int*)oStatus)->body, 0, 15));
					}

     					if (oChannel && (oChannel->type == urids.atom_Int))
					{
						tabs[id].midiWidgets[PAGE_CONTROLS_CHANNEL].setValue (LIMIT (((LV2_Atom_Int*)oChannel)->body, 0, 16));
					}

     					if (oMsg && (oMsg->type == urids.atom_Int))
					{
						tabs[id].midiWidgets[PAGE_CONTROLS_MESSAGE].setValue (LIMIT (((LV2_Atom_Int*)oMsg)->body, 0, 128));
					}

     					if (oValue && (oValue->type == urids.atom_Int))
					{
						tabs[id].midiWidgets[PAGE_CONTROLS_VALUE].setValue (LIMIT (((LV2_Atom_Int*)oValue)->body, 0, 128));
					}
     				}
			}

			// Path notification
			else if (obj->body.otype == urids.bOops_samplePathEvent)
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

				if (oPath && (oPath->type == urids.atom_Path))
				{
					const BUtilities::Path p = BUtilities::Path ((const char*)LV2_ATOM_BODY_CONST(oPath));
					samplePath = p.dir();
					sampleNameLabel.setText (p.filename());
				}

				if (oStart && (oStart->type == urids.atom_Long)) sampleStart = ((LV2_Atom_Long*)oStart)->body;
				if (oEnd && (oEnd->type == urids.atom_Long)) sampleEnd = ((LV2_Atom_Long*)oEnd)->body;
				if (oAmp && (oAmp->type == urids.atom_Float)) sampleAmpDial.setValue (((LV2_Atom_Float*)oAmp)->body);
				if (oLoop && (oLoop->type == urids.atom_Bool)) sampleLoop = bool (((LV2_Atom_Bool*)oLoop)->body);
			}

			// Monitor notification
			else if (obj->body.otype == urids.bOops_waveformEvent)
			{
				int start = -1;

				const LV2_Atom *oStart = NULL, *oData = NULL;
				lv2_atom_object_get (obj,
						     urids.bOops_waveformStart, &oStart,
						     urids.bOops_waveformData, &oData,
						     NULL);
				if (oStart && (oStart->type == urids.atom_Int)) start = ((LV2_Atom_Int*)oStart)->body;

				if (oData && (oData->type == urids.atom_Vector))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) oData;
					if (vec->body.child_type == urids.atom_Float)
					{
						uint32_t size = (uint32_t) ((oData->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (float));
						float* data = (float*) (&vec->body + 1);
						if ((start >= 0) && (size > 0))
						{
							monitor.addData (start, size, data);
							monitor.redrawRange (start, size);
						}
					}
				}
			}
		}
	}

	// Scan remaining ports
	else if (format == 0)
	{
		if ((port >= CONTROLLERS) && (port < CONTROLLERS + NR_CONTROLLERS))
		{
			float* pval = (float*) buffer;
			controllerWidgets[port - CONTROLLERS]->setValue (*pval);
		}

		else fprintf(stderr, "BOops.lv2#GUI: Invalid control port %i in BOopsGUI::port_event()\n", port);
	}

}

void BOopsGUI::resize ()
{
	hide ();
	//Scale fonts
	ctLabelFont.setFontSize (12 * sz);
	tLabelFont.setFontSize (12 * sz);
	tgLabelFont.setFontSize (12 * sz);
	lfLabelFont.setFontSize (12 * sz);
	boldLfLabelFont.setFontSize (12 * sz);
	mdLabelFont.setFontSize (10 * sz);
	smLabelFont.setFontSize (8 * sz);

	//Background
	cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1240 * sz, 648 * sz);
	cairo_t* cr = cairo_create (surface);
	cairo_scale (cr, sz, sz);
	cairo_set_source_surface(cr, bgImageSurface, 0, 0);
	cairo_paint(cr);
	widgetBg.loadFillFromCairoSurface(surface);
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	//Scale widgets
	RESIZE (mContainer, 0, 0, 1240, 648, sz);
	RESIZE (messageLabel, 400, 45, 600, 20, sz);
	RESIZE (helpButton, 1168, 18, 24, 24, sz);
	RESIZE (ytButton, 1198, 18, 24, 24, sz);

	RESIZE (settingsContainer, 10, 90, 1220, 40, sz);
	RESIZE (playButton, 8, 8, 24, 24, sz);
	RESIZE (bypassButton, 38, 8, 24, 24, sz);
	RESIZE (stopButton, 68, 8, 24, 24, sz);
	RESIZE (sourceListBox, 120, 10, 80, 20, sz);
	sourceListBox.resizeListBox (BUtilities::Point (80 * sz, 60 * sz));
	sourceListBox.resizeListBoxItems (BUtilities::Point (80 * sz, 20 * sz));
	RESIZE (loadButton, 220, 10, 20, 20, sz);
	RESIZE (sampleAmpLabel, 398, 0, 24, 8, sz);
	RESIZE (sampleLabel, 240, 0, 140, 8, sz);
	RESIZE (sampleNameLabel, 240, 10, 140, 20, sz);
	if (fileChooser) RESIZE ((*fileChooser), 200, 140, 640, 400, sz);
	RESIZE (sampleAmpDial, 398, 8, 24, 24, sz);
	RESIZE (playModeListBox, 440, 10, 120, 20, sz);
	playModeListBox.resizeListBox(BUtilities::Point (120 * sz, 80 * sz));
	playModeListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	playModeListBox.resizeListBoxItems(BUtilities::Point (120 * sz, 20 * sz));
	RESIZE (onMidiListBox, 580, 10, 120, 20, sz);
	onMidiListBox.resizeListBox(BUtilities::Point (120 * sz, 80 * sz));
	onMidiListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	onMidiListBox.resizeListBoxItems(BUtilities::Point (120 * sz, 20 * sz));
	RESIZE (transportGateButton, 720, 10, 60, 20, sz);
	RESIZE (autoplayBpmLabel, 580, 0, 80, 8, sz);
	RESIZE (autoplayBpmSlider, 580, 10, 80, 20, sz);
	RESIZE (autoplayBpbLabel, 680, 0, 80, 8, sz);
	RESIZE (autoplayBpbSlider, 680, 10, 80, 20, sz);
	RESIZE (autoplayPositionLabel, 780, 0, 110, 8, sz);
	RESIZE (autoplayPositionSlider, 780, 14, 110, 12, sz);
	RESIZE (sequenceSizeSelect, 910, 12, 80, 16, sz);
	RESIZE (sequenceBaseListBox, 1010, 10, 90, 20, sz);
	sequenceBaseListBox.resizeListBox(BUtilities::Point (90 * sz, 80 * sz));
	sequenceBaseListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	sequenceBaseListBox.resizeListBoxItems(BUtilities::Point (90 * sz, 20 * sz));
	RESIZE (stepsListBox, 1120, 10, 90, 20, sz);
	stepsListBox.resizeListBox(BUtilities::Point (90 * sz, 240 * sz));
	stepsListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	stepsListBox.resizeListBoxItems(BUtilities::Point (90 * sz, 20 * sz));

	RESIZE (transportGateContainer, 420, 130, 600, 110, sz);
	RESIZE (transportGateLabel, 210, 10, 180, 20, sz);
	RESIZE (transportGatePiano, 10, 40, 580, 30, sz);
	RESIZE (transportGateOkButton, 320, 80, 40, 20, sz);
	RESIZE (transportGateCancelButton, 240, 80, 60, 20, sz);

	RESIZE (pageWidget, 288, 136, 824, 30, sz);
	updatePageContainer();
	for (Tab& t : tabs)
	{
		RESIZE (t.icon, 0, 8, 40, 20, sz);
		RESIZE (t.playSymbol, 40, 12, 20, 16, sz);
		RESIZE (t.midiSymbol, 60, 10, 20, 20, sz);
		for (int j = 0; j < 4; ++j) RESIZE (t.symbols[j], 68 - j * 10, 2, 8, 8, sz);
	}



	RESIZE (midiBox, 390, 170, 510, 120, sz);
	RESIZE (midiText, 20, 10, 450, 20, sz);
	RESIZE (midiStatusLabel, 10, 30, 130, 20, sz);
	RESIZE (midiStatusListBox, 10, 50, 130, 20, sz);
	midiStatusListBox.resizeListBox(BUtilities::Point (130 * sz, 100 * sz));
	midiStatusListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	midiStatusListBox.resizeListBoxItems(BUtilities::Point (130 * sz, 20 * sz));
	RESIZE (midiChannelLabel, 150, 30, 50, 20, sz);
	RESIZE (midiChannelListBox, 150, 50, 50, 20, sz);
	midiChannelListBox.resizeListBox(BUtilities::Point (50 * sz, 360 * sz));
	midiChannelListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	midiChannelListBox.resizeListBoxItems(BUtilities::Point (50 * sz, 20 * sz));
	RESIZE (midiNoteLabel, 210, 30, 160, 20, sz);
	RESIZE (midiNoteListBox, 210, 50, 160, 20, sz);
	midiNoteListBox.resizeListBox(BUtilities::Point (160 * sz, 360 * sz));
	midiNoteListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	midiNoteListBox.resizeListBoxItems(BUtilities::Point (160 * sz, 20 * sz));
	RESIZE (midiValueLabel, 380, 30, 50, 20, sz);
	RESIZE (midiValueListBox, 380, 50, 50, 20, sz);
	midiValueListBox.resizeListBox(BUtilities::Point (50 * sz, 360 * sz));
	midiValueListBox.moveListBox(BUtilities::Point (0, 20 * sz));
	midiValueListBox.resizeListBoxItems(BUtilities::Point (50 * sz, 20 * sz));
	RESIZE (midiLearnButton, 440, 50, 60, 20, sz);
	RESIZE (midiCancelButton, 170, 90, 60, 20, sz);
	RESIZE (midiOkButton, 280, 90, 60, 20, sz);

	RESIZE (monitor, 290, 170, 820, 288, sz);
	RESIZE (padSurface, 290, 170, 820, 288, sz);
	RESIZE (editContainer, 578, 466, 284, 24, sz);

	RESIZE (gettingstartedContainer, 20, 478, 1200, 150, sz);
	RESIZE (gettingstartedText, 20, 30, 960, 110, sz);

	RESIZE (padParamContainer, 1120, 170, 100, 288, sz);
	RESIZE (padGateLabel, 0, 90, 100, 20, sz);
	RESIZE (padGateDial, 20, 30, 60, 60, sz);
	RESIZE (padMixLabel, 20, 180, 60, 20, sz);
	RESIZE (padMixDial, 20, 120, 60, 60, sz);

	RESIZE (slotsContainer, 20, 170, 260, 288, sz);

	for (int i = 0; i < NR_SLOTS; ++i)
	{
		RESIZE (slots[i].container, 0, i * 24, 260, 24, sz);
		RESIZE (slots[i].addPad, 0, 0, 20, 24, sz);
		RESIZE (slots[i].delPad, 20, 0, 20, 24, sz);
		RESIZE (slots[i].upPad, 40, 0, 20, 24, sz);
		RESIZE (slots[i].downPad , 60, 0, 20, 24, sz);
		RESIZE (slots[i].effectPad, 80, 0, 160, 24, sz);
		RESIZE (slots[i].effectsListbox, 80, 24, 160, 160, sz);
		RESIZE (slots[i].playPad, 240, 0, 20, 24, sz);
	}

	for (int i = 0; i < EDIT_RESET; ++i) RESIZE (edit1Buttons[i], i * 30, 0, 24, 24, sz);
	for (int i = 0; i < MAXEDIT - EDIT_RESET; ++i) RESIZE (edit2Buttons[i], 170 + i * 30, 0, 24, 24, sz);

	for (int i = 0; i < NR_SLOTS; ++i)
	{
		RESIZE (slotParams[i].container, 20, 478, 1200, 150, sz);
		RESIZE (slotParams[i].nrIcon, 20, 8, 40, 20, sz);
		RESIZE (slotParams[i].nameIcon, 60, 8, 160, 20, sz);
		RESIZE (slotParams[i].attackLabel, 190, 30, 20, 20, sz);
		RESIZE (slotParams[i].decayLabel, 190, 60, 20, 20, sz);
		RESIZE (slotParams[i].sustainLabel, 190, 90, 20, 20, sz);
		RESIZE (slotParams[i].releaseLabel, 190, 120, 20, 20, sz);
		RESIZE (slotParams[i].attackSlider, 210, 30, 60, 20, sz);
		RESIZE (slotParams[i].decaySlider, 210, 60, 60, 20, sz);
		RESIZE (slotParams[i].sustainSlider, 210, 90, 60, 20, sz);
		RESIZE (slotParams[i].releaseSlider, 210, 120, 60, 20, sz);
		RESIZE (slotParams[i].adsrDisplay, 10, 30, 170, 110, sz);
		RESIZE (slotParams[i].panLabel, 280, 110, 60, 20, sz);
		RESIZE (slotParams[i].panDial, 280, 40, 60, 60, sz);
		RESIZE (slotParams[i].mixLabel, 360, 110, 60, 20, sz);
		RESIZE (slotParams[i].mixDial, 360, 40, 60, 60, sz);
		if (slotParams[i].optionWidget) slotParams[i].optionWidget->zoom (sz);
	}

	applyTheme (theme);
	drawPad ();
	show ();
}

void BOopsGUI::applyTheme (BStyles::Theme& theme)
{
	mContainer.applyTheme (theme);
	messageLabel.applyTheme (theme);
	helpButton.applyTheme (theme);
	ytButton.applyTheme (theme);

	settingsContainer.applyTheme (theme);
	playButton.applyTheme (theme);
	bypassButton.applyTheme (theme);
	stopButton.applyTheme (theme);
	sourceListBox.applyTheme (theme);
	loadButton.applyTheme (theme);
	sampleLabel.applyTheme (theme);
	sampleNameLabel.applyTheme (theme);
	if (fileChooser) fileChooser->applyTheme (theme);
	sampleAmpLabel.applyTheme (theme);
	sampleAmpDial.applyTheme (theme);
	playModeListBox.applyTheme (theme);
	onMidiListBox.applyTheme (theme);
	transportGateButton.applyTheme (theme);
	autoplayBpmLabel.applyTheme (theme);
	autoplayBpmSlider.applyTheme (theme);
	autoplayBpbLabel.applyTheme (theme);
	autoplayBpbSlider.applyTheme (theme);
	autoplayPositionLabel.applyTheme (theme);
	autoplayPositionSlider.applyTheme (theme);
	sequenceSizeSelect.applyTheme (theme);
	sequenceBaseListBox.applyTheme (theme);
	stepsListBox.applyTheme (theme);

	transportGateContainer.applyTheme (theme);
	transportGateLabel.applyTheme (theme);
	transportGatePiano.applyTheme (theme);
	transportGateOkButton.applyTheme (theme);
	transportGateCancelButton.applyTheme (theme);

	slotsContainer.applyTheme (theme);

	for (Slot& s : slots)
	{
		s.container.applyTheme (theme);
		s.addPad.applyTheme (theme);
		s.delPad.applyTheme (theme);
		s.upPad.applyTheme (theme);
		s.downPad.applyTheme (theme);
		s.effectPad.applyTheme (theme);
		s.effectsListbox.applyTheme (theme);
		s.playPad.applyTheme (theme);
	};

	pageWidget.applyTheme (theme);
	pageBackSymbol.applyTheme (theme);
	pageForwardSymbol.applyTheme (theme);
	for (Tab& t : tabs)
	{
		t.container.applyTheme (theme);
		t.icon.applyTheme (theme);
		t.playSymbol.applyTheme (theme);
		t.midiSymbol.applyTheme (theme);
		for (SymbolWidget& s : t.symbols) s.applyTheme (theme);
	}

	midiBox.applyTheme (theme);
	midiText.applyTheme (theme);
	midiStatusLabel.applyTheme (theme);
	midiStatusListBox.applyTheme (theme);
	midiChannelLabel.applyTheme (theme);
	midiChannelListBox.applyTheme (theme);
	midiNoteLabel.applyTheme (theme);
	midiNoteListBox.applyTheme (theme);
	midiValueLabel.applyTheme (theme);
	midiValueListBox.applyTheme (theme);
	midiLearnButton.applyTheme (theme);
	midiCancelButton.applyTheme (theme);
	midiOkButton.applyTheme (theme);

	monitor.applyTheme (theme);
	padSurface.applyTheme (theme);

	editContainer.applyTheme (theme);
	for (HaloToggleButton& e1 : edit1Buttons) e1.applyTheme (theme);
	for (HaloButton& e2 : edit2Buttons) e2.applyTheme (theme);

	gettingstartedContainer.applyTheme (theme);
	gettingstartedText.applyTheme (theme);


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

void BOopsGUI::onConfigureRequest (BEvents::ExposeEvent* event)
{
	Window::onConfigureRequest (event);

	sz = (getWidth() / 1240 > getHeight() / 648 ? getHeight() / 648 : getWidth() / 1240);
	resize ();
}

void BOopsGUI::onKeyPressed (BEvents::KeyEvent* event)
{
	if ((event) && (event->getKey() == BDevices::KEY_SHIFT))
	{
		monitor.setScrollable (true);
	}
}

void BOopsGUI::onKeyReleased (BEvents::KeyEvent* event)
{
	if ((event) && (event->getKey() == BDevices::KEY_SHIFT))
	{
		monitor.setScrollable (false);
	}
}

void BOopsGUI::onCloseRequest (BEvents::WidgetEvent* event)
{
	if (!event) return;
	Widget* requestWidget = event->getRequestWidget ();
	if (!requestWidget) return;

	if (requestWidget == fileChooser)
	{
		if (fileChooser->getValue() == 1.0)
		{
			sampleNameLabel.setText (fileChooser->getFileName());
			samplePath = fileChooser->getPath();
			sampleStart = fileChooser->getStart();
			sampleEnd = fileChooser->getEnd();
			sampleLoop = fileChooser->getLoop();
			sendSamplePath ();
		}

		// Close fileChooser
		mContainer.release (fileChooser);	// TODO Check why this is required
		delete fileChooser;
		fileChooser = nullptr;
		return;
	}

	Window::onCloseRequest (event);
}

void BOopsGUI::sendUiOn ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_uiOn);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendUiOff ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_uiOff);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendSlot (const int page, const int slot)
{
	Pad pads[NR_STEPS];
	for (int i = 0; i < NR_STEPS; ++i) pads[i] = patterns[page].getPad (slot, i);
	uint8_t obj_buf[1024];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_slotEvent);
	lv2_atom_forge_key(&forge, urids.bOops_pageID);
	lv2_atom_forge_int(&forge, page);
	lv2_atom_forge_key(&forge, urids.bOops_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bOops_pads);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, NR_STEPS * sizeof(Pad) / sizeof(float), (void*) pads);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendPad (const int page, const int slot, const int step)
{
	Pad pad (patterns[page].getPad (slot, step));

	uint8_t obj_buf[256];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_padEvent);
	lv2_atom_forge_key(&forge, urids.bOops_pageID);
	lv2_atom_forge_int(&forge, page);
	lv2_atom_forge_key(&forge, urids.bOops_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bOops_step);
	lv2_atom_forge_int(&forge, step);
	lv2_atom_forge_key(&forge, urids.bOops_pads);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, sizeof(Pad) / sizeof(float), (void*) &pad);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendMaxPage ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_statusEvent);
	lv2_atom_forge_key(&forge, urids.bOops_pageMax);
	lv2_atom_forge_int(&forge, pageMax);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendPlaybackPage ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_statusEvent);
	lv2_atom_forge_key(&forge, urids.bOops_pageID);
	lv2_atom_forge_int(&forge, int (pageWidget.getValue()));
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendPageProperties (const int page)
{
	uint8_t obj_buf[256];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));
	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_pagePropertiesEvent);
	lv2_atom_forge_key(&forge, urids.bOops_pageID);
	lv2_atom_forge_int(&forge, page);
	lv2_atom_forge_key(&forge, urids.bOops_pageStatus);
	lv2_atom_forge_int(&forge, tabs[page].midiWidgets[PAGE_CONTROLS_STATUS].getValue());
	lv2_atom_forge_key(&forge, urids.bOops_pageChannel);
	lv2_atom_forge_int(&forge, tabs[page].midiWidgets[PAGE_CONTROLS_CHANNEL].getValue());
	lv2_atom_forge_key(&forge, urids.bOops_pageMessage);
	lv2_atom_forge_int(&forge, tabs[page].midiWidgets[PAGE_CONTROLS_MESSAGE].getValue());
	lv2_atom_forge_key(&forge, urids.bOops_pageValue);
	lv2_atom_forge_int(&forge, tabs[page].midiWidgets[PAGE_CONTROLS_VALUE].getValue());
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendRequestMidiLearn ()
{
	uint8_t obj_buf[128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_statusEvent);
	lv2_atom_forge_key(&forge, urids.bOops_requestMidiLearn);
	lv2_atom_forge_bool(&forge, midiLearnButton.getValue() != 0.0);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendShape (const int slot)
{
	size_t size = slotParams[slot].shape.size ();

	uint8_t obj_buf[4096];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	// Load shapeBuffer
	float shapeBuffer[SHAPE_MAXNODES * 7];
	for (unsigned int i = 0; i < size; ++i)
	{
		Node node = slotParams[slot].shape.getNode (i);
		shapeBuffer[i * 7 + 0] = (float)node.nodeType;
		shapeBuffer[i * 7 + 1] = (float)node.point.x;
		shapeBuffer[i * 7 + 2] = (float)node.point.y;
		shapeBuffer[i * 7 + 3] = (float)node.handle1.x;
		shapeBuffer[i * 7 + 4] = (float)node.handle1.y;
		shapeBuffer[i * 7 + 5] = (float)node.handle2.x;
		shapeBuffer[i * 7 + 6] = (float)node.handle2.y;
	}

	// Notify shapeBuffer
	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object (&forge, &frame, 0, urids.bOops_shapeEvent);
	lv2_atom_forge_key(&forge, urids.bOops_slot);
	lv2_atom_forge_int(&forge, slot);
	lv2_atom_forge_key(&forge, urids.bOops_shapeData);
	lv2_atom_forge_vector(&forge, sizeof(float), urids.atom_Float, (uint32_t) (7 * size), &shapeBuffer);
	lv2_atom_forge_pop(&forge, &frame);
	write_function (controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendTransportGateKeys()
{
	uint8_t obj_buf[1024];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	std::vector<int> keys = {};
	for (unsigned int i = 0; (i < NR_PIANO_KEYS) && (i < transportGateKeys.size()); ++i)
	{
		if (transportGateKeys[i]) keys.push_back (i);
	}

	// Send notifications
	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object (&forge, &frame, 0, urids.bOops_transportGateKeyEvent);
	lv2_atom_forge_key(&forge, urids.bOops_transportGateKeys);
	lv2_atom_forge_vector(&forge, sizeof(int), urids.atom_Int, keys.size(), (void*) keys.data());
	lv2_atom_forge_pop(&forge, &frame);
	write_function (controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendSamplePath ()
{
	std::string path = samplePath + BUTILITIES_PATH_SLASH + sampleNameLabel.getText();
	if (path.length() >= PATH_MAX)
	{
		fprintf(stderr, "BOops.lv2#GUI: Can't send sample path. File path lenght >= %i not supported.\n", PATH_MAX);
		return;
	}

	uint8_t obj_buf[PATH_MAX + 128];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_samplePathEvent);
	lv2_atom_forge_key(&forge, urids.bOops_samplePath);
	lv2_atom_forge_path (&forge, path.c_str(), path.size() + 1);
	lv2_atom_forge_key(&forge, urids.bOops_sampleStart);
	lv2_atom_forge_long(&forge, sampleStart);
	lv2_atom_forge_key(&forge, urids.bOops_sampleEnd);
	lv2_atom_forge_long(&forge, sampleEnd);
	lv2_atom_forge_key(&forge, urids.bOops_sampleAmp);
	lv2_atom_forge_float(&forge, sampleAmpDial.getValue());
	lv2_atom_forge_key(&forge, urids.bOops_sampleLoop);
	lv2_atom_forge_bool(&forge, int32_t(sampleLoop));
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::sendSampleAmp ()
{
	uint8_t obj_buf[1024];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, urids.bOops_samplePathEvent);
	lv2_atom_forge_key(&forge, urids.bOops_sampleAmp);
	lv2_atom_forge_float(&forge, sampleAmpDial.getValue());
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, CONTROL, lv2_atom_total_size(msg), urids.atom_eventTransfer, msg);
}

void BOopsGUI::pushPage ()
{
	if (pageMax >= NR_PAGES - 1) return;

	tabs[pageMax].symbols[CLOSESYMBOL].show();
	tabs[pageMax].symbols[RIGHTSYMBOL].show();

	tabs[pageMax + 1].container.show();
	tabs[pageMax + 1].symbols[CLOSESYMBOL].show();
	tabs[pageMax + 1].symbols[LEFTSYMBOL].show();
	tabs[pageMax + 1].symbols[RIGHTSYMBOL].hide();

	if (pageMax + 1 == NR_PAGES - 1)
	{
		for (Tab& t : tabs) t.symbols[ADDSYMBOL].hide();
	}

	++pageMax;
	updatePageContainer();
}

void BOopsGUI::popPage ()
{
	if (pageMax <= 0) return;

	tabs[pageMax - 1].symbols[RIGHTSYMBOL].hide();
	if (pageMax == 1) tabs[0].symbols[CLOSESYMBOL].hide();
	tabs[pageMax].container.hide();
	for (Tab& t : tabs) t.symbols[ADDSYMBOL].show();

	if (pageAct >= pageMax) gotoPage (pageMax - 1);
	if (pageWidget.getValue() >= pageMax) pageWidget.setValue (0);

	--pageMax;
	updatePageContainer();
}

void BOopsGUI::gotoPage (const int page)
{
	if ((page < 0) || (page > pageMax)) return;

	pageAct = page;
	for (int i = 0; i < NR_PAGES; ++i)
	{
		if (i == page) tabs[i].container.rename ("activetab");
		else tabs[i].container.rename ("tab");
		tabs[i].container.applyTheme (theme);
	}
	drawPad();
	updatePageContainer();
}

void BOopsGUI::insertPage (const int page)
{
	if ((page < 0) || (pageMax >= NR_PAGES - 1)) return;

	pushPage();
	sendMaxPage();
	if (pageAct >= page) gotoPage (pageAct + 1);
	if (pageWidget.getValue() >= page) pageWidget.setValue (pageWidget.getValue() + 1);

	// Move pages
	for (int i = pageMax; i > page; --i)
	{
		patterns[i] = patterns[i - 1];
		for (int j = 0; j < NR_SLOTS; ++j) sendSlot (i, j);
		if (i == pageAct) drawPad();
		for (BWidgets::ValueWidget& m : tabs[i].midiWidgets) m.setValue (m.getValue());
	}

	// Init new page
	patterns[page].clear();
	for (int j = 0; j < NR_SLOTS; ++j) sendSlot (page, j);
	if (page == pageAct) drawPad();
	tabs[page].midiWidgets[PAGE_CONTROLS_STATUS].setValue (0);
	tabs[page].midiWidgets[PAGE_CONTROLS_CHANNEL].setValue (0);
	tabs[page].midiWidgets[PAGE_CONTROLS_MESSAGE].setValue (128);
	tabs[page].midiWidgets[PAGE_CONTROLS_VALUE].setValue (128);
}

void BOopsGUI::deletePage (const int page)
{
	if ((page < 0) || (page > pageMax)) return;

	if (pageAct > page) gotoPage (pageAct - 1);
	if (pageWidget.getValue() > page) pageWidget.setValue (pageWidget.getValue() - 1);

	for (int i = page; i < pageMax; ++i)
	{
		patterns[i] = patterns[i + 1];
		for (int j = 0; j < NR_SLOTS; ++j) sendSlot (i, j);
		if (i == pageAct) drawPad ();
		for (int j = 0; j < NR_MIDI_CTRLS; ++j)
		{
			tabs[i].midiWidgets[j].setValue (tabs[i + 1].midiWidgets[j].getValue());
		}
	}

	tabs[pageMax].midiWidgets[PAGE_CONTROLS_STATUS].setValue (0);
	tabs[pageMax].midiWidgets[PAGE_CONTROLS_CHANNEL].setValue (0);
	tabs[pageMax].midiWidgets[PAGE_CONTROLS_MESSAGE].setValue (128);
	tabs[pageMax].midiWidgets[PAGE_CONTROLS_VALUE].setValue (128);

	popPage();
	sendMaxPage();
}

void BOopsGUI::swapPage (const int page1, const int page2)
{
	if ((page1 < 0) || (page1 > pageMax) || (page2 < 0) || (page2 > pageMax)) return;

	Pattern p;
	p.clear();
	p = patterns[page1];
	patterns[page1] = patterns[page2];
	patterns[page2] = p;
	for (int j = 0; j < NR_SLOTS; ++j) sendSlot (page1, j);
	for (int j = 0; j < NR_SLOTS; ++j) sendSlot (page2, j);

	if (pageAct == page1) gotoPage (page2);
	else if (pageAct == page2) gotoPage (page1);

	if (pageWidget.getValue() == page1) pageWidget.setValue (page2);
	else if (pageWidget.getValue() == page2) pageWidget.setValue (page1);

	for (int j = 0; j < NR_MIDI_CTRLS; ++j)
	{
		double v = tabs[page1].midiWidgets[j].getValue();
		tabs[page1].midiWidgets[j].setValue (tabs[page2].midiWidgets[j].getValue());
		tabs[page2].midiWidgets[j].setValue (v);
	}
}

void BOopsGUI::updatePageContainer()
{
	if (pageMax > 9) pageOffset = LIMIT (pageOffset, 0, pageMax - 9);
	else pageOffset = 0;

	int x0 = (pageOffset == 0 ? 0 : 12 * sz);

	if (pageOffset != 0) pageBackSymbol.show();
	else pageBackSymbol.hide();

	if (pageOffset + 9 < pageMax) pageForwardSymbol.show();
	else pageForwardSymbol.hide();

	for (int p = 0; p <= pageMax; ++p)
	{
		if ((p < pageOffset) || (p > pageOffset + 9)) tabs[p].container.hide();
		else
		{
			tabs[p].container.moveTo (x0 + (p - pageOffset) * 80 * sz, 0);
			tabs[p].container.resize (78 * sz, 30 * sz);
			tabs[p].container.show();
		}
	}

	for (int p = pageMax + 1; p < NR_PAGES; ++p ) tabs[p].container.hide();

	pageBackSymbol.moveTo (0, 0);
	pageBackSymbol.resize (10 * sz, 30 * sz);
	pageForwardSymbol.moveTo (x0 + 800 * sz, 0);
	pageForwardSymbol.resize (10 * sz, 30 * sz);
}

int BOopsGUI::getSlotsSize () const
{
	int slotSize = 0;
	while ((slotSize < NR_SLOTS) && (slots[slotSize].container.getValue() > FX_NONE) && (slots[slotSize].container.getValue() < NR_FX)) ++slotSize;
	return slotSize;
}

void BOopsGUI::clearSlot (int slot)
{
	slots[slot].effectsListbox.hide();
	controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_EFFECT]->setValue (FX_NONE);
	for (int j = 0; j < NR_PARAMS; ++j)
	{
		controllerWidgets[SLOTS + slot * (SLOTS_PARAMS + NR_PARAMS) + SLOTS_PARAMS + j]->setValue (fxDefaultValues[FX_NONE][j]);
	}

	for (Pattern& p : patterns)
	{
		for (int j = 0; j < NR_STEPS; ++j) p.setPad (slot, j, Pad());
	}

	slotParams[slot].shape.setDefaultShape();
	sendShape (slot);
	if (slotParams[slot].optionWidget) slotParams[slot].optionWidget->setShape (slotParams[slot].shape);

	for (int i = 0; i < NR_PAGES; ++i) sendSlot (i, slot);
	drawPad (slot);
}

void BOopsGUI::copySlot (int dest, int source)
{
	slots[dest].effectsListbox.hide();
	for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j)
	{
		controllerWidgets[SLOTS + dest * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (controllerWidgets[SLOTS + source * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue());
	}

	for (Pattern& p : patterns)
	{
		for (int j = 0; j < NR_STEPS; ++j) p.setPad (dest, j, p.getPad (source, j));
	}

	slotParams[dest].shape = slotParams[source].shape;
	sendShape (dest);
	if (slotParams[dest].optionWidget) slotParams[dest].optionWidget->setShape (slotParams[dest].shape);

	for (int i = 0; i < NR_PAGES; ++i) sendSlot (i, dest);
	drawPad (dest);
}

void BOopsGUI::insertSlot (int slot, const BOopsEffectsIndex effect)
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
	for (Pattern& p : patterns)
	{
		for (int j = 0; j < NR_STEPS; ++j) p.setPad (slot, j, Pad());
	}
	slotParams[slot].shape.setDefaultShape();
	sendShape (slot);
	if (slotParams[slot].optionWidget) slotParams[slot].optionWidget->setShape (slotParams[slot].shape);

	for (Pattern& p : patterns) p.store();
	//updateSlots();
	for (int i = 0; i < NR_PAGES; ++i) sendSlot (pageAct, slot);
	drawPad (slot);
}

void BOopsGUI::deleteSlot (int slot)
{
	if ((slot < 0) || (slot >= NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	slot = LIMIT (slot, 0, slotSize - 1);

	// Move slots, slotParams, pads
	for (int i = slot; i < slotSize - 1; ++i) copySlot (i, i + 1);

	// Cleanup: Clear slots, slotParams, pads
	for (int i = slotSize - 1; i < NR_SLOTS; ++i) clearSlot (i);

	for (Pattern& p : patterns) p.store();
	updateSlots();
}

void BOopsGUI::swapSlots (int slot1, int slot2)
{
	if ((slot1 < 0) || (slot1 >= NR_SLOTS) || (slot2 < 0) || (slot2 >= NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	slot1 = LIMIT (slot1, 0, slotSize - 1);
	slot2 = LIMIT (slot2, 0, slotSize - 1);
	slots[slot1].effectsListbox.hide();
	slots[slot2].effectsListbox.hide();

	// Swap pads
	for (Pattern& p : patterns)
	{
		for (int j = 0; j < NR_STEPS; ++j)
		{
			Pad slot1Pad = p.getPad (slot1, j);
			p.setPad (slot1, j, p.getPad (slot2, j));
			p.setPad (slot2, j, slot1Pad);
		}
	}

	// Swap slots
	for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j)
	{
		double slot1Value = controllerWidgets[SLOTS + slot1 * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue();
		controllerWidgets[SLOTS + slot1 * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (controllerWidgets[SLOTS + slot2 * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue());
		controllerWidgets[SLOTS + slot2 * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (slot1Value);
	}

	// Swap shapes
	Shape<SHAPE_MAXNODES> slot1Shape = slotParams[slot1].shape;
	slotParams[slot1].shape = slotParams[slot2].shape;
	slotParams[slot2].shape = slot1Shape;
	sendShape (slot1);
	sendShape (slot2);
	if (slotParams[slot1].optionWidget) slotParams[slot1].optionWidget->setShape (slotParams[slot1].shape);
	if (slotParams[slot2].optionWidget) slotParams[slot2].optionWidget->setShape (slotParams[slot2].shape);

	patterns[pageAct].store();
	updateSlot (slot1);
	for (int i = 0; i < NR_PAGES; ++i) sendSlot (pageAct, slot1);
	drawPad (slot1);
	updateSlot (slot2);
	for (int i = 0; i < NR_PAGES; ++i) sendSlot (pageAct, slot2);
	drawPad (slot2);
}

void BOopsGUI::moveSlot (int source, int target)
{
	if ((source < 0) || (source >= NR_SLOTS) || (target < 0) || (target > NR_SLOTS)) return;

	int slotSize = getSlotsSize ();
	source = LIMIT (source, 0, slotSize - 1);
	target = LIMIT (target, 0, slotSize);

	// No move?
	if ((source == target) || (source + 1 == target)) updateSlot (source);

	// Neighbours? Swap
	else if (source - 1 == target) swapSlots (source, target);
	else if (source + 2 == target) swapSlots (source, target - 1);

	else
	{
		int inc = (source < target ? 1 : -1);
		int offs = (source < target ? 1 : 0);

		// Buffer source
		// Pads
		std::array<std::array<Pad, NR_STEPS>, NR_PAGES> sourcePads;
		for (int i = 0; i < NR_PAGES; ++i)
		{

			for (int j = 0; j < NR_STEPS; ++j) sourcePads[i][j] = patterns[i].getPad (source, j);
		}

		// Params
		std::array<double, SLOTS_PARAMS + NR_PARAMS> sourceParams;
		for (int i = 0; i < SLOTS_PARAMS + NR_PARAMS; ++i) sourceParams[i] = controllerWidgets[SLOTS + source * (SLOTS_PARAMS + NR_PARAMS) + i]->getValue();

		// Shape
		Shape<SHAPE_MAXNODES> sourceShape = slotParams[source].shape;

		// Move section
		for (int i = source; i != target - offs; i += inc)
		{
			// Move pads
			for (Pattern& p : patterns)
			{
				for (int j = 0; j < NR_STEPS; ++j) p.setPad (i, j, p.getPad (i + inc, j));
			}

			// Move params
			for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j)
			{
				const double param = controllerWidgets[SLOTS + (i + inc) * (SLOTS_PARAMS + NR_PARAMS) + j]->getValue();
				controllerWidgets[SLOTS + i * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (param);
			}

			// Move shapes
			slotParams[i].shape = slotParams[i + inc].shape;
			sendShape (i);
			if (slotParams[i].optionWidget) slotParams[i].optionWidget->setShape (slotParams[i].shape);
		}

		// Write target
		for (int i = 0; i < NR_PAGES; ++i)
		{
			for (int j = 0; j < NR_STEPS; ++j) patterns[i].setPad (target - offs, j, sourcePads[i][j]);
		}
		for (int j = 0; j < SLOTS_PARAMS + NR_PARAMS; ++j) controllerWidgets[SLOTS + (target - offs) * (SLOTS_PARAMS + NR_PARAMS) + j]->setValue (sourceParams[j]);
		slotParams[target - offs].shape = sourceShape;
		sendShape (target - offs);
		if (slotParams[target - offs].optionWidget) slotParams[target - offs].optionWidget->setShape (slotParams[target - offs].shape);

		// Apply changes
		for (Pattern& p : patterns) p.store();
		for (int i = source; i != target + inc - offs; i += inc)
		{
			updateSlot (i);
			for (int j = 0; j < NR_PAGES; ++j) sendSlot (j, i);
			drawPad (i);
		}
		gotoSlot (target - offs);
	}
}

void BOopsGUI::updateSlot (const int slot)
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

	if (slot <= slotSize)
	{
		RESIZE (slots[slot].container, 0, slot * 24, 260, 24, sz);
		slots[slot].container.show();
	}
	else slots[slot].container.hide();
}

void BOopsGUI::updateSlots ()
{
	for (int i = 0; i < NR_SLOTS; ++i) updateSlot (i);
}

void BOopsGUI::gotoSlot (const int slot)
{
	actSlot = slot;
	const int slotSize = getSlotsSize();
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if ((i == slot) && (i < slotSize))
		{
			gettingstartedContainer.hide();
			slotParams[i].container.show();
		}

		else slotParams[i].container.hide();
	}
	drawPad();
}

void BOopsGUI::setOptionWidget (const int slot)
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
		case FX_SURPRISE:	slotParams[slot].optionWidget = new OptionSurprise (270, 20, 640, 130, "widget");
					break;

		case FX_AMP:		slotParams[slot].optionWidget = new OptionAmp (430, 20, 80, 130, "widget");
					break;

		case FX_BALANCE:	slotParams[slot].optionWidget = new OptionBalance (430, 20, 80, 130, "widget");
					break;

		case FX_WIDTH:		slotParams[slot].optionWidget = new OptionWidth (430, 20, 80, 130, "widget");
					break;

		case FX_DELAY:		slotParams[slot].optionWidget = new OptionDelay (430, 20, 240, 130, "widget");
					break;

		case FX_CHOPPER:	slotParams[slot].optionWidget = new OptionChopper (430, 20, 560, 130, "widget");
					break;

		case FX_TAPE_STOP:	slotParams[slot].optionWidget = new OptionTapeStop (430, 20, 160, 130, "widget");
					break;

		case FX_TAPE_SPEED:	slotParams[slot].optionWidget = new OptionTapeSpeed (430, 20, 80, 130, "widget");
					break;

		case FX_SCRATCH:	slotParams[slot].optionWidget = new OptionScratch (430, 20, 480, 130, "widget", pluginPath);
					if (slotParams[slot].optionWidget) ((OptionScratch*)slotParams[slot].optionWidget)->setShape (slotParams[slot].shape);
					break;

		case FX_WOWFLUTTER:	slotParams[slot].optionWidget = new OptionWowFlutter (430, 20, 320, 130, "widget");
					break;

		case FX_BITCRUSH:	slotParams[slot].optionWidget = new OptionBitcrush (430, 20, 160, 130, "widget");
					break;

		case FX_DECIMATE:	slotParams[slot].optionWidget = new OptionDecimate (430, 20, 80, 130, "widget");
					break;

		case FX_DISTORTION:	slotParams[slot].optionWidget = new OptionDistortion (430, 20, 240, 130, "widget");
					break;

		case FX_FILTER:		slotParams[slot].optionWidget = new OptionFilter (430, 20, 240, 130, "widget");
					break;

		case FX_NOISE:		slotParams[slot].optionWidget = new OptionNoise (430, 20, 80, 130, "widget");
					break;

		case FX_CRACKLES:	slotParams[slot].optionWidget = new OptionCrackles (430, 20, 320, 130, "widget");
					break;

		case FX_STUTTER:	slotParams[slot].optionWidget = new OptionStutter (430, 20, 160, 130, "widget");
					break;

		case FX_FLANGER:	slotParams[slot].optionWidget = new OptionFlanger (430, 20, 400, 130, "widget");
					break;

		case FX_PHASER:		slotParams[slot].optionWidget = new OptionPhaser (430, 20, 480, 130, "widget");
					break;

		case FX_RINGMOD:	slotParams[slot].optionWidget = new OptionRingModulator (430, 20, 260, 130, "widget");
					break;

		case FX_OOPS:		slotParams[slot].optionWidget = new OptionOops (430, 20, 240, 130, "widget");
					break;

		case FX_WAH:		slotParams[slot].optionWidget = new OptionWah (430, 20, 720, 130, "widget", pluginPath);
					if (slotParams[slot].optionWidget) ((OptionWah*)slotParams[slot].optionWidget)->setShape (slotParams[slot].shape);
					break;

		default:		slotParams[slot].optionWidget = new OptionWidget (0, 0, 0, 0, "widget");
	}

	if (slotParams[slot].optionWidget) slotParams[slot].optionWidget->zoom (sz);
}

void BOopsGUI::loadOptions (const int slot)
{
	// Copy parameters and add optionWidget
	if (slotParams[slot].optionWidget)
	{
		slotParams[slot].container.add (*slotParams[slot].optionWidget);

		// Load values
		for (int i = 0; i < NR_OPTPARAMS; ++i)
		{
			if (slotParams[slot].optionWidget->getWidget (i)) slotParams[slot].optionWidget->setOption (i, slotParams[slot].options[i].getValue());
		}

		// ... and shape
		slotParams[slot].optionWidget->setShape (slotParams[slot].shape);

		// Load styles
		std::vector<Widget*> children = slotParams[slot].optionWidget->getChildren();
		for (Widget* w : children)
		{
			if (w)
			{
				const std::string name = w->getName();
				if (name.substr (0, 3) == "pad") w->rename (slotParams[slot].adsrDisplay.getName());
			}
		}
	}

}

void BOopsGUI::optionChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	double value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	if (dynamic_cast<ShapeWidget*>(widget))
	{
		OptionWidget* p = (OptionWidget*)widget->getParent();
		if (p)
		{
			for (int i = 0; i < NR_SLOTS; ++i)
			{
				if (p == ui->slotParams[i].optionWidget)
				{
					ui->slotParams[i].shape = (*(ShapeWidget*)widget);
					ui->sendShape (i);
					return;
				}
			}
		}
	}

	else
	{
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
}

void BOopsGUI::valueChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

			case SOURCE:		if (value == SOURCE_STREAM)
						{
							ui->loadButton.hide();
							ui->sampleLabel.hide();
							ui->sampleNameLabel.hide();
							ui->sampleAmpLabel.hide();
							ui->sampleAmpDial.hide();
						}
						else
						{
							ui->loadButton.show();
							ui->sampleLabel.show();
							ui->sampleNameLabel.show();
							ui->sampleAmpLabel.show();
							ui->sampleAmpDial.show();
						}
						break;

			case PLAY_MODE:		if  (value == AUTOPLAY)
						{
							ui->autoplayBpmLabel.show();
							ui->autoplayBpmSlider.show();
							ui->autoplayBpbLabel.show();
							ui->autoplayBpbSlider.show();
							ui->autoplayPositionLabel.show();
							ui->autoplayPositionSlider.show();
						}
						else
						{
							ui->autoplayBpmLabel.hide();
							ui->autoplayBpmSlider.hide();
							ui->autoplayBpbLabel.hide();
							ui->autoplayBpbSlider.hide();
							ui->autoplayPositionLabel.hide();
							ui->autoplayPositionSlider.hide();
						}

						if  (value == MIDI_CONTROLLED)
						{
							ui->onMidiListBox.show();
							ui->transportGateButton.show();
						}
						else
						{
							ui->onMidiListBox.hide();
							ui->transportGateButton.hide();
							ui->transportGateContainer.hide();
						}

						break;

			case AUTOPLAY_BPB:	value = floor (value);
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

									ui->slotParams[slot].container.loadImage (BColors::NORMAL, ui->pluginPath + ui->fxBgFilenames[fxnr]);

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
									ui->updateSlots();
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

	else if (widget == &ui->sampleAmpDial) ui->sendSampleAmp();

	else if (widget == &ui->pageWidget)
	{
		for (int i = 0; i < NR_PAGES; ++i)
		{
			ui->tabs[i].playSymbol.setState (i == ui->pageWidget.getValue() ? BColors::ACTIVE : BColors::INACTIVE);
		}

		ui->sendPlaybackPage();
	}

	else
	{
		for (int i = 0; i < NR_PAGES; ++i)
		{
			if
			(
				(widget == &ui->tabs[i].midiWidgets[PAGE_CONTROLS_STATUS]) ||
				(widget == &ui->tabs[i].midiWidgets[PAGE_CONTROLS_CHANNEL]) ||
				(widget == &ui->tabs[i].midiWidgets[PAGE_CONTROLS_MESSAGE]) ||
				(widget == &ui->tabs[i].midiWidgets[PAGE_CONTROLS_VALUE])
			)
			{
				ui->tabs[i].midiSymbol.setState (ui->tabs[i].midiWidgets[PAGE_CONTROLS_STATUS].getValue() >=8 ? BColors::ACTIVE : BColors::INACTIVE);
				ui->sendPageProperties (i);
				return;
			}
		}
	}
}


void BOopsGUI::pageClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::Widget* widget = event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	for (int i = 0; i <= ui->pageMax; ++i)
	{
		if (&ui->tabs[i].container == widget)
		{
			ui->gotoPage (i);
			break;
		}
	}
}

void BOopsGUI::pageSymbolClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	SymbolWidget* widget = (SymbolWidget*)event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	for (int i = 0; i <= ui->pageMax; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (&ui->tabs[i].symbols[j] == widget)
			{
				switch (j)
				{
					// Symbol +
					case ADDSYMBOL:		ui->insertPage (i + 1);
								break;

					// Symbol -
					case CLOSESYMBOL: 	ui->deletePage (i);
								break;

					// Symbol <
					case LEFTSYMBOL:	ui->swapPage (i, i - 1);
								break;

					// Symbol >
					case RIGHTSYMBOL:	ui->swapPage (i, i + 1);
								break;
				}
				return;
			}
		}
	}
}

void BOopsGUI::pagePlayClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	SymbolWidget* widget = (SymbolWidget*)event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	for (int i = 0; i <= ui->pageMax; ++i)
	{
		if (&ui->tabs[i].playSymbol == widget)
		{
			ui->pageWidget.setValue (i);
			break;
		}
	}
}

void BOopsGUI::pageScrollClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	SymbolWidget* widget = (SymbolWidget*)event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	if (widget == &ui->pageBackSymbol) --ui->pageOffset;
	else if (widget == &ui->pageForwardSymbol) ++ui->pageOffset;

	ui->updatePageContainer();
}

void BOopsGUI::midiSymbolClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	SymbolWidget* widget = (SymbolWidget*)event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	for (int i = 0; i <= ui->pageMax; ++i)
	{
		if (widget == &ui->tabs[i].midiSymbol)
		{
			ui->midiText.setText (BOOPS_LABEL_MIDI_PAGE " #" + std::to_string (i + 1));
			ui->midiStatusListBox.setValue (ui->tabs[i].midiWidgets[PAGE_CONTROLS_STATUS].getValue());
			ui->midiChannelListBox.setValue (ui->tabs[i].midiWidgets[PAGE_CONTROLS_CHANNEL].getValue());
			ui->midiNoteListBox.setValue (ui->tabs[i].midiWidgets[PAGE_CONTROLS_MESSAGE].getValue());
			ui->midiValueListBox.setValue (ui->tabs[i].midiWidgets[PAGE_CONTROLS_VALUE].getValue());
			ui->midiBox.setValue (i);
			ui->midiBox.show();
			return;
		}
	}
}

void BOopsGUI::midiButtonClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	if (widget == &ui->midiLearnButton)
	{
		if (value == 1) ui->sendRequestMidiLearn();
	}

	else if (widget == &ui->midiCancelButton)
	{
		if (value == 1)
		{
			ui->midiLearnButton.setValue (0);
			ui->midiBox.hide();
		}
	}

	else if (widget == &ui->midiOkButton)
	{
		if (value == 1)
		{
			int page = ui->midiBox.getValue();
			ui->midiLearnButton.setValue (0);
			ui->tabs[page].midiWidgets[PAGE_CONTROLS_STATUS].setValue (ui->midiStatusListBox.getValue());
			ui->tabs[page].midiWidgets[PAGE_CONTROLS_CHANNEL].setValue (ui->midiChannelListBox.getValue());
			ui->tabs[page].midiWidgets[PAGE_CONTROLS_MESSAGE].setValue (ui->midiNoteListBox.getValue());
			ui->tabs[page].midiWidgets[PAGE_CONTROLS_VALUE].setValue (ui->midiValueListBox.getValue());
			ui->midiBox.hide();
		}
	}
}

void BOopsGUI::midiStatusChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::PopupListBox* widget = (BWidgets::PopupListBox*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	BWidgets::PopupListBox& nlb = ui->midiNoteListBox;
	BWidgets::Label& nl = ui->midiNoteLabel;
	int nr = nlb.getValue();

	if (value == 11)
	{
		nlb = BWidgets::PopupListBox
		(
			210 * ui->sz, 50 * ui->sz, 160 * ui->sz, 20 * ui->sz, 0, 20 * ui->sz, 160 * ui->sz, 360 *ui->sz,
			"menu",
			BItems::ItemList ({CCLIST}),
			0
		);
		nl.setText (BOOPS_LABEL_CC);
	}

	else
	{
		nlb = BWidgets::PopupListBox
		(
			210 * ui->sz, 50 * ui->sz, 160 * ui->sz, 20 * ui->sz, 0, 20 * ui->sz, 160 * ui->sz, 360 * ui->sz,
			"menu",
			BItems::ItemList ({NOTELIST}),
			0
		);
		nl.setText (BOOPS_LABEL_NOTE);
	}

	nlb.resizeListBoxItems(BUtilities::Point (160 * ui->sz, 20 * ui->sz));
	nlb.applyTheme (ui->theme);
	nlb.setValue (nr);
}

void BOopsGUI::playStopBypassChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::effectChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ListBox* widget = (BWidgets::ListBox*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::addClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::delClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::upClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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



void BOopsGUI::downClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	PadButton* widget = (PadButton*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::menuClickedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ImageIcon* widget = (BWidgets::ImageIcon*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::effectDraggedCallback(BEvents::Event* event)
{
	if (!event) return;
	BEvents::PointerEvent* pev = (BEvents::PointerEvent*) event;
	IconPadButton* widget = (IconPadButton*) event->getWidget ();
	if (!widget) return;
	BWidgets::ValueWidget* parent = (BWidgets::ValueWidget*) widget->getParent();
	if (!parent) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	// Create insert line
	if (!ui->insLine)

	{
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

		if (slot >= 0)
		{
			// New line
			try {ui->insLine = new HLine (0, slot * 24.0 * ui->sz - 2.0, ui->slotsContainer.getWidth(), 4.0, "line");}
			catch (std::exception& exc)
			{
				std::cerr << "BOops.lv2#GUI: Can't create widget." << exc.what () << std::endl;
				return;
			}

			ui->slotsContainer.add (*ui->insLine);
			ui->insLine->applyTheme (ui->theme);
			ui->insLine->pushToBottom();

			parent->raiseToTop();
		}
	}

	// Move container
	BEvents::PointerEvent parentEvent = BEvents::PointerEvent (parent, BEvents::POINTER_DRAG_EVENT, pev->getPosition(), pev->getOrigin(), pev->getDelta(), pev->getButton());
	ui->dragAndDropCallback (&parentEvent);

	// Move line
	int index = (ui->sz > 0 ? (parent->getPosition().y + 24.0 * ui->sz) / (24.0 * ui->sz) : 0);
	index = LIMIT (index, 0, ui->getSlotsSize());
	ui->insLine->moveTo (0, index * 24.0 * ui->sz - 2.0);
}

void BOopsGUI::effectReleasedCallback(BEvents::Event* event)
{
	if (!event) return;
	IconPadButton* widget = (IconPadButton*) event->getWidget ();
	if (!widget) return;
	BWidgets::ValueWidget* parent = (BWidgets::ValueWidget*) widget->getParent();
	if (!parent) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	// Identify slot
	int slot = -1;
	for (int i = 0; i < NR_SLOTS; ++i)
	{
		if (widget == &ui->slots[i].effectPad)
		{
			slot = i;
			break;
		}
	}

	// Release following effect pad dragged
	if (ui->insLine)
	{
		// Delete line
		delete ui->insLine;
		ui->insLine = nullptr;

		// Move slot
		if (slot >= 0)
		{
			int dest = (ui->sz > 0 ? (parent->getPosition().y + 24.0 * ui->sz) / (24.0 * ui->sz) : 0);
			dest = LIMIT (dest, 0, ui->getSlotsSize());
			ui->moveSlot (slot, dest);
		}
	}

	// Release following click
	else
	{
		if (slot >= 0) ui->gotoSlot (slot);
	}
}


void BOopsGUI::edit1ChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	if (value != 1.0) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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

void BOopsGUI::edit2ChangedCallback(BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();
	if (!widget) return;
	float value = widget->getValue();
	if (value != 1.0) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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
				ui->patterns[ui->pageAct].store ();
				ui->wheelScrolled = false;
			}

			for (int r = 0; r < NR_SLOTS; ++r)
			{
				for (int s = 0; s < NR_STEPS; ++s)
				{
					ui->patterns[ui->pageAct].setPad (r, s, Pad ());
					ui->sendPad (ui->pageAct, r, s);
				}
			}

			ui->drawPad ();
			ui->patterns[ui->pageAct].store ();
		}
		break;

		case EDIT_UNDO:
		{
			std::vector<PadMessage> padMessages = ui->patterns[ui->pageAct].undo ();
			for (PadMessage const& p : padMessages)
			{
				size_t r = LIMIT (p.row, 0, NR_SLOTS);
				size_t s = LIMIT (p.step, 0, NR_STEPS);
				ui->sendPad (ui->pageAct, r, s);
			}
			ui->drawPad ();
		}
		break;

		case EDIT_REDO:
		{
			std::vector<PadMessage> padMessages = ui->patterns[ui->pageAct].redo ();
			for (PadMessage const& p : padMessages)
			{
				size_t r = LIMIT (p.row, 0, NR_SLOTS);
				size_t s = LIMIT (p.step, 0, NR_STEPS);
				ui->sendPad (ui->pageAct, r, s);
			}
			ui->drawPad ();
		}
		break;

		default:	break;
	}
}

void BOopsGUI::padsPressedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::PointerEvent* pointerEvent = (BEvents::PointerEvent*) event;
	BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
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
			ui->patterns[ui->pageAct].store ();
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
											ui->patterns[ui->pageAct].setPad (row + r, step + s, ui->clipBoard.data.at(r).at(s));
											ui->sendPad (ui->pageAct, row + r, step + s);
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
						if ((ui->dragOrigin.x < 0) || (ui->dragOrigin.y < 0))
						{
							ui->dragOrigin.x = int ((pointerEvent->getOrigin ().x - widget->getXOffset()) / (width / double (maxstep)));
							ui->dragOrigin.y = int (pointerEvent->getOrigin ().y - widget->getYOffset()) / (height / double (NR_SLOTS));
						}
						int s = (ui->dragOrigin.x < step ? ui->dragOrigin.x : step);
						int size = 1 + LIMIT (abs (step - ui->dragOrigin.x), 0, NR_STEPS);

						if (row != ui->dragOrigin.y)
						{
							ui->dragOrigin.x = step;
							ui->dragOrigin.y = row;
							s = step;
							size = 1;
						}

						const Pad oldPad = ui->patterns[ui->pageAct].getPad (row, s);

						if (!ui->padPressed) ui->deleteMode =
						(
							(oldPad.gate == float (ui->padGateDial.getValue())) &&
							(oldPad.mix == float (ui->padMixDial.getValue()))
						);
						Pad newPad = (ui->deleteMode ? Pad () : Pad (ui->padGateDial.getValue(), size, ui->padMixDial.getValue()));
						if (newPad != oldPad) ui->setPad (ui->pageAct, row, s, newPad);
					}

					ui->padPressed = true;
				}

				else if (pointerEvent->getButton() == BDevices::RIGHT_BUTTON)
				{
					ui->padGateDial.setValue (ui->patterns[ui->pageAct].getPad (row, step).gate);
					ui->padMixDial.setValue (ui->patterns[ui->pageAct].getPad (row, step).mix);
				}
			}
		}

		else if ((event->getEventType () == BEvents::BUTTON_RELEASE_EVENT) && (pointerEvent->getButton() == BDevices::LEFT_BUTTON))
		{
			ui->dragOrigin.x = -1;
			ui->dragOrigin.y = -1;

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

								const Pad pd = ui->patterns[ui->pageAct].getPad (r, clipSMin + ds);
								ui->patterns[ui->pageAct].setPad (r, clipSMin + ds, ui->patterns[ui->pageAct].getPad (r, clipSMax - ds));
								ui->sendPad (ui->pageAct, r, clipSMin + ds);
								ui->patterns[ui->pageAct].setPad (r, clipSMax - ds, pd);
								ui->sendPad (ui->pageAct, r, clipSMax - ds);
							}
						}
						ui->patterns[ui->pageAct].store ();
						ui->drawPad();
					}

					// YFLIP
					if (editNr == EDIT_YFLIP)
					{
						for (int dr = 0; dr <= int ((clipRMax + 1 - clipRMin) / 2); ++dr)
						{
							for (int s = 0; s < clipSMax; ++s)
							{

								const Pad pd = ui->patterns[ui->pageAct].getPad (clipRMin + dr, s);
								ui->patterns[ui->pageAct].setPad (clipRMin + dr, s, ui->patterns[ui->pageAct].getPad (clipRMax - dr, s));
								ui->sendPad (ui->pageAct, clipRMin + dr, s);
								ui->patterns[ui->pageAct].setPad (clipRMax - dr, s, pd);
								ui->sendPad (ui->pageAct, clipRMax - dr, s);
							}
						}
						ui->patterns[ui->pageAct].store ();
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
						for (int s = clipSMin; s <= clipSMax; ++s) padRow.push_back (ui->patterns[ui->pageAct].getPad (r, s));
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
								if (ui->patterns[ui->pageAct].getPad (r, s) != Pad())
								{
									ui->patterns[ui->pageAct].setPad (r, s,  Pad ());
									ui->sendPad (ui->pageAct, r, s);
								}
							}
						}
						ui->patterns[ui->pageAct].store ();
					}

					ui->clipBoard.ready = true;
					ui->drawPad ();
				}
			}

			else
			{
				ui->padPressed = false;
				ui->patterns[ui->pageAct].store ();
			}
		}
	}
}

void BOopsGUI::padsScrolledCallback (BEvents::Event* event)
{
	if ((event) && (event->getWidget ()) && (event->getWidget()->getMainWindow()) &&
		((event->getEventType () == BEvents::WHEEL_SCROLL_EVENT)))
	{
		BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
		BEvents::WheelEvent* wheelEvent = (BEvents::WheelEvent*) event;

		// Get size of drawing area
		const double width = ui->padSurface.getEffectiveWidth ();
		const double height = ui->padSurface.getEffectiveHeight ();

		const int maxstep = ui->controllerWidgets[STEPS]->getValue ();
		const int step =  int ((wheelEvent->getPosition ().x - widget->getXOffset()) / (width / double (maxstep)));
		const int row = int (wheelEvent->getPosition ().y - widget->getYOffset()) / (height / double (NR_SLOTS));

		if ((row >= 0) && (row < NR_SLOTS) && (step >= 0) && (step < maxstep))
		{
			Pad pd = ui->patterns[ui->pageAct].getPad (row, step);
			pd.mix = LIMIT (pd.mix + 0.01 * wheelEvent->getDelta().y, 0.0, 1.0);
			ui->patterns[ui->pageAct].setPad (row, step, pd);
			ui->sendPad (ui->pageAct, row, step);
			ui->drawPad (row, step);
			ui->wheelScrolled = true;
		}
	}
}

void BOopsGUI::padsFocusedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::FocusEvent* focusEvent = (BEvents::FocusEvent*) event;
	BWidgets::DrawingSurface* widget = (BWidgets::DrawingSurface*) event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	// Get size of drawing area
	const double width = ui->padSurface.getEffectiveWidth ();
	const double height = ui->padSurface.getEffectiveHeight ();

	const int maxstep = ui->controllerWidgets[STEPS]->getValue ();
	const int step =  int ((focusEvent->getPosition ().x - widget->getXOffset()) / (width / double (maxstep)));
	const int row = int (focusEvent->getPosition ().y - widget->getYOffset()) / (height / double (NR_SLOTS));

	if ((row >= 0) && (row < NR_SLOTS) && (step >= 0) && (step < maxstep))
	{
		const Pad pd = ui->patterns[ui->pageAct].getPad (row, ui->getPadOrigin (ui->pageAct, row, step));

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

void BOopsGUI::transportGateButtonClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::Widget* widget = event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	if (widget == (BWidgets::Widget*)&ui->transportGateOkButton)
	{
		ui->transportGateKeys = ui->transportGatePiano.getPressedKeys();
		ui->sendTransportGateKeys();
	}

	else if (widget == (BWidgets::Widget*)&ui->transportGateCancelButton) ui->transportGatePiano.pressKeys (ui->transportGateKeys);

	if (ui->transportGateContainer.isVisible()) ui->transportGateContainer.hide();
	else ui->transportGateContainer.show();
}

void BOopsGUI::loadButtonClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::Widget* widget = event->getWidget ();
	if (!widget) return;
	BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
	if (!ui) return;

	if (ui->fileChooser) delete ui->fileChooser;
	ui->fileChooser = new SampleChooser
	(
		200, 140, 640, 400, "filechooser", ui->samplePath,
		std::vector<BWidgets::FileFilter>
		{
			BWidgets::FileFilter {"All files", std::regex (".*")},
			BWidgets::FileFilter {"Audio files", std::regex (".*\\.((wav)|(wave)|(aif)|(aiff)|(au)|(sd2)|(flac)|(caf)|(ogg)|(mp3))$", std::regex_constants::icase)}
		},
		"Open");
	if (ui->fileChooser)
	{
		const std::string filename = ui->sampleNameLabel.getText();
		if (filename != "")
		{
			ui->fileChooser->setFileName (ui->sampleNameLabel.getText());
			ui->fileChooser->setStart (ui->sampleStart);
			ui->fileChooser->setEnd (ui->sampleEnd);
			ui->fileChooser->setLoop (ui->sampleLoop);
		}

		RESIZE ((*ui->fileChooser), 200, 140, 640, 400, ui->sz);
		ui->fileChooser->applyTheme (ui->theme);
		ui->fileChooser->selectFilter ("Audio files");
		ui->mContainer.add (*ui->fileChooser);
	}
}

void BOopsGUI::helpButtonClickedCallback (BEvents::Event* event)
{
	char cmd[] = WWW_BROWSER_CMD;
	char param[] = HELP_URL;
	char* argv[] = {cmd, param, NULL};
	std::cerr << "BOops.lv2#GUI: Call " << HELP_URL << " for help.\n";
	if (BUtilities::vsystem (argv) == -1) std::cerr << "BOops.lv2#GUI: Couldn't fork.\n";
}

void BOopsGUI::ytButtonClickedCallback (BEvents::Event* event)
{
	char cmd[] = WWW_BROWSER_CMD;
	char param[] = YT_URL;
	char* argv[] = {cmd, param, NULL};
	std::cerr << "BOops.lv2#GUI: Call " << YT_URL << " for tutorial video.\n";
	if (BUtilities::vsystem (argv) == -1) std::cerr << "BOops.lv2#GUI: Couldn't fork.\n";
}

int BOopsGUI::getPadOrigin (const int page, const int slot, const int step) const
{
	for (int i = step; i >= 0; --i)
	{
		Pad pd = patterns[page].getPad (slot, i);
		if ((pd.gate != 0) && (pd.size != 0) && (pd.mix != 0))
		{
			if (i + pd.size > step) return i;
			else return step;
		}
	}

	return step;
}

void BOopsGUI::setPad (const int page, const int slot, const int step, const Pad pad)
{
	const int size = LIMIT (pad.size, 1, NR_STEPS - step);
	const Pad oPad = patterns[page].getPad (slot, step);

	// Check if overlap with previous pad
	if (step >= 1)
	{
		int pStep = getPadOrigin (page, slot, step - 1);
		Pad pPad = patterns[page].getPad (slot, pStep);
		if (pPad.gate && pPad.size && pPad.mix)
		{
			// Previous pad overlaps start of actual pad: clip previous pad
			if (pStep + pPad.size > step)
			{
				Pad newPad = pPad;
				newPad.size = step - pStep;
				setPad (page, slot, pStep, newPad);
			}

			// Previous pad overlaps even the end of actual pad: create a pad after the end
			if (pStep + pPad.size > step + size)
			{
				Pad newPad = pPad;
				newPad.size = pPad.size - (step - pStep) - size;
				setPad (page, slot, step + size, newPad);
			}
		}
	}

	// Actual pad hides start of next pad
	for (int i = step + 1; i < step + size; ++i)
	{
		Pad nPad = patterns[page].getPad (slot, i);
		if (nPad.gate && nPad.size && nPad.mix)
		{
			// Delete next pad
			setPad (page, slot, i, Pad());

			// Next pad exceeds end of actual pad: create a pad after the end
			if (i + nPad.size > step + size)
			{
				Pad newPad = nPad;
				newPad.size = nPad.size - (step - i) - size;
				setPad (page, slot, step + size, newPad);
			}
		}
	}

	patterns[page].setPad (slot, step, pad);
	sendPad (page, slot, step);
	if (page == pageAct) drawPad (slot, step);

	// Show removed pads
	for (int i = size; i < oPad.size; ++i) drawPad (slot, step + i);
}

void BOopsGUI::drawPad ()
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	int maxstep = controllerWidgets[STEPS]->getValue ();
	for (int row = 0; row < NR_SLOTS; ++row)
	{
		for (int step = 0; step < maxstep; step += (patterns[pageAct].getPad (row, step).size > 1 ? patterns[pageAct].getPad (row, step).size : 1))
		{
			drawPad (cr, row, step);
		}
	}
	cairo_destroy (cr);
	padSurface.update();
}

void BOopsGUI::drawPad (const int slot)
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	int maxstep = controllerWidgets[STEPS]->getValue ();
	for (int step = 0; step < maxstep; step += (patterns[pageAct].getPad (slot, step).size > 1 ? patterns[pageAct].getPad (slot, step).size : 1))
	{
		drawPad (cr, slot, step);
	}
	cairo_destroy (cr);
	padSurface.update();
}

void BOopsGUI::drawPad (const int row, const int step)
{
	cairo_surface_t* surface = padSurface.getDrawingSurface();
	cairo_t* cr = cairo_create (surface);
	drawPad (cr, row, step);
	cairo_destroy (cr);
	padSurface.update();
}

void BOopsGUI::drawPad (cairo_t* cr, const int row, const int step)
{
	int maxstep = controllerWidgets[STEPS]->getValue ();
	if ((!cr) || (cairo_status (cr) != CAIRO_STATUS_SUCCESS) || (row < 0) || (row >= NR_SLOTS) || (step < 0) || (step >= maxstep)) return;

	// Get origin and size of pad data
	const int p0 = getPadOrigin (pageAct, row, step);
	const Pad pd = patterns[pageAct].getPad (row, p0);
	const int ps = LIMIT (pd.size, 1.0, maxstep - p0);

	// Get size of drawing area
	const double width = padSurface.getEffectiveWidth ();
	const double height = padSurface.getEffectiveHeight ();
	const double w1 = width / double (maxstep);
	const double w = double (ps) * w1;
	const double h = height / NR_SLOTS;
	const double x = double (p0) * w1;
	const double y = row * h;
	const double xr = round (x);
	const double yr = round (y);
	const double wr = round (x + w) - xr;
	const double hr = round (y + h) - yr;

	// Draw background
	// Odd or even?
	BColors::Color bg = ((int (p0 / 4) % 2) ? oddPadBgColor : evenPadBgColor);
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
	color.applyBrightness (pd.mix - 1.0);
	if (p0 <= int (cursor) && (p0 + ps > int (cursor))) color.applyBrightness (0.75);
	drawButton (cr, xr + 1, yr + 1, wr - 2, hr - 2, color);
}


static LV2UI_Handle instantiate (const LV2UI_Descriptor *descriptor,
						  const char *plugin_uri,
						  const char *bundle_path,
						  LV2UI_Write_Function write_function,
						  LV2UI_Controller controller,
						  LV2UI_Widget *widget,
						  const LV2_Feature *const *features)
{
	PuglNativeView parentWindow = 0;
	LV2UI_Resize* resize = NULL;

	if (strcmp(plugin_uri, BOOPS_URI) != 0)
	{
		std::cerr << "BOops.lv2#GUI: GUI does not support plugin with URI " << plugin_uri << std::endl;
		return NULL;
	}

	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = (PuglNativeView) features[i]->data;
		else if (!strcmp(features[i]->URI, LV2_UI__resize)) resize = (LV2UI_Resize*)features[i]->data;
	}
	if (parentWindow == 0) std::cerr << "BOops.lv2#GUI: No parent window.\n";

	// New instance
	BOopsGUI* ui;
	try {ui = new BOopsGUI (bundle_path, features, parentWindow);}
	catch (std::exception& exc)
	{
		std::cerr << "BOops.lv2#GUI: Instantiation failed. " << exc.what () << std::endl;
		return NULL;
	}

	ui->controller = controller;
	ui->write_function = write_function;

	// Reduce min GUI size for small displays
	double sz = 1.0;
	int screenWidth  = getScreenWidth ();
	int screenHeight = getScreenHeight ();
	if ((screenWidth < 860) || (screenHeight < 480)) sz = 0.5;
	else if ((screenWidth < 1280) || (screenHeight < 680)) sz = 0.66;
	if (resize) resize->ui_resize(resize->handle, 1240 * sz, 648 * sz);

	*widget = (LV2UI_Widget) puglGetNativeWindow (ui->getPuglView ());

	ui->sendUiOn();

	return (LV2UI_Handle) ui;
}

static void cleanup(LV2UI_Handle ui)
{
	BOopsGUI* self = (BOopsGUI*) ui;
	if (self) delete self;
}

static void port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	BOopsGUI* self = (BOopsGUI*) ui;
	if (self) self->port_event(port_index, buffer_size, format, buffer);
}

static int call_idle (LV2UI_Handle ui)
{
	BOopsGUI* self = (BOopsGUI*) ui;
	if (self) self->handleEvents ();
	return 0;
}

static int call_resize (LV2UI_Handle ui, int width, int height)
{
	BOopsGUI* self = (BOopsGUI*) ui;
	if (self)
	{
		BEvents::ExposeEvent* ev = new BEvents::ExposeEvent (self, self, BEvents::CONFIGURE_REQUEST_EVENT, self->getPosition().x, self->getPosition().y, width, height);
		self->addEventToQueue (ev);
	}
	return 0;
}

static const LV2UI_Idle_Interface idle = {call_idle};
static const LV2UI_Resize resize = {nullptr, call_resize};

static const void* extension_data(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) return &idle;
	else if(!strcmp(uri, LV2_UI__resize)) return &resize;
	else return NULL;
}

static const LV2UI_Descriptor guiDescriptor = {
		BOOPS_GUI_URI,
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
