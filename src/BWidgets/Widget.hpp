/* Widget.hpp
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

#ifndef BWIDGETS_WIDGET_HPP_
#define BWIDGETS_WIDGET_HPP_

// Default basic widget settings
#define BWIDGETS_DEFAULT_BORDER BStyles::noBorder
#define BWIDGETS_DEFAULT_BACKGROUND BStyles::noFill
#define BWIDGETS_DEFAULT_WIDTH 200
#define BWIDGETS_DEFAULT_HEIGHT 200
#define BWIDGETS_DEFAULT_STATE BColors::NORMAL
#define BWIDGETS_DEFAULT_FGCOLORS BColors::greens
#define BWIDGETS_DEFAULT_BGCOLORS BColors::darks
#define BWIDGETS_DEFAULT_FONT BStyles::sans12pt
#define BWIDGETS_DEFAULT_ILLUMINATED 0.333
#define BWIDGETS_DEFAULT_NORMALLIGHTED 0.0
#define BWIDGETS_DEFAULT_SHADOWED -0.333
#define BWIDGETS_DEFAULT_DARKENED -0.5

// Default settings for all text containing widgets
#define BWIDGETS_DEFAULT_TEXT_COLORS BColors::lights
#define BWIDGETS_DEFAULT_TEXT_ALIGN BStyles::TEXT_ALIGN_LEFT
#define BWIDGETS_DEFAULT_TEXT_VALIGN BStyles::TEXT_VALIGN_TOP

// Default settings for all menu widgets
#define BWIDGETS_DEFAULT_MENU_PADDING 10
#define BWIDGETS_DEFAULT_MENU_BORDER BStyles::greyBorder1pt
#define BWIDGETS_DEFAULT_MENU_TEXTBORDER {{BColors::invisible, 4.0}, 0.0, 0.0, 0.0}
#define BWIDGETS_DEFAULT_MENU_BACKGROUND BStyles::grey20Fill

// BWidgets theme keywords
#define BWIDGETS_KEYWORD_BORDER "border"
#define BWIDGETS_KEYWORD_BACKGROUND "background"
#define BWIDGETS_KEYWORD_FONT "font"
#define BWIDGETS_KEYWORD_FGCOLORS "fgcolors"
#define BWIDGETS_KEYWORD_BGCOLORS "bgcolors"
#define BWIDGETS_KEYWORD_TEXTCOLORS "textcolors"


#include <cairo/cairo.h>
#include "cairoplus.h"
#include "pugl/pugl/pugl.h"
#include "pugl/pugl/cairo.h"
#include <stdint.h>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <chrono>

#include "../BUtilities/RectArea.hpp"
#include "BColors.hpp"
#include "BStyles.hpp"
#include "BEvents.hpp"

namespace BWidgets
{

enum WidgetStacking
{
	STACKING_NORMAL,
	STACKING_CATCH,
	//STACKING_FIT,
	//STACKING_RESIZE_TO_FIT,
	//STACKING_RESIZE_PARENT_TO_FIT,
	STACKING_OVERSIZE
};

class Window; // Forward declaration

/**
 * Class BWidgets::Widget
 *
 * Root widget class of BWidgets. All other widget (including BWidgets::Window)
 * are derived from this class.
 * A BWidgets::Widget (and all derived widgets) are drawable and may have a
 * border and a background. A BWidgets::Widget (and all derived widgets) may
 * also be containers for other widgets (= have children).
 */

class Widget
{
public:
	Widget ();
	Widget (const double x, const double y, const double width, const double height);
	Widget (const double x, const double y, const double width, const double height, const std::string& name);

	/**
	 * Creates a new (orphan) widget and copies the widget properties from a
	 * source widget. This method doesn't copy any parent or child widgets.
	 * @param that Source widget
	 */
	Widget (const Widget& that);

	virtual ~Widget ();

	/**
	 * Assignment. Copies the widget properties from a source widget and keeps
	 * its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param that Source widget
	 */
	Widget& operator= (const Widget& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const;

	/**
	 * Makes the widget visible (if its parents are visible too) and emits an
	 * BEvents::ExposeEvent to the main window.
	 */
	void show ();

	/**
	 * Makes the widget invisible and emits an
	 * BEvents::ExposeEvent to the main window.
	 */
	void hide ();

	/**
	 * Adds a child widget to the widget (container function). A
	 * BEvents::ExposeEvent will be emitted if the child widget will be
	 * unveiled due to the addition.
	 * @param child Child widget
	 */
	void add (Widget& child);

	/**
	 * Releases (but not deletes) a child widget from the container of this
	 * widget. A BEvents::ExposeEvent will be emitted if a visible child is
	 * released.
	 * @param child Pointer to the child widget
	 */
	void release (Widget* child);

	/**
	 * Moves the widget and emits a BEvents::ExposeEvent if the widget is
	 * visible.
	 * @param x 		New x coordinate
	 * @param y 		New y coordinate
	 * @param position	New position
	 */
	virtual void moveTo (const double x, const double y);
	virtual void moveTo (const BUtilities::Point& position);

	/**
	 * Gets the widgets position
	 * @return Position
	 */
	BUtilities::Point getPosition () const;

	/**
	 * Gets the widgets position relative to the position of its main window.
	 * @return Position
	 */
	BUtilities::Point getAbsolutePosition () const;

	/**
	 * Pushes this widget one step backwards if it is linked to a
	 * parent widget. Emits a BEvents::ExposeEvent if the widget is visible.
	 */
	void pushBackwards ();

	/**
	 * Raises this widget one step frontwards if it is linked to a
	 * parent widget. Emits a BEvents::ExposeEvent if the widget is visible.
	 */
	void raiseFrontwards ();

	/**
	 * Pushes this widget to the bottom if it is linked to a
	 * parent widget. Emits a BEvents::ExposeEvent if the widget is visible.
	 */
	void pushToBottom ();

	/**
	 * Raises this widget to the front if it is linked to a
	 * parent widget. Emits a BEvents::ExposeEvent if the widget is visible.
	 */
	void raiseToTop ();

	/**
	 * Pushs this widget to the bottom if it is linked to a
	 * parent widget. Emits a BEvents::ExposeEvent if the widget is visible.
	 */
	// TODO void pushToBottom ();

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible.
	 * @param width New widgets width
	 */
	virtual void setWidth (const double width);

	/**
	 * Gets the width of the widget
	 * @return Width
	 */
	double getWidth () const;

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible.
	 * @param height New widgets height
	 */
	virtual void setHeight (const double height);

	/**
	 * Gets the height of the widget
	 * @return Height
	 */
	double getHeight () const;

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible. If no parameters are given, the widget will be
	 * resized to the size of the containing child widgets.
	 * @param width		New widgets width
	 * @param height	New widgets height
	 * @param extends	New widget extends
	 */
	virtual void resize ();
	virtual void resize (const double width, const double height);
	virtual void resize (const BUtilities::Point extends);

	/**
	 * Gets the width and the height of the widget
	 * @return Point containing width and height
	 */
	BUtilities::Point getExtends () const;

	/**
	 * Gets the x offset of the widget content. This is distance between the
	 * outer border and the widget content. It is also the sum of margin,
	 * border, and padding.
	 * @return X offset of the widget
	 */
	double getXOffset ();

	/**
	 * Gets the y offset of the widget content. This is distance between the
	 * outer border and the widget content. It is also the sum of margin,
	 * border, and padding.
	 * @return Y offset of the widget
	 */
	double getYOffset ();

	/**
	 * Gets the effective width of the widget content without its borders.
	 * @return Effective width of the widget
	 */
	double getEffectiveWidth ();

	/**
	 * Gets the effective height of the widget content without its borders.
	 * @return Effective height of the widget
	 */
	double getEffectiveHeight ();

	/**
	 * Gets the widgets area without its borders
	 * @return	Effective widgets area
	 */
	BUtilities::RectArea getEffectiveArea ();

	/**
	 * Sets the widgets state
	 * @param state Widget state
	 */
	void setState (const BColors::State state);

	/**
	 * Gets the widgets state
	 * @return Widget state
	 */
	BColors::State getState () const;

	/**
	 * Sets the filter for display and event handling.
	 * @param ()		All widget states
	 * @param state		Widget state
	 * @param states	Vector of widget states
	 */
	void setStateFilter ();
	void setStateFilter (const BColors::State state);
	void setStateFilter (const std::vector<BColors::State>& states);

	/**
	 * Clears the filter for display and event handling.
	 * @param ()		All widget states
	 * @param state		Widget state
	 * @param states	Vector of widget states
	 */
	void clearStateFilter ();
	void clearStateFilter (const BColors::State state);
	void clearStateFilter (const std::vector<BColors::State>& states);

	/**
	 * Gets all set filters for display and event handling
	 */
	std::vector<BColors::State> getStateFilter () const;

	/**
	 * (Re-)Defines the border of the widget. Redraws widget and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param border New widgets border
	 */
	virtual void setBorder (const BStyles::Border& border);

	/**
	 * Gets (a pointer to) the border of the widget.
	 * @return Pointer to BStyles::Border
	 */
	BStyles::Border* getBorder ();

	/**
	 * (Re-)Defines the background of the widget. Redraws widget and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param background New widgets background
	 */
	void setBackground (const BStyles::Fill& background);

	/**
	 * Gets (a pointer to) the background of the widget.
	 * @return Pointer to BStyles::Fill
	 */
	BStyles::Fill* getBackground ();

	/**
	 * Gets a pointer to the widgets main window.
	 * @return Pointer to the main window. Returns nullptr if the widget
	 * isn't connected to a main window.
	 */
	Window* getMainWindow () const;

	/**
	 * Gets a pointer to the widgets parent widget.
	 * @return Pointer to the parent widget. Returns nullptr if the widget
	 * doesn't have any parents.
	 */
	Widget* getParent () const;

	/**
	 * Tests whether the widget has children or not.
	 * @return TRUE if the widget has children, otherwise FALSE
	 */
	bool hasChildren () const;

	/**
	 * Tests whether child is on of children of the widget.
	 * @param child		Pointer to the child widget.
	 * @return 				TRUE if child is one of the children ofthe widget, otherwise
	 *								FALSE.
	 */
	bool isChild (Widget* child);

	/**
	 * Gets the widgets children vector. The vector contains all children of
	 * the widgets from background to foreground.
	 * @return Children vector.
	 */
	std::vector<Widget*> getChildren () const;

	/**
	 * Renames the widget.
	 * @param name New name
	 */
	void rename (const std::string& name);

	/**
	 * Gets the name of the widget
	 * @return Name of the widget
	 */
	std::string getName () const;

	/**
	 * Gets the visibility of the widget. Therefore, all its parents will be
	 * checked for visibility too.
	 * @return TRUE if the widget (and all its parents) are visible, otherwise
	 * 		   FALSE.
	 */
	bool isVisible ();

	/**
	 * Defines whether the widget may emit BEvents::BUTTON_PRESS_EVENT's
	 * following a host button event.
	 * @param status TRUE if widget is clickable, otherwise false
	 */
	void setClickable (const bool status);

	/**
	 * Gets whether the widget may emit BEvents::BUTTON_PRESS_EVENT's or
	 * BEvents::BUTTON_RELEASE_Event's following a host button event.
	 * @return TRUE if widget is clickable, otherwise false
	 */
	bool isClickable () const;

	/**
	 * Defines whether the widget may emit
	 * BEvents::POINER_DRAG_EVENT's following a host pointer event.
	 * By default, "draggable" widgets can be dragged over the window as
	 * result of calling the default dragAndDropCallback method from the
	 * onPointerDragged method. This behavior can be changed by overriding the
	 * onPointerDragged method or by setting a callback function different from
	 * dragAndDropCallback.
	 * @param status TRUE if widget is draggable, otherwise false
	 */
	void setDraggable (const bool status);

	/**
	 * Gets whether the widget may emit BEvents::POINTER_DRAG_EVENT's following
	 * a host pointer event.
	 * @return TRUE if widget is draggable, otherwise false
	 */
	bool isDraggable () const;

	/**
	 * Defines whether the widget may emit
	 * BEvents::WHEEL_SCROLL_EVENT's following a host (mouse) wheel event.
	 * @param status TRUE if widget is scrollable, otherwise false
	 */
	void setScrollable (const bool status);

	/**
	 * Gets whether the widget may emit BEvents::WHEEL_SCROLL_EVENT's following
	 * a host (mouse) wheel event.
	 * @return TRUE if widget is scrollable, otherwise false
	 */
	bool isScrollable () const;

	/**
	 * Defines whether the widget may emit BEvents::FOCUS_EVENT's if the
	 * pointer rests for a predefined time over the widget.
	 * @param status TRUE if widget is focusable, otherwise false
	 */
	void setFocusable (const bool status);

	/**
	 * Gets whether the widget may emit BEvents::FOCUS_EVENT's if the
	 * pointer rests for a predefined time over the widget.
	 * @return TRUE if widget is focusable, otherwise false
	 */
	bool isFocusable () const;

	/**
	 * Defines whether events emitted by this widget MAY be merged to precursor
	 * events of the same type (and (optional) the same widget and (optional)
	 * the same position, depending on the event type) or not.
	 * This flag is ignored if merging doesn't make sense (e.g., in case of
	 * BEvents::CLOSE_EVENT).
	 * @param eventType	BEvents::EventType for which the status is defined
	 * @param status 	TRUE if the the events emitted by this widget and
	 * 			specified by eventType may be merged, otherwise FALSE
	 */
	void setMergeable (const BEvents::EventType eventType, const bool status);

	/**
	 * Gets whether events emitted by this widget MAY be merged to precursor
	 * events of the same type or not.
	 * @return	TRUE if the the events emitted by this widget and specified by
	 * 		eventType may be merged, otherwise FALSE
	 */
	bool isMergeable (const BEvents::EventType eventType) const;

	void setStacking (const WidgetStacking stacking);

	WidgetStacking getStacking () const;

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update ();

	/**
	 * Requests a redisplay of the widgets area (and all underlying widget
	 * areas) by emitting a BEvents::ExposeEvent.
	 */
	void postRedisplay ();

	/**
	 * Requests close of this widget by emitting a BEvents::WidgetEvent
	 * @param handle	Widget that will handle the close request event
	 			(default = main_)
	 */
	void postCloseRequest ();
	void postCloseRequest (Widget* handle);

	/**
	 * (Re-)Defines the callback function for an event. It is on the onXXX
	 * methods whether a callback function will be called or not. By default,
	 * the callback is set to defaultCallback.
	 * @param eventType Enumeration of the event type.
	 * @param callbackFunction Function that should be called if an onXXX
	 * 						   method is called as response of an event and
	 * 						   onXXX allows callbacks.
	 */
	void setCallbackFunction (const BEvents::EventType eventType, const std::function<void (BEvents::Event*)>& callbackFunction);

	/**
	 * Predefined empty callback function
	 * @param event Placeholder, will not be interpreted by this method.
	 */
	static void defaultCallback (BEvents::Event* event);

	/**
	 * Predefined callback function, moves the Widget that emited the
	 * event.
	 * @param event Placeholder, will not be interpreted by this method.
	 */
	static void dragAndDropCallback (BEvents::Event* event);

	static void focusInCallback (BEvents::Event* event);

	static void focusOutCallback (BEvents::Event* event);

	/**
	 * Predefined empty method to handle a BEvents::EventType::CONFIGURE_EVENT.
	 * @param event Expose event
	 */
	virtual void onConfigureRequest (BEvents::ExposeEvent* event);

	/**
	 * Predefined empty method to handle a BEvents::EventType::EXPOSE_EVENT.
	 * @param event Expose event
	 */
	virtual void onExposeRequest (BEvents::ExposeEvent* event);

	/**
	 * Predefined method to handle a BEvents::EventType::CLOSE_EVENT.
	 * Releases the request widget.
	 * @param event Widget event
	 */
	virtual void onCloseRequest (BEvents::WidgetEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::KEY_PRESS_EVENT.
	 * @param event Key event
	 */
	virtual void onKeyPressed (BEvents::KeyEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::KEY_RELEASE_EVENT.
	 * @param event Key event
	 */
	virtual void onKeyReleased (BEvents::KeyEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::BUTTON_PRESS_EVENT.
	 * @param event Pointer event
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::BUTTON_RELEASE_EVENT.
	 * @param event Pointer event
	 */
	virtual void onButtonReleased (BEvents::PointerEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::BUTTON_CLICK_EVENT. This event is emitted i a button
	 * is pressed and released over the same widget.
	 * @param event Pointer event
	 */
	virtual void onButtonClicked (BEvents::PointerEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::POINTER_MOTION_EVENT.
	 * @param event Pointer event
	 */
	virtual void onPointerMotion (BEvents::PointerEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::POINTER_DRAG_EVENT.
	 * @param event Pointer event
	 */
	virtual void onPointerDragged (BEvents::PointerEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::WHEEL_SCROLL_EVENT.
	 * @param event Value changed event
	 */
	virtual void onWheelScrolled (BEvents::WheelEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::VALUE_CHANGED_EVENT.
	 * @param event Value changed event
	 */
	virtual void onValueChanged (BEvents::ValueChangedEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::FOCUS_IN_EVENT.
	 * @param event Focus event
	 */
	virtual void onFocusIn (BEvents::FocusEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::FOCUS_OUT_EVENT.
	 * @param event Focus event
	 */
	virtual void onFocusOut (BEvents::FocusEvent* event);

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::MESSAGE_EVENT.
	 * @param event Event
	 */
	virtual void onMessage (BEvents::MessageEvent* event);

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned
	 */
	virtual void applyTheme (BStyles::Theme& theme);

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name);

protected:

	BUtilities::RectArea getArea () const;
	BUtilities::RectArea getAbsoluteArea () const;
	BUtilities::RectArea getTotalArea (std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});
	BUtilities::RectArea getAbsoluteTotalArea (std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});

	void forEachChild (std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});
	void forEachChild (std::vector<Widget*>::iterator first, std::vector<Widget*>::iterator last,
			   std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});


	Widget* getWidgetAt (const BUtilities::Point& position, std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});

	void stackingCatch ();

	void postMessage (const std::string& name, const BUtilities::Any content);

	void postRedisplay (const BUtilities::RectArea& area);

	void redisplay (cairo_surface_t* surface, const BUtilities::RectArea& area);

	virtual bool filter (Widget* widget);

	virtual void draw (const BUtilities::RectArea& area);

	BUtilities::RectArea area_;
	bool visible_;
	bool clickable_;
	bool draggable_;
	bool scrollable_;
	bool focusable_;
	bool scheduleDraw_;
	std::array<bool, BEvents::EventType::NO_EVENT> mergeable_;
	WidgetStacking stacking_;
	Window* main_;
	Widget* parent_;
	std::vector <Widget*> children_;
	BStyles::Border border_;
	BStyles::Fill background_;
	std::string name_;
	std::array<std::function<void (BEvents::Event*)>, BEvents::EventType::NO_EVENT> cbfunction_;
	cairo_surface_t* widgetSurface_;
	BColors::State widgetState_;

private:
	void redisplay (cairo_surface_t* surface, const BUtilities::RectArea& outerArea, const BUtilities::RectArea& area);
	Widget* getWidgetAt (const BUtilities::Point& abspos, const BUtilities::RectArea& outerArea,
			     const BUtilities::RectArea& area, std::function<bool (Widget* widget)> func = [] (Widget* widget) {return true;});
};

bool isVisible (Widget* widget);

bool isClickable (Widget* widget);

bool isDraggable (Widget* widget);

bool isScrollable (Widget* widget);

bool isFocusable (Widget* widget);

}

#endif /* BWIDGETS_WIDGET_HPP_ */
