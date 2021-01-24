/* Window.hpp
 * Copyright (C) 2018, 2019  Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BWIDGETS_WINDOW_HPP_
#define BWIDGETS_WINDOW_HPP_

// Default BWidgets::Window settings (Note: use non-transparent backgrounds only)
#define BWIDGETS_DEFAULT_WINDOW_BACKGROUND BStyles::blackFill

#include <chrono>
#include <deque>
#include <list>
#include "Widget.hpp"

namespace BWidgets
{

/**
 * Class BWidgets::Window
 *
 * Main window class of BWidgets. Add all other widgets (directly or
 * indirectly) to this window.
 * A BWidgets::Window is the BWidgets::Widget that is controlled by the host
 * via Pugl, receives host events via Pugl and coordinates handling of all
 * events. Configure, expose, and close events will be handled directly and
 * exclusively by this widget.
 */
class Window : public Widget
{
protected:
	BDevices::DeviceGrabStack<uint32_t> keyGrabStack_;
	BDevices::DeviceGrabStack<BDevices::MouseDevice> buttonGrabStack_;

public:
	Window ();
	Window (const double width, const double height, const std::string& title,
		PuglNativeWindow nativeWindow, bool resizable = false,
		PuglWorldType worldType = PUGL_PROGRAM, int worldFlag = 0);

	Window (const Window& that) = delete;			// Only one window in this version

	~Window ();

	Window& operator= (const Window& that) = delete;	// Only one Window in this version

	virtual Widget* clone () = delete;			// Only one Window in this version

	/**
	 * Gets in contact to the host system via Pugl
	 * @return Pointer to the PuglView
	 */
	PuglView* getPuglView ();

	/**
	 * Gets the Cairo context provided by the host system via Pugl
	 * @return Pointer to the Cairo context
	 */
	cairo_t* getPuglContext ();

	/**
	 * Runs the window until the close flag is set and thus it will be closed.
	 * For stand-alone applications.
	 */
	void run ();

	/**
	 * Queues an event until the next call of the handleEvents method.
	 * @param event Event
	 */
	void addEventToQueue (BEvents::Event* event);

	/**
	 * Main Event handler. Walks through the event queue and sorts the events
	 * to their respective onXXX handling methods
	 */
	void handleEvents ();

	/**
	 * Executes an reexposure of the area given by the expose event.
	 * @param event Expose event containing the widget that emitted the event
	 * 		and the area that should be reexposed.
	 */
	virtual void onExposeRequest (BEvents::ExposeEvent* event) override;

	/**
	 * Predefined empty method to handle a BEvents::EventType::CONFIGURE_EVENT.
	 * @param event Expose event containing the widget that emitted the event
	 * 		and the area that should be reexposed.
	 */
	virtual void onConfigureRequest (BEvents::ExposeEvent* event) override;

	/**
	 * Sets the close flag and thus ends the run method.
	 * @param event Widget event containing the widget that emitted the event
	 */
	virtual void onCloseRequest (BEvents::WidgetEvent* event) override;

	/* Gets (the pointer to) the keyGrabStack and thus enables access to the
	 * keyboard input.
	 * @return	Pointer to keyGrabStack_.
	 */
	BDevices::DeviceGrabStack<uint32_t>* getKeyGrabStack ();

	/* Gets (the pointer to) the buttonGrabStack and thus enables access to
	 * the mouse buttons pressed.
	 * @return	Pointer to buttonGrabStack_.
	 */
	BDevices::DeviceGrabStack<BDevices::MouseDevice>* getButtonGrabStack ();

	/*
	 * Removes events (emited by a given widget) from the event queue
	 * @param widget	Emitting widget (nullptr for all widgets)
	 */
	void purgeEventQueue (Widget* widget = nullptr);

protected:

	/**
	 * Communication interface to the host via Pugl. Translates PuglEvents to
	 * BEvents::Event derived objects.
	 */
	static PuglStatus translatePuglEvent (PuglView* view, const PuglEvent* event);

	void translateTimeEvent ();
	void mergeEvents ();

	void unfocus();

	std::string title_;
	PuglWorld* world_;
	PuglView* view_;
	PuglNativeWindow nativeWindow_;
	bool quit_;
	bool focused_;
	BUtilities::Point pointer_;

	std::deque<BEvents::Event*> eventQueue_;		// TODO: std::list ?
};

}



#endif /* BWIDGETS_WINDOW_HPP_ */
