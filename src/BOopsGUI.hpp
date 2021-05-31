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

#ifndef BOOPSGUI_HPP_
#define BOOPSGUI_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <iostream>
#include <algorithm>

#include "BUtilities/Path.hpp"
#include "BWidgets/Widget.hpp"
#include "BWidgets/Window.hpp"
#include "BWidgets/Label.hpp"
#include "BWidgets/PopupListBox.hpp"
#include "BWidgets/HPianoRoll.hpp"
#include "BWidgets/TextButton.hpp"
#include "BWidgets/TextToggleButton.hpp"
#include "BWidgets/HSlider.hpp"
#include "screen.h"

#include "HaloButton.hpp"
#include "HaloToggleButton.hpp"
#include "PadSurface.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Urids.hpp"
#include "Pad.hpp"
#include "PadMessage.hpp"
#include "Journal.hpp"
#include "Pattern.hpp"
#include "PadButton.hpp"
#include "LoadButton.hpp"
#include "IconPadButton.hpp"
#include "PadToggleButton.hpp"
#include "ValueSelect.hpp"
#include "MidiToggleButton.hpp"
#include "CurveChart.hpp"
#include "HSlider.hpp"
#include "Dial.hpp"
#include "Shape.hpp"
#include "MonitorWidget.hpp"
#include "SampleChooser.hpp"
#include "HLine.hpp"
#include "SymbolWidget.hpp"
#include "PatternChooser.hpp"

#ifdef LOCALEFILE
#include LOCALEFILE
#else
#include "Locale_EN.hpp"
#endif

#define BG_FILE "inc/surface.png"
#define HELP_URL "https://github.com/sjaehn/BOops/blob/master/README.md"
#define YT_URL "https://www.youtube.com/watch?v=bGUmZHWqdkE"
#define WWW_BROWSER_CMD "x-www-browser"

class OptionWidget;	// Forward declaration

#define RESIZE(widget, x, y, w, h, sz) {widget.moveTo ((x) * (sz), (y) * (sz)); widget.resize ((w) * (sz), (h) * (sz));}

enum editIndex
{
	EDIT_CUT	= 0,
	EDIT_COPY	= 1,
	EDIT_XFLIP	= 2,
	EDIT_YFLIP	= 3,
	EDIT_PASTE	= 4,
	EDIT_RESET	= 5,
	EDIT_UNDO	= 6,
	EDIT_REDO	= 7,
	EDIT_LOAD	= 8,
	EDIT_SAVE	= 9,
	MAXEDIT		= 10
};

enum PageControlsIndex
{
	PAGE_CONTROLS_STATUS	= 0,
	PAGE_CONTROLS_CHANNEL	= 1,
	PAGE_CONTROLS_MESSAGE	= 2,
	PAGE_CONTROLS_VALUE	= 3,
};

const std::string editLabels[MAXEDIT] =
{
	BOOPS_LABEL_SELECT_CUT,
	BOOPS_LABEL_SELECT_COPY,
	BOOPS_LABEL_SELECT_XFLIP,
	BOOPS_LABEL_SELECT_YFLIP,
	BOOPS_LABEL_PASTE,
	BOOPS_LABEL_RESET,
	BOOPS_LABEL_UNDO,
	BOOPS_LABEL_REDO,
	BOOPS_LABEL_LOAD,
	BOOPS_LABEL_SAVE
};

class BOopsGUI : public BWidgets::Window
{
public:
	BOopsGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow);
	~BOopsGUI ();
	void port_event (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
	void sendUiOn ();
	void sendUiOff ();
	void sendMaxPage ();
	void sendPlaybackPage ();
	void sendPageProperties (const int page);
	void sendRequestMidiLearn ();
	void sendEditorPage ();
	void sendEditorSlot ();
	void sendSlot (const int page, const int slot);
	void sendPad (const int page, const int slot, const int step);
	void sendShape (const int slot);
	void sendTransportGateKeys();
	void sendSamplePath();
	void sendSampleAmp();
	void pushPage ();
	void popPage ();
	void gotoPage (const int page);
	void insertPage (const int page);
	void deletePage (const int page);
	void swapPage (const int page1, const int page2);
	void updatePageContainer ();
	virtual void onConfigureRequest (BEvents::ExposeEvent* event) override;
	virtual void onKeyPressed (BEvents::KeyEvent* event) override;
	virtual void onKeyReleased (BEvents::KeyEvent* event) override;
	virtual void onCloseRequest (BEvents::WidgetEvent* event) override;
	void applyTheme (BStyles::Theme& theme) override;

	static void optionChangedCallback(BEvents::Event* event);

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;

private:
	static void valueChangedCallback(BEvents::Event* event);
	static void pageClickedCallback(BEvents::Event* event);
	static void pageSymbolClickedCallback(BEvents::Event* event);
	static void pagePlayClickedCallback(BEvents::Event* event);
	static void pageScrollClickedCallback(BEvents::Event* event);
	static void midiSymbolClickedCallback(BEvents::Event* event);
	static void midiButtonClickedCallback(BEvents::Event* event);
	static void midiStatusChangedCallback(BEvents::Event* event);
	static void playStopBypassChangedCallback(BEvents::Event* event);
	static void effectChangedCallback(BEvents::Event* event);
	static void addClickedCallback(BEvents::Event* event);
	static void delClickedCallback(BEvents::Event* event);
	static void upClickedCallback(BEvents::Event* event);
	static void downClickedCallback(BEvents::Event* event);
	static void menuClickedCallback(BEvents::Event* event);
	static void effectDraggedCallback(BEvents::Event* event);
	static void effectReleasedCallback(BEvents::Event* event);
	static void edit1ChangedCallback(BEvents::Event* event);
	static void edit2ChangedCallback(BEvents::Event* event);
	static void edit3ChangedCallback(BEvents::Event* event);
	static void padsPressedCallback (BEvents::Event* event);
	static void padsScrolledCallback (BEvents::Event* event);
	static void padsFocusedCallback (BEvents::Event* event);
	static void transportGateButtonClickedCallback (BEvents::Event* event);
	static void sampleLoadButtonClickedCallback (BEvents::Event* event);
	static void helpButtonClickedCallback (BEvents::Event* event);
	static void ytButtonClickedCallback (BEvents::Event* event);
	virtual void resize () override;
	int getSlotsSize () const;
	void clearSlot (int slot);
	void copySlot (int dest, int source);
	void insertSlot (int slot, const BOopsEffectsIndex effect);
	void deleteSlot (int slot);
	void swapSlots (int slot1, int slot2);
	void moveSlot (int source, int target);
	void updateSlot (const int slot);
	void updateSlots ();
	void gotoSlot (const int slot);
	void setOptionWidget (const int slot);
	void loadOptions (const int slot);
	int getPadOrigin (const int page, const int slot, const int step) const;
	void setPad (const int page, const int slot, const int step, const Pad pad);
	void drawPad ();
	void drawPad (const int slot);
	void drawPad (const int slot, const int step);
	void drawPad (cairo_t* cr, const int slot, const int step);

	std::string pluginPath;
	double sz;
	cairo_surface_t* bgImageSurface;
	std::string samplePath;
	int64_t sampleStart;
	int64_t sampleEnd;
	bool sampleLoop;

	BOopsURIDs urids;
	LV2_Atom_Forge forge;

	// Controllers
	std::array<BWidgets::ValueWidget*, NR_CONTROLLERS> controllerWidgets;

	// Pages
	int pageAct;
	int pageMax;
	int pageOffset;

	//Pads
	std::array<Pattern, NR_PAGES> patterns;

	struct ClipBoard
	{
		std::vector<std::vector<Pad>> data;
		std::pair<int, int> origin;
		std::pair<int, int> extends;
		bool ready = true;
		std::chrono::steady_clock::time_point time;
	};

	ClipBoard clipBoard;

	// Cursor
	double cursor;
	bool wheelScrolled;
	bool padPressed;
	bool deleteMode;
	int actSlot;

	struct Coords
	{
		int x;
		int y;
	};

	Coords dragOrigin;

	//Widgets
	BWidgets::Widget mContainer;
	BWidgets::Label messageLabel;
	HaloButton helpButton;
	HaloButton ytButton;

	BWidgets::Widget settingsContainer;
	HaloToggleButton playButton;
	HaloToggleButton bypassButton;
	HaloButton stopButton;
	BWidgets::Label sourceLabel;
	BWidgets::PopupListBox sourceListBox;
	LoadButton loadButton;
	BWidgets::Label sampleLabel;
	BWidgets::Label sampleNameLabel;
	SampleChooser* sampleChooser;
	BWidgets::Label sampleAmpLabel;
	Dial sampleAmpDial;
	BWidgets::Label modeLabel;
	BWidgets::PopupListBox playModeListBox;
	BWidgets::PopupListBox onMidiListBox;
	BWidgets::HPianoRoll transportGateButton;
	BWidgets::Label autoplayBpmLabel;
	HSlider autoplayBpmSlider;
	BWidgets::Label autoplayBpbLabel;
	HSlider autoplayBpbSlider;
	BWidgets::Label autoplayPositionLabel;
	BWidgets::HSlider autoplayPositionSlider;
	BWidgets::Label sequenceSizeLabel;
	ValueSelect sequenceSizeSelect;
	BWidgets::PopupListBox sequenceBaseListBox;
	BWidgets::Label stepsLabel;
	BWidgets::PopupListBox stepsListBox;

	BWidgets::Widget transportGateContainer;
	BWidgets::Label transportGateLabel;
	BWidgets::HPianoRoll transportGatePiano;
	BWidgets::TextButton transportGateOkButton;
	BWidgets::TextButton transportGateCancelButton;
	std::vector<bool> transportGateKeys;

	BWidgets::Widget slotsContainer;
	struct Slot
	{
		BWidgets::ValueWidget container;
		PadButton addPad;
		PadButton delPad;
		PadButton upPad;
		PadButton downPad;
		IconPadButton effectPad;
		BWidgets::ListBox effectsListbox;
		PadToggleButton playPad;
	};

	std::array<Slot, NR_SLOTS> slots;
	HLine* insLine;

	BWidgets::ValueWidget pageWidget;
	SymbolWidget pageBackSymbol;
	SymbolWidget pageForwardSymbol;

	struct Tab
	{
		BWidgets::Widget container;
		BWidgets::ImageIcon icon;
		SymbolWidget playSymbol;
		SymbolWidget midiSymbol;
		std::array<SymbolWidget, 4> symbols;
		std::array<BWidgets::ValueWidget, 4> midiWidgets;
	};

	std::array<Tab, NR_PAGES> tabs;

	BWidgets::ValueWidget midiBox;
	BWidgets::Label midiText;
	BWidgets::Label midiStatusLabel;
	BWidgets::PopupListBox midiStatusListBox;
	BWidgets::Label midiChannelLabel;
	BWidgets::PopupListBox midiChannelListBox;
	BWidgets::Label midiNoteLabel;
	BWidgets::PopupListBox midiNoteListBox;
	BWidgets::Label midiValueLabel;
	BWidgets::PopupListBox midiValueListBox;
	BWidgets::TextToggleButton midiLearnButton;
	BWidgets::TextButton midiCancelButton;
	BWidgets::TextButton midiOkButton;

	MonitorWidget monitor;
	PadSurface padSurface;

	BWidgets::Widget editContainer;
	std::array<HaloToggleButton, EDIT_RESET> edit1Buttons;
	std::array<HaloButton, EDIT_LOAD - EDIT_RESET> edit2Buttons;
	std::array<HaloButton, MAXEDIT - EDIT_LOAD> edit3Buttons;
	PatternChooser* patternChooser;

	struct SlotParam
	{
		BWidgets::ImageIcon container;
		BWidgets::ImageIcon nrIcon;
		BWidgets::ImageIcon nameIcon;
		BWidgets::Label attackLabel;
		BWidgets::Label decayLabel;
		BWidgets::Label sustainLabel;
		BWidgets::Label releaseLabel;
		HSlider attackSlider;
		HSlider decaySlider;
		HSlider sustainSlider;
		HSlider releaseSlider;
		CurveChart adsrDisplay;
		BWidgets::Label panLabel;
		Dial panDial;
		BWidgets::Label mixLabel;
		Dial mixDial;
		std::array<BWidgets::ValueWidget, NR_OPTPARAMS> options;
		Shape<SHAPE_MAXNODES> shape;
		OptionWidget* optionWidget;
	};

	std::array<SlotParam, NR_SLOTS> slotParams;

	BWidgets::ImageIcon gettingstartedContainer;
	BWidgets::Text gettingstartedText;

	//BWidgets::Widget padParamContainer;
	BWidgets::Label padGateLabel;
	Dial padGateDial;
	BWidgets::Label padMixLabel;
	Dial padMixDial;

	// Definition of styles
	BColors::ColorSet fgColors = {{{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet txColors = {{{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet buttonBgColors = {{{0.4, 0.4, 0.4, 1.0}, {0.6, 0.6, 0.6, 1.0}, {0.05, 0.05, 0.05, 1.0}, {0.0, 0.0, 0.0, 1.0}}};
	BColors::ColorSet bgColors = {{{0.15, 0.15, 0.15, 1.0}, {0.3, 0.3, 0.3, 1.0}, {0.05, 0.05, 0.05, 1.0}, {0.0, 0.0, 0.0, 1.0}}};
	BColors::ColorSet tgBgColors = {{{0.0, 0.03, 0.06, 1.0}, {0.3, 0.3, 0.3, 1.0}, {0.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 1.0}}};
	BColors::ColorSet ltColors = {{{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.25, 0.25, 0.25, 1.0}, {0.0, 0.0, 0.0, 1.0}}};
	BColors::ColorSet wvColors = {{{1.0, 1.0, 1.0, 0.15}, {1.0, 1.0, 1.0, 0.15}, {0.25, 0.25, 0.25, 0.15}, {0.0, 0.0, 0.0, 0.15}}};
	BColors::ColorSet blkColors = {{{0.0, 0.0, 0.0, 0.75}, {0.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 0.25}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::Color ink = {0.0, 0.25, 0.5, 1.0};
	BColors::Color evenPadBgColor = {0.0, 0.05, 0.1, 1.0};
	BColors::Color oddPadBgColor = {0.0, 0.0, 0.0, 1.0};

	std::array<BColors::ColorSet, NR_FX> padColors =
	{{
		{{{0.25, 0.25, 0.25, 1.0}, {0.35, 0.35, 0.35, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},	// FX_NONE
		{{{0.75, 0.75, 0.75, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_SURPRISE
		{{{0.75, 0.75, 0.0, 1.0}, {1.0, 0.25, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_AMP
		{{{0.7, 0.8, 0.0, 1.0}, {1.0, 0.35, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_BALANCE
		{{{0.6, 0.85, 0.0, 1.0}, {1.0, 0.5, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_WIDTH
		{{{0.5, 0.9, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_PITCH
		{{{0.0, 0.98, 0.2, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_DELAY
		{{{0.0, 0.95, 0.35, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_REVERSER
		{{{0.0, 0.8, 0.7, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_CHOPPER
		{{{0.0, 0.7, 0.8, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_JUMBLER
		{{{0.0, 0.2, 0.98, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_TAPE_STOP
		{{{0.0, 0.0, 1.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_TAPE_START
		{{{0.2, 0.0, 0.98, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_TAPE_SPEED
		{{{0.7, 0.0, 0.8, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_SCRATCH
		{{{0.35, 0.0, 0.95, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_WOWFLUTTER
		{{{0.95, 0.0, 0.35, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_BITCRUSH
		{{{0.98, 0.0, 0.2, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_DECIMATE
		{{{1.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_DISTORTION
		{{{0.98, 0.2, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_FILTER
		{{{0.75, 0.0, 0.75, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_NOISE
		{{{0.8, 0.0, 0.7, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_CRACKLES
		{{{0.0, 0.75, 0.75, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_STUTTER
		{{{0.95, 0.35, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_FLANGER
		{{{0.9, 0.5, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_PHASER
		{{{0.85, 0.6, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_RINGMOD
		{{{1.0, 0.65, 0.65, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_OOPS
		{{{0.8, 0.7, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_WAH
		{{{0.35, 0.95, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_REVERB
		{{{0.2, 0.90, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_GALACTIC
		{{{0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}}		// FX_INFINITY
	}};

	std::array<std::string, NR_FX> fxBgFilenames = {BOOPSFXBGNAMES};

	BStyles::Border border = {{ink, 1.0}, 0.0, 2.0, 0.0};
	BStyles::Border menuBorder = {{BColors::grey, 1.0}, 0.0, 0.0, 0.0};
	BStyles::Border labelborder = {BStyles::noLine, 4.0, 0.0, 0.0};
	BStyles::Border boxlabelborder = {{BColors::grey, 1.0}, 0.0, 3.0, 0.0};
	BStyles::Border focusborder = BStyles::Border (BStyles::Line (BColors::Color (0.0, 0.0, 0.0, 0.5), 2.0));
	BStyles::Border padborder = {BStyles::noLine, 1.0, 0.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill tabBg = BStyles::Fill (BColors::Color (1.0, 1.0, 1.0, 0.25));
	BStyles::Fill activeTabBg = BStyles::Fill (BColors::Color (1.0, 1.0, 1.0, 0.75));
	BStyles::Fill menuBg = BStyles::Fill (BColors::Color (0.0, 0.0, 0.05, 1.0));
	BStyles::Fill screenBg = BStyles::Fill (BColors::Color (0.0, 0.0, 0.0, 0.8));
	BStyles::Fill boxBg = BStyles::Fill (BColors::Color (0.0, 0.0, 0.0, 0.9));
	BStyles::Font ctLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
   	BStyles::Font tLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 12.0,
   						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::Font tgLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::Font lfLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   BStyles::TEXT_ALIGN_LEFT, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::Font boldLfLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 12.0,
						   BStyles::TEXT_ALIGN_LEFT, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::Font mdLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 10.0,
						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::Font smLabelFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 8.0,
						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::StyleSet defaultStyles = {"default", {{"background", STYLEPTR (&BStyles::noFill)},
					  {"border", STYLEPTR (&BStyles::noBorder)}}};
	BStyles::StyleSet labelStyles = {"labels", {{"background", STYLEPTR (&BStyles::noFill)},
					{"border", STYLEPTR (&labelborder)},
					{"textcolors", STYLEPTR (&fgColors)},
					{"font", STYLEPTR (&ctLabelFont)}}};
	BStyles::StyleSet smlabelStyles = {"labels", {{"background", STYLEPTR (&BStyles::noFill)},
					  {"border", STYLEPTR (&labelborder)},
					  {"textcolors", STYLEPTR (&fgColors)},
					  {"font", STYLEPTR (&smLabelFont)}}};
	BStyles::StyleSet focusStyles = {"labels", {{"background", STYLEPTR (&screenBg)},
					{"border", STYLEPTR (&focusborder)},
					{"textcolors", STYLEPTR (&fgColors)},
					{"font", STYLEPTR (&lfLabelFont)}}};
	BStyles::StyleSet padStyles =   {"pads", {{"background", STYLEPTR (&BStyles::noFill)},
					{"border", STYLEPTR (&padborder)},
					{"bgcolors", STYLEPTR (&bgColors)},
					{"fgcolors", STYLEPTR (&fgColors)},
					{"textcolors", STYLEPTR (&txColors)},
					{"font", STYLEPTR (&mdLabelFont)}}};

	BStyles::Theme theme = BStyles::Theme
	({
		defaultStyles,
		{"B.Oops", 				{{"background", STYLEPTR (&BStyles::blackFill)},
					 		 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"main",		 		{{"background", STYLEPTR (&widgetBg)},
					 		 {"border", STYLEPTR (&BStyles::noBorder)}}},
 		{"monitor", 				{{"uses", STYLEPTR (&defaultStyles)},
 					 		 {"fgcolors", STYLEPTR (&wvColors)}}},
 		{"line", 				{{"uses", STYLEPTR (&defaultStyles)},
 					 		 {"bgcolors", STYLEPTR (&fgColors)}}},
		{"widget", 				{{"uses", STYLEPTR (&defaultStyles)}}},
		{"widget/focus",			{{"uses", STYLEPTR (&focusStyles)}}},
		{"screen", 				{{"background", STYLEPTR (&screenBg)},
					 		 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"tab", 				{{"background", STYLEPTR (&tabBg)},
					 		 {"border", STYLEPTR (&BStyles::noBorder)},
			 		 		 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"activetab", 				{{"background", STYLEPTR (&activeTabBg)},
					 		 {"border", STYLEPTR (&BStyles::noBorder)},
			 		 		 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"symbol", 				{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"symbol/focus",			{{"uses", STYLEPTR (&focusStyles)}}},
		{"box", 				{{"background", STYLEPTR (&boxBg)},
							 {"border", STYLEPTR (&border)}}},
		{"box/focus",				{{"uses", STYLEPTR (&focusStyles)}}},
		{"boxlabel",				{{"background", STYLEPTR (&boxBg)},
					 		 {"border", STYLEPTR (&boxlabelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
 					 		 {"font", STYLEPTR (&lfLabelFont)}}},
 		{"smboxlabel",				{{"background", STYLEPTR (&boxBg)},
 					 		 {"border", STYLEPTR (&boxlabelborder)},
 					 		 {"textcolors", STYLEPTR (&BColors::whites)},
  					 		 {"font", STYLEPTR (&smLabelFont)}}},
		{"button", 				{{"background", STYLEPTR (&BStyles::blackFill)},
					 		 {"border", STYLEPTR (&border)},
				 	 		 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"dial", 				{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"fgcolors", STYLEPTR (&fgColors)},
					 	 	 {"bgcolors", STYLEPTR (&bgColors)},
					 	 	 {"textcolors", STYLEPTR (&fgColors)},
					 	 	 {"font", STYLEPTR (&mdLabelFont)}}},
		{"dial/focus", 				{{"uses", STYLEPTR (&focusStyles)}}},
		{"slider", 				{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"fgcolors", STYLEPTR (&fgColors)},
					 	 	 {"bgcolors", STYLEPTR (&bgColors)},
					 	 	 {"textcolors", STYLEPTR (&bgColors)},
					 	 	 {"font", STYLEPTR (&mdLabelFont)}}},
		{"tlabel",	 			{{"uses", STYLEPTR (&smlabelStyles)},
					 		 {"font", STYLEPTR (&tLabelFont)}}},
		{"ylabel",	 			{{"uses", STYLEPTR (&smlabelStyles)},
					 		 {"textcolors", STYLEPTR (&fgColors)}}},
		{"ctlabel",	 			{{"uses", STYLEPTR (&labelStyles)}}},
		{"smlabel",	 			{{"uses", STYLEPTR (&smlabelStyles)}}},
		{"lflabel",	 			{{"uses", STYLEPTR (&labelStyles)},
					 		 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu",	 			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"menu/item",	 			{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu/button",	 			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)},
					 	 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"menu/listbox",			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"menu/listbox/item",			{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 		 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu/listbox/button",			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)},
					 	 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"filechooser",	 			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"filechooser/label",			{{"background", STYLEPTR (&menuBg)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/textbox", 		{{"background", STYLEPTR (&menuBg)},
					 		 {"border", STYLEPTR (&boxlabelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/scrollbar",		{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"fgcolors", STYLEPTR (&blkColors)},
				 	 		 {"bgcolors", STYLEPTR (&bgColors)}}},
		{"filechooser/marker",			{{"uses", STYLEPTR (&defaultStyles)},
				 	 		 {"bgcolors", STYLEPTR (&fgColors)}}},
		{"filechooser/checkbox",		{{"uses", STYLEPTR (&defaultStyles)},
				 	 		 {"fgcolors", STYLEPTR (&fgColors)},
					 		 {"bgcolors", STYLEPTR (&bgColors)}}},
		{"filechooser/listbox",			{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"filechooser/listbox/item",		{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/listbox/item/dir",	{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&boldLfLabelFont)}}},
		{"filechooser/listbox/item/file",	{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/listbox/button",		{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)},
					 	 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"filechooser/popup",	 		{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"filechooser/popup/item",	 	{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/popup/button",	 	{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)},
					 	 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"filechooser/popup/listbox",		{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)}}},
		{"filechooser/popup/listbox/item",	{{"uses", STYLEPTR (&defaultStyles)},
					 		 {"border", STYLEPTR (&labelborder)},
					 	 	 {"textcolors", STYLEPTR (&BColors::whites)},
					 	 	 {"font", STYLEPTR (&lfLabelFont)}}},
		{"filechooser/popup/listbox/button",	{{"border", STYLEPTR (&menuBorder)},
					 		 {"background", STYLEPTR (&menuBg)},
					 	 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"pad0",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[0])}}},
		{"pad0/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad1",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[1])}}},
		{"pad1/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad2",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[2])}}},
		{"pad2/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad3",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[3])}}},
		{"pad3/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad4",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[4])}}},
		{"pad4/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad5",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[5])}}},
		{"pad5/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad6",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[6])}}},
		{"pad6/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad7",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[7])}}},
		{"pad7/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad8",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[8])}}},
		{"pad8/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad9",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[9])}}},
		{"pad9/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad10",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[10])}}},
		{"pad10/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad11",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[11])}}},
		{"pad11/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad12",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[12])}}},
		{"pad12/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad13",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[13])}}},
		{"pad13/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad14",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[14])}}},
		{"pad14/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad15",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[15])}}},
		{"pad15/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad16",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[16])}}},
		{"pad16/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad17",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[17])}}},
		{"pad17/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad18",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[18])}}},
		{"pad18/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad19",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[19])}}},
		{"pad19/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad20",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[20])}}},
		{"pad20/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad21",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[21])}}},
		{"pad21/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad22",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[22])}}},
		{"pad22/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad23",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[23])}}},
		{"pad23/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad24",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[24])}}},
		{"pad24/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad25",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[25])}}},
		{"pad25/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad26",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[26])}}},
		{"pad26/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad27",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[27])}}},
		{"pad27/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad28",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[28])}}},
		{"pad28/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"pad29",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[29])}}},
		{"pad29/focus",		{{"uses", STYLEPTR (&focusStyles)}}}
	});
};

#endif /* BOOPSGUI_HPP_ */
