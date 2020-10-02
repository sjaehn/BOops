/* BEvents.hpp
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

#ifndef BEVENTS_HPP_
#define BEVENTS_HPP_

#include <cstdint>
#include <string>
#include "BDevices.hpp"
#include "../BUtilities/Any.hpp"
#include "../BUtilities/RectArea.hpp"

namespace BWidgets
{
class Widget;	// Forward declaration
}

namespace BEvents
{

//TODO switch toward three pointer event handling states: pass, handle, block

/**
 * Enumeration of event types
 */
enum EventType
{
	CONFIGURE_REQUEST_EVENT,
	EXPOSE_REQUEST_EVENT,
	CLOSE_REQUEST_EVENT,
	KEY_PRESS_EVENT,
	KEY_RELEASE_EVENT,
	BUTTON_PRESS_EVENT,
	BUTTON_RELEASE_EVENT,
	BUTTON_CLICK_EVENT,
	POINTER_MOTION_EVENT,
	POINTER_DRAG_EVENT,
	WHEEL_SCROLL_EVENT,
	VALUE_CHANGED_EVENT,
	FOCUS_IN_EVENT,
	FOCUS_OUT_EVENT,
	MESSAGE_EVENT,
	NO_EVENT
};

/**
 * Class BEvents::Event
 *
 * Main class of events. Only contains the event type and (a pointer to) the
 * widget which caused the event. All other event classes are derived from this
 * class.
 */
class Event
{
protected:
	BWidgets::Widget* eventWidget;
	EventType eventType;

public:
	Event () :
		Event (nullptr, NO_EVENT) {}
	Event (BWidgets::Widget* widget, const EventType type) :
		eventWidget (widget), eventType (type) {}

	virtual ~Event () {}

	/**
	 * Gets a pointer to the widget which caused the event.
	 * @return Pointer to the widget
	 */
	BWidgets::Widget* getWidget ()
	{return eventWidget;}

	/**
	 * Gets the type of the event
	 * @return Event type
	 */
	EventType getEventType () const
	{return eventType;}

};
/*
 * End of class BEvents::Event
 *****************************************************************************/


 /**
  * Class BEvents::WidgetEvent
  *
  * Widget events are emitted by an (event) widget if the widget is requested
  * by an other (request) widget. This event class is typically used if opening
  * or closing of a window or an request widget is requested.
  */
 class WidgetEvent : public Event
 {
 protected:
 	BWidgets::Widget* requestWidget;

 public:
 	WidgetEvent () :
 		WidgetEvent (nullptr, nullptr, NO_EVENT) {}
 	WidgetEvent (BWidgets::Widget* eventWidget, BWidgets::Widget* requestWidget, const EventType type) :
 		Event (eventWidget, type),  requestWidget (requestWidget) {}

 	/**
 	 * Gets a pointer to the widget which request the event.
 	 * @return Pointer to the widget
 	 */
 	BWidgets::Widget* getRequestWidget ()
 	{return requestWidget;}

 };
 /*
  * End of class BEvents::WidgetEvent
  *****************************************************************************/


/**
 * Class BEvents::ExposeEvent
 *
 * Expose events are emitted by a parent event widget (or window) if the visual
 * output of a child (request) widget is requested to be
 * updated. An expose event additionally contains the coordinates (x, y, width
 * and height) of the output region (relative to the widgets origin) that should
 * be updated.
 */
class ExposeEvent : public WidgetEvent
{
protected:
	BUtilities::RectArea exposeArea;

public:
	ExposeEvent () :
		ExposeEvent (nullptr, nullptr, NO_EVENT, 0, 0, 0, 0) {};
	ExposeEvent (BWidgets::Widget* eventWidget, BWidgets::Widget* requestWidget, const EventType type,
		     const double x, const double y, const double width, const double height) :
		ExposeEvent (eventWidget, requestWidget, type, BUtilities::RectArea (x, y, width, height)) {}
	ExposeEvent (BWidgets::Widget* eventWidget, BWidgets::Widget* requestWidget, const EventType type,
		     const BUtilities::RectArea& area) :
		WidgetEvent (eventWidget, requestWidget, type),
		exposeArea (area) {}

	/**
	 * Redefines the area coordinates of the output region for the expose
	 * event
	 * @param area Area coordinates relative to the widgets origin
	 */
	void setArea (const BUtilities::RectArea& area)
	{exposeArea = area;}

	/**
	 * Gets the area coordinates of the output region for the expose event
	 * @return Area coordinates relative to the widgets origin
	 */
	BUtilities::RectArea getArea () const
	{return exposeArea;}
};
/*
 * End of class BEvents::ExposeEvent
 *****************************************************************************/

/**
 * Class BEvents::KeyEvent
 *
 * Key events are emitted by the system if a key is pressed or released.
 */
 class KeyEvent : public Event
 {
 protected:
	BUtilities::Point point;
	uint32_t key;

 public:
	KeyEvent () :
	 	KeyEvent (nullptr, NO_EVENT, 0, 0, 0) {}
	KeyEvent (BWidgets::Widget* widget, const EventType type, const double x, const double y, const uint32_t unicode) :
		KeyEvent (widget, type, BUtilities::Point (x, y), unicode) {}
	KeyEvent (BWidgets::Widget* widget, const EventType type, const BUtilities::Point& position, const uint32_t unicode) :
		Event (widget, type),
		point (position),
		key (unicode) {}

	 /**
	  * Redefines the point coordinates of the key event
	  * @param coords Point coordinates relative to the widgets origin
	  */
	 void setPosition (const BUtilities::Point& coords)
	 {point = coords;}

	 /**
	  * Gets the point coordinates of the key event
	  * @return coords Point coordinates relative to the widgets origin
	  */
	 BUtilities::Point getPosition () const
	 {return point;}

	 /**
	  * Gets the key that caused of the key event
	  * @return Unicode of the key
	  */
	 uint32_t getKey () const
	 {return key;}

	 std::string getKeyUTF8 () const
	 {
		 // Invalide unicode
		 if (key > 0x0010ffff) return "";

		 std::string s = "";

		 // 7 bit ASCII: utf-8 = unicode
		 if (key < 0x80) s += char (key);

		 // 2/3/4(/5/6) byte utf-8
		 else
		 {
			 uint32_t steps = 2;
			 for (uint32_t i = 3; i <= 6; ++i)
			 {
				 if (key >= (uint32_t (2) << (5 * (i - 1)))) steps = i;
			 }

			 char c = char ((0xFF & (0xFF << (8 - steps))) | (key >> (6 * (steps - 1))));
			 s += c;

			 for (uint32_t i = steps - 1; i >= 1; --i)
			 {
				 char c = char (0x80 | ((key >> (6 * (i - 1))) & 0x3f));
				 s += c;
			 }
		 }

		 return s;
	 }
 };
 /*
  * End of class BEvents::KeyEvent
  *****************************************************************************/

/**
 * Class BEvents::PointerEvent
 *
 * Pointer events are emitted by the system (via pugl and the main window) if
 * buttons are pressed or released and/or the pointer is moved over a widget.
 * The pointer event contains data about the position (relative to the
 * respective widget and the button pressed (or not).
 * Pointer events will be handled by the respective widget and can be
 * redirected to external callback functions.
 */
class PointerEvent : public Event
{
protected:
	BUtilities::Point point, origin, delta;
	BDevices::ButtonCode buttonNr;


public:
	PointerEvent () :
		PointerEvent (nullptr, NO_EVENT, 0, 0, 0, 0, 0, 0, BDevices::NO_BUTTON) {}
	PointerEvent (BWidgets::Widget* widget, const EventType type,
			const double x, const double y, const double xOrigin, const double yOrigin,
			const double deltaX, const double deltaY, const BDevices::ButtonCode button) :
		PointerEvent (widget, type, BUtilities::Point (x, y), BUtilities::Point (xOrigin, yOrigin), BUtilities::Point (deltaX, deltaY), button) {}
	PointerEvent (BWidgets::Widget* widget, const EventType type, const BUtilities::Point& point,
			const BUtilities::Point& origin, const BUtilities::Point& delta,
			const BDevices::ButtonCode button) :
		Event (widget, type), point (point), origin (origin), delta (delta), buttonNr (button) {}

	/**
	 * Redefines the point coordinate of the pointer event
	 * @param pont Point coordinate relative to the widgets origin
	 */
	void setPosition (const BUtilities::Point& coords)
 	{point = coords;}

	/**
	 * Gets the point coordinate of the pointer event
	 * @return Point coordinate relative to the widgets origin
	 */
	BUtilities::Point getPosition () const
	{return point;}

	/**
	 * Redefines the point coordinate of the position where the button was
	 * initially pressed
	 * @param origin Point coordinate relative to the widgets origin
	 */
	void setOrigin (const BUtilities::Point& coords)
	{origin = coords;}

	/**
	 * Gets the point coordinate of the pointer position where the respective
	 * button was initially pressed. The returned value is the same as
	 * for getPoint() for BUTTON_PRESS_EVENTs, 0.0 for POINTER_MOTION_EVENTs.
	 * @return Point coordinate relative to the widgets origin
	 */
	BUtilities::Point getOrigin () const
	{return origin;}

	/**
	 * Redefines the pointers movement
	 * @param delta Movement of the pointer
	 */
	void setDelta (const BUtilities::Point& coords)
	{delta = coords;}

	/**
	 * Gets the movement (relative to the last PointerEvent)
	 * @return Change in coordinates
	 */
	BUtilities::Point getDelta () const
	{return delta;}

	/**
	 * Redefines the button pressed of the pointer event
	 * @param button Button pressed
	 */
	void setButton (const BDevices::ButtonCode button)
	{buttonNr = button;}

	/**
	 * Gets the button pressed of the pointer event
	 * @return Button pressed
	 */
	BDevices::ButtonCode getButton () const
	{return buttonNr;}
};
/*
 * End of class BEvents::PointerEvent
 *****************************************************************************/

/**
 * Class BEvents::WheelEvent
 *
 * Wheel events are emitted by the system (via pugl and the main window) if
 * a (mouse) wheel is turned.
 * The wheel event contains data about the relative change of the wheel and
 * about the pointer position (relative to the respective widget.
 * Wheel events will be handled by the respective widget and can be
 * redirected to external callback functions.
 */
class WheelEvent : public Event
{
protected:
	BUtilities::Point point;
	BUtilities::Point delta;

public:
	WheelEvent () :
		WheelEvent (nullptr, NO_EVENT, 0, 0, 0, 0) {}
	WheelEvent (BWidgets::Widget* widget, const EventType type, const double x, const double y, const double deltaX, const double deltaY) :
		WheelEvent (widget, type, BUtilities::Point (x, y), BUtilities::Point (deltaX, deltaY)) {}

	WheelEvent (BWidgets::Widget* widget, const EventType type, const BUtilities::Point& point, const BUtilities::Point delta) :
		Event (widget, type), point (point), delta (delta) {}

	/**
	 * Redefines the pointers coordinate
	 * @param x Point coordinate relative to the widgets origin
	 */
	void setPosition (const BUtilities::Point& coords)
  	{point = coords;}

	/**
	 * Gets the pointers coordinate of the wheel event
	 * @return Point coordinate relative to the widgets origin
	 */
	BUtilities::Point getPosition () const
 	{return point;}

	/**
	 * Redefines the wheels  movement
	 * @param delta Movement of the wheel
	 */
	void setDelta (const BUtilities::Point& coords)
 	{delta = coords;}

	/**
	 * Gets the xmovement of the wheel
	 * @return Change in coordinate
	 */
	BUtilities::Point getDelta () const
 	{return delta;}
};
/*
 * End of class BEvents::WheelEvent
 *****************************************************************************/


/**
 * Class BEvents::ValueChangedEvent
 *
 * Value changed events are emitted by widgets (namely BWidgets::ValueWidget)
 * if their setValue method is called. The event additionally exposes the
 * changed value (that should also be accessible via
 * BWidgets::ValueWidget::getValue ()). Value changed events can be handled
 * internally (e.g., by composite widgets) and can also be redirected to
 * external callback functions.
 */
class ValueChangedEvent : public Event
{
protected:
	double value;

public:
	ValueChangedEvent () :
		ValueChangedEvent (nullptr, 0.0) {}
	ValueChangedEvent (BWidgets::Widget* widget, const double val) :
		Event (widget, VALUE_CHANGED_EVENT), value (val) {}

	/**
	 * Redefines the value exposed by the event. This method doesn't change the
	 * value within a widget!
	 * @param val New value
	 */
	void setValue (const double val)
	{value = val;}

	/**
	 * Gets the value exposed by the event
	 * @return Value of the event
	 */
	double getValue () const
	{return value;}
};
/*
 * End of class BEvents::ValueChangedEvent
 *****************************************************************************/

/**
 * Class BEvents::FocusEvent
 *
 * Focus events are emitted by widgets if the pointer rests for a predefined
 * time over the widget
 */
class FocusEvent : public Event
{
protected:
	BUtilities::Point point;

public:
	FocusEvent () :
		FocusEvent (nullptr, NO_EVENT, 0, 0) {}
	FocusEvent (BWidgets::Widget* widget, const EventType type, const double x, const double y) :
		FocusEvent (widget, type, BUtilities::Point (x, y)) {}
	FocusEvent (BWidgets::Widget* widget, const EventType type, const BUtilities::Point& point) :
		Event (widget, type), point (point) {}

	/**
	 * Redefines the pointers coordinate
	 * @param x Point coordinate relative to the widgets origin
	 */
	void setPosition (const BUtilities::Point& coords)
  	{point = coords;}

	/**
	 * Gets the pointers coordinate of the wheel event
	 * @return Point coordinate relative to the widgets origin
	 */
	BUtilities::Point getPosition () const
 	{return point;}
};
/*
 * End of class BEvents::ValueChangedEvent
 *****************************************************************************/

 /**
  * Class BEvents::MessageEvent
  *
  * Ubiquitous event type
  */
class MessageEvent : public Event
{
protected:
	std::string messageName;
	BUtilities::Any messageContent;

public:
	MessageEvent () :
		MessageEvent (nullptr, "", BUtilities::Any ()) {}
	MessageEvent (BWidgets::Widget* widget, const std::string& name, const BUtilities::Any& content) :
		Event (widget, MESSAGE_EVENT), messageName (name), messageContent (content) {}

	void setName (const std::string& name)
	{messageName = name;}

	std::string getName () const
	{return messageName;}

	void setContent (const BUtilities::Any& content)
	{messageContent = content;}

	BUtilities::Any getContent () const
	{return messageContent;}
};

}

#endif /* BEVENTS_HPP_ */
