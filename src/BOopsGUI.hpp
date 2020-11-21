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

#include "BWidgets/Widget.hpp"
#include "BWidgets/Window.hpp"
#include "BWidgets/Label.hpp"
#include "BWidgets/PopupListBox.hpp"
#include "BWidgets/HPianoRoll.hpp"
#include "BWidgets/TextButton.hpp"
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
#include "PadButton.hpp"
#include "IconPadButton.hpp"
#include "PadToggleButton.hpp"
#include "ValueSelect.hpp"
#include "MidiToggleButton.hpp"
#include "CurveChart.hpp"
#include "HSlider.hpp"
#include "Dial.hpp"
#include "Shape.hpp"
#include "MonitorWidget.hpp"

#define BG_FILE "inc/surface.png"
#define HELP_URL "https://github.com/sjaehn/BOops/blob/master/README.md"
#define YT_URL "https://www.youtube.com/watch?v=bGUmZHWqdkE"
#define OPEN_CMD "xdg-open"

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
	MAXEDIT		= 8
};

const std::string editLabels[MAXEDIT] = {"Select & cut", "Select & copy", "Select & X flip", "Select & Y flip", "Paste", "Reset", "Undo", "Redo"};

class BOopsGUI : public BWidgets::Window
{
public:
	BOopsGUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeWindow parentWindow);
	~BOopsGUI ();
	void port_event (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
	void sendUiOn ();
	void sendUiOff ();
	void sendSlot (const int slot);
	void sendPad (const int slot, const int step);
	void sendShape (const int slot);
	void sendTransportGateKeys();
	virtual void onConfigureRequest (BEvents::ExposeEvent* event) override;
	virtual void onKeyPressed (BEvents::KeyEvent* event) override;
	virtual void onKeyReleased (BEvents::KeyEvent* event) override;
	void applyTheme (BStyles::Theme& theme) override;

	static void optionChangedCallback(BEvents::Event* event);

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;

private:
	static void valueChangedCallback(BEvents::Event* event);
	static void playStopBypassChangedCallback(BEvents::Event* event);
	static void effectChangedCallback(BEvents::Event* event);
	static void addClickedCallback(BEvents::Event* event);
	static void delClickedCallback(BEvents::Event* event);
	static void upClickedCallback(BEvents::Event* event);
	static void downClickedCallback(BEvents::Event* event);
	static void menuClickedCallback(BEvents::Event* event);
	static void effectClickedCallback(BEvents::Event* event);
	static void edit1ChangedCallback(BEvents::Event* event);
	static void edit2ChangedCallback(BEvents::Event* event);
	static void padsPressedCallback (BEvents::Event* event);
	static void padsScrolledCallback (BEvents::Event* event);
	static void padsFocusedCallback (BEvents::Event* event);
	static void transportGateButtonClickedCallback (BEvents::Event* event);
	static void helpButtonClickedCallback (BEvents::Event* event);
	static void ytButtonClickedCallback (BEvents::Event* event);
	virtual void resize () override;
	int getSlotsSize () const;
	void clearSlot (int slot);
	void copySlot (int dest, int source);
	void insertSlot (int slot, const BOopsEffectsIndex effect);
	void deleteSlot (int slot);
	void swapSlots (int slot1, int slot2);
	void updateSlot (const int slot);
	void updateSlots ();
	void gotoSlot (const int slot);
	void setOptionWidget (const int slot);
	void loadOptions (const int slot);
	int getPadOrigin (const int slot, const int step) const;
	void setPad (const int slot, const int step, const Pad pad);
	void drawPad ();
	void drawPad (const int slot);
	void drawPad (const int slot, const int step);
	void drawPad (cairo_t* cr, const int slot, const int step);

	std::string pluginPath;
	double sz;
	cairo_surface_t* bgImageSurface;

	BOopsURIDs urids;
	LV2_Atom_Forge forge;

	// Controllers
	std::array<BWidgets::ValueWidget*, NR_CONTROLLERS> controllerWidgets;

	//Pads
	class Pattern
	{
	public:
		void clear ();
		Pad getPad (const size_t row, const size_t step) const;
		void setPad (const size_t row, const size_t step, const Pad& pad);
		std::vector<PadMessage> undo ();
		std::vector<PadMessage> redo ();
		void store ();
	private:
		Journal<std::vector<PadMessage>, MAXUNDO> journal;
		std::array<std::array<Pad, NR_STEPS>, NR_SLOTS> pads;
		struct
		{
			std::vector<PadMessage> oldMessage;
			std::vector<PadMessage> newMessage;
		} changes;
	};

	Pattern pattern;

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
	BWidgets::PopupListBox playModeListBox;
	BWidgets::PopupListBox onMidiListBox;
	BWidgets::HPianoRoll transportGateButton;
	BWidgets::Label autoplayBpmLabel;
	HSlider autoplayBpmSlider;
	BWidgets::Label autoplayBpbLabel;
	HSlider autoplayBpbSlider;
	BWidgets::Label autoplayPositionLabel;
	BWidgets::HSlider autoplayPositionSlider;
	ValueSelect sequenceSizeSelect;
	BWidgets::PopupListBox sequenceBaseListBox;
	BWidgets::PopupListBox stepsListBox;

	BWidgets::Widget transportGateContainer;
	BWidgets::Label transportGateLabel;
	BWidgets::HPianoRoll transportGatePiano;
	BWidgets::TextButton transportGateOkButton;
	BWidgets::TextButton transportGateCancelButton;
	std::vector<bool> transportGateKeys;

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

	MonitorWidget monitor;
	PadSurface padSurface;

	BWidgets::Widget editContainer;
	std::array<HaloToggleButton, EDIT_RESET> edit1Buttons;
	std::array<HaloButton, MAXEDIT - EDIT_RESET> edit2Buttons;

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

	BWidgets::Widget padParamContainer;
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
		{{{0.2, 0.98, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_DELAY
		{{{0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.25, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}},		// FX_REVERSER
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
	}};

	std::array<std::string, NR_FX> fxBgFilenames = {BOOPSFXBGNAMES};

	BStyles::Border border = {{ink, 1.0}, 0.0, 2.0, 0.0};
	BStyles::Border menuBorder = {{BColors::grey, 1.0}, 0.0, 0.0, 0.0};
	BStyles::Border labelborder = {BStyles::noLine, 4.0, 0.0, 0.0};
	BStyles::Border boxlabelborder = {{BColors::grey, 1.0}, 0.0, 3.0, 0.0};
	BStyles::Border focusborder = BStyles::Border (BStyles::Line (BColors::Color (0.0, 0.0, 0.0, 0.5), 2.0));
	BStyles::Border padborder = {BStyles::noLine, 1.0, 0.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill tabBg = BStyles::Fill (BColors::Color (0.75, 0.75, 0.0, 0.5));
	BStyles::Fill activeTabBg = BStyles::Fill (BColors::Color (0.75, 0.75, 0.0, 1.0));
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
					{"font", STYLEPTR (&ctLabelFont)}}};

	BStyles::Theme theme = BStyles::Theme
	({
		defaultStyles,
		{"B.Oops", 		{{"background", STYLEPTR (&BStyles::blackFill)},
					 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"main", 		{{"background", STYLEPTR (&widgetBg)},
					 {"border", STYLEPTR (&BStyles::noBorder)}}},
 		{"monitor", 		{{"uses", STYLEPTR (&defaultStyles)},
 					 {"fgcolors", STYLEPTR (&wvColors)}}},
		{"widget", 		{{"uses", STYLEPTR (&defaultStyles)}}},
		{"widget/focus",	{{"uses", STYLEPTR (&focusStyles)}}},
		{"screen", 		{{"background", STYLEPTR (&screenBg)},
					 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"tab", 		{{"background", STYLEPTR (&tabBg)},
					 {"border", STYLEPTR (&BStyles::noBorder)},
			 		 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"activetab", 		{{"background", STYLEPTR (&activeTabBg)},
					 {"border", STYLEPTR (&BStyles::noBorder)},
			 		 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"symbol", 		{{"uses", STYLEPTR (&defaultStyles)},
					 {"fgcolors", STYLEPTR (&blkColors)}}},
		{"symbol/focus",	{{"uses", STYLEPTR (&focusStyles)}}},
		{"box", 		{{"background", STYLEPTR (&boxBg)},
					{"border", STYLEPTR (&border)}}},
		{"box/focus",		{{"uses", STYLEPTR (&focusStyles)}}},
		{"boxlabel",		{{"background", STYLEPTR (&boxBg)},
					 {"border", STYLEPTR (&boxlabelborder)},
					 {"textcolors", STYLEPTR (&BColors::whites)},
 					 {"font", STYLEPTR (&lfLabelFont)}}},
 		{"smboxlabel",		{{"background", STYLEPTR (&boxBg)},
 					 {"border", STYLEPTR (&boxlabelborder)},
 					 {"textcolors", STYLEPTR (&BColors::whites)},
  					 {"font", STYLEPTR (&smLabelFont)}}},
		{"button", 		{{"background", STYLEPTR (&BStyles::blackFill)},
					 {"border", STYLEPTR (&border)},
				 	 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"dial", 		{{"uses", STYLEPTR (&defaultStyles)},
					 {"fgcolors", STYLEPTR (&fgColors)},
					 {"bgcolors", STYLEPTR (&bgColors)},
					 {"textcolors", STYLEPTR (&fgColors)},
					 {"font", STYLEPTR (&ctLabelFont)}}},
		{"dial/focus", 		{{"uses", STYLEPTR (&focusStyles)}}},
		{"slider", 		{{"uses", STYLEPTR (&defaultStyles)},
					 {"fgcolors", STYLEPTR (&fgColors)},
					 {"bgcolors", STYLEPTR (&bgColors)},
					 {"textcolors", STYLEPTR (&bgColors)},
					 {"font", STYLEPTR (&ctLabelFont)}}},
		{"tlabel",	 	{{"uses", STYLEPTR (&smlabelStyles)},
					 {"font", STYLEPTR (&tLabelFont)}}},
		{"ylabel",	 	{{"uses", STYLEPTR (&smlabelStyles)},
					 {"textcolors", STYLEPTR (&fgColors)}}},
		{"ctlabel",	 	{{"uses", STYLEPTR (&labelStyles)}}},
		{"smlabel",	 	{{"uses", STYLEPTR (&smlabelStyles)}}},
		{"lflabel",	 	{{"uses", STYLEPTR (&labelStyles)},
					 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu",	 	{{"border", STYLEPTR (&menuBorder)},
					 {"background", STYLEPTR (&menuBg)}}},
		{"menu/item",	 	{{"uses", STYLEPTR (&defaultStyles)},
					 {"border", STYLEPTR (&labelborder)},
					 {"textcolors", STYLEPTR (&BColors::whites)},
					 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu/button",	 	{{"border", STYLEPTR (&menuBorder)},
					 {"background", STYLEPTR (&menuBg)},
					 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"menu/listbox",	{{"border", STYLEPTR (&menuBorder)},
					 {"background", STYLEPTR (&menuBg)}}},
		{"menu/listbox/item",	{{"uses", STYLEPTR (&defaultStyles)},
					 {"border", STYLEPTR (&labelborder)},
					 {"textcolors", STYLEPTR (&BColors::whites)},
					 {"font", STYLEPTR (&lfLabelFont)}}},
		{"menu/listbox/button",	{{"border", STYLEPTR (&menuBorder)},
					 {"background", STYLEPTR (&menuBg)},
					 {"bgcolors", STYLEPTR (&buttonBgColors)}}},
		{"pad0",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[0])}}},
		{"pad1",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[1])}}},
		{"pad2",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[2])}}},
		{"pad3",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[3])}}},
		{"pad4",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[4])}}},
		{"pad5",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[5])}}},
		{"pad6",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[6])}}},
		{"pad7",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[7])}}},
		{"pad8",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[8])}}},
		{"pad9",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[9])}}},
		{"pad10",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[10])}}},
		{"pad11",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[11])}}},
		{"pad12",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[12])}}},
		{"pad13",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[13])}}},
		{"pad14",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[14])}}},
		{"pad15",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[15])}}},
		{"pad16",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[16])}}},
		{"pad17",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[17])}}},
		{"pad18",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[18])}}},
		{"pad19",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[19])}}},
		{"pad20",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[20])}}},
		{"pad21",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[21])}}},
		{"pad22",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[22])}}},
		{"pad23",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[23])}}},
		{"pad24",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[24])}}},
		{"pad25",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[25])}}},
		{"pad26",		{{"uses", STYLEPTR (&padStyles)},
					 {"fgcolors", STYLEPTR (&padColors[26])}}}
	});
};

#endif /* BOOPSGUI_HPP_ */