/* Window.cpp
 * Copyright (C) 2018, 2019 by Sven Jähnichen
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

#ifdef PKG_HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif /*PKG_HAVE_FONTCONFIG*/

#include "Window.hpp"
#include "Focusable.hpp"

namespace BWidgets
{

Window::Window () : Window (BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, "window", 0) {}

Window::Window (const double width, const double height, const std::string& title, PuglNativeView nativeWindow, bool resizable,
		PuglWorldType worldType, int worldFlag) :
		Widget (0.0, 0.0, width, height, title),
		keyGrabStack_ (), buttonGrabStack_ (),
		title_ (title), world_ (NULL), view_ (NULL), nativeWindow_ (nativeWindow),
		quit_ (false), focused_ (false), pointer_ (),
		eventQueue_ ()
{
	main_ = this;

	world_ = puglNewWorld (worldType, worldFlag);
	puglSetClassName (world_, "BWidgets");

	view_ = puglNewView (world_);
	if (nativeWindow_ != 0) puglSetParentWindow(view_, nativeWindow_);
	puglSetWindowTitle(view_, title.c_str());
	puglSetDefaultSize (view_, getWidth (), getHeight ());
	puglSetViewHint(view_, PUGL_RESIZABLE, resizable ? PUGL_TRUE : PUGL_FALSE);
	puglSetViewHint(view_, PUGL_IGNORE_KEY_REPEAT, PUGL_TRUE);
	puglSetWorldHandle(world_, this);
	puglSetHandle (view_, this);
	puglSetBackend(view_, puglCairoBackend());
	puglSetEventFunc (view_, Window::translatePuglEvent);
	puglRealize (view_);
	puglShow (view_);

	background_ = (BWIDGETS_DEFAULT_WINDOW_BACKGROUND);
	postRedisplay();
}

Window::~Window ()
{
	hide();
	while (!children_.empty ())
	{
		Widget* w = children_.front ();
		if (w) release (w);
	}
	purgeEventQueue ();
	keyGrabStack_.clear ();
	buttonGrabStack_.clear ();
	puglFreeView (view_);
	puglFreeWorld (world_);
	main_ = nullptr;	// Important switch for the super destructor. It took
				// days of debugging ...

	// Cleanup debug information for memory checkers
	//cairo_debug_reset_static_data();
#ifdef PKG_HAVE_FONTCONFIG
	//FcFini();
#endif /*PKG_HAVE_FONTCONFIG*/
}

PuglView* Window::getPuglView () {return view_;}

cairo_t* Window::getPuglContext ()
{
	if (view_) return (cairo_t*) puglGetContext (view_);
	else return NULL;
}

void Window::run ()
{
	while (!quit_) handleEvents();
}

void Window::onConfigureRequest (BEvents::ExposeEvent* event)
{
	if (getExtends () != event->getArea().getExtends ()) Widget::resize (event->getArea().getExtends ());
}

void Window::onCloseRequest (BEvents::WidgetEvent* event)
{
	if ((event) && (event->getRequestWidget () == this)) quit_ = true;
	else Widget::onCloseRequest (event);
}

void Window::onExposeRequest (BEvents::ExposeEvent* event)
{
	if (event)
	{
		BEvents::ExposeEvent* ee = (BEvents::ExposeEvent*)event;
		puglPostRedisplayRect (view_, {ee->getArea().getX(), ee->getArea().getY(), ee->getArea().getWidth(), ee->getArea().getHeight()});
	}
}

void Window::addEventToQueue (BEvents::Event* event)
{
	// Try to merge with precursor event
	if
	(
		(event) &&
		(event->getWidget()) &&
		(!eventQueue_.empty ()) &&
		(eventQueue_.back())
	)
	{
		BEvents::EventType eventType = event->getEventType();

		if
		(
			(event->getWidget ()->isMergeable(eventType)) &&
			(
				(eventType == BEvents::CONFIGURE_REQUEST_EVENT) ||
				(eventType == BEvents::EXPOSE_REQUEST_EVENT) ||
				(eventType == BEvents::POINTER_MOTION_EVENT) ||
				(eventType == BEvents::POINTER_DRAG_EVENT) ||
				(eventType == BEvents::WHEEL_SCROLL_EVENT) ||
				(eventType == BEvents::VALUE_CHANGED_EVENT)
			)
		)
		{
			// Check for mergeable precursor events
			for (std::deque<BEvents::Event*>::reverse_iterator rit = eventQueue_.rbegin(); rit != eventQueue_.rend(); ++rit)
			{
				BEvents::Event* precursor = *rit;

				if ((precursor->getEventType() == eventType) && (event->getWidget () == precursor->getWidget ()))
				{
					// CONFIGURE_EVENT
					if (eventType == BEvents::CONFIGURE_REQUEST_EVENT)
					{
						BEvents::ExposeEvent* firstEvent = (BEvents::ExposeEvent*) precursor;
						BEvents::ExposeEvent* nextEvent = (BEvents::ExposeEvent*) event;

						BUtilities::RectArea area = nextEvent->getArea ();
						firstEvent->setArea (area);

						delete event;
						return;
					}

					// EXPOSE_EVENT
					if (eventType == BEvents::EXPOSE_REQUEST_EVENT)
					{
						BEvents::ExposeEvent* firstEvent = (BEvents::ExposeEvent*) precursor;
						BEvents::ExposeEvent* nextEvent = (BEvents::ExposeEvent*) event;

						BUtilities::RectArea area = firstEvent->getArea ();
						area.extend (nextEvent->getArea ());
						firstEvent->setArea (area);

						delete event;
						return;
					}


					// POINTER_MOTION_EVENT
					else if (eventType == BEvents::POINTER_MOTION_EVENT)
					{
						BEvents::PointerEvent* firstEvent = (BEvents::PointerEvent*) precursor;
						BEvents::PointerEvent* nextEvent = (BEvents::PointerEvent*) event;

						firstEvent->setPosition (nextEvent->getPosition ());
						firstEvent->setDelta (firstEvent->getDelta () + nextEvent->getDelta ());

						delete event;
						return;
					}

					// POINTER_DRAG_EVENT
					else if (eventType == BEvents::POINTER_DRAG_EVENT)
					{
						BEvents::PointerEvent* firstEvent = (BEvents::PointerEvent*) precursor;
						BEvents::PointerEvent* nextEvent = (BEvents::PointerEvent*) event;

						if
						(
							(nextEvent->getButton() == firstEvent->getButton()) &&
							(nextEvent->getOrigin() == firstEvent->getOrigin())
						)
						{
							firstEvent->setPosition (nextEvent->getPosition ());
							firstEvent->setDelta (firstEvent->getDelta () + nextEvent->getDelta ());

							delete event;
							return;
						}
					}


					// WHEEL_SCROLL_EVENT
					else if (eventType == BEvents::WHEEL_SCROLL_EVENT)
					{
						BEvents::WheelEvent* firstEvent = (BEvents::WheelEvent*) precursor;
						BEvents::WheelEvent* nextEvent = (BEvents::WheelEvent*) event;

						if (nextEvent->getPosition() == firstEvent->getPosition())
						{
							firstEvent->setDelta (firstEvent->getDelta () + nextEvent->getDelta ());

							delete event;
							return;
						}
					}

					// VALUE_CHANGED_EVENT
					else if (eventType == BEvents::VALUE_CHANGED_EVENT)
					{
						BEvents::ValueChangedEvent* firstEvent = (BEvents::ValueChangedEvent*) precursor;
						BEvents::ValueChangedEvent* nextEvent = (BEvents::ValueChangedEvent*) event;

						firstEvent->setValue (nextEvent->getValue());
						delete event;
						return;
					}
				}
			}
		}
	}

	eventQueue_.push_back (event);
}

BDevices::DeviceGrabStack<uint32_t>* Window::getKeyGrabStack () {return &keyGrabStack_;}

BDevices::DeviceGrabStack<BDevices::MouseDevice>* Window::getButtonGrabStack () {return &buttonGrabStack_;}

void Window::handleEvents ()
{
	puglUpdate (world_, 0);
	translateTimeEvent ();

	while (!eventQueue_.empty ())
	{
		BEvents::Event* event = eventQueue_.front ();
		eventQueue_.pop_front ();

		if (event)
		{
			Widget* widget = event->getWidget ();
			if (widget)
			{
				BEvents::EventType eventType = event->getEventType ();

				switch (eventType)
				{
				case BEvents::CONFIGURE_REQUEST_EVENT:
					widget->onConfigureRequest ((BEvents::ExposeEvent*) event);
					break;

				// Expose events: Forward to pugl!
				case BEvents::EXPOSE_REQUEST_EVENT:
					widget->onExposeRequest ((BEvents::ExposeEvent*) event);
					break;

				case BEvents::CLOSE_REQUEST_EVENT:
					widget->onCloseRequest ((BEvents::WidgetEvent*) event);
					break;

				case BEvents::KEY_PRESS_EVENT:
					buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
					widget->onKeyPressed ((BEvents::KeyEvent*) event);
					break;

				case BEvents::KEY_RELEASE_EVENT:
					buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
					widget->onKeyReleased ((BEvents::KeyEvent*) event);
					break;

				case BEvents::BUTTON_PRESS_EVENT:
					{
						BEvents::PointerEvent* be = (BEvents::PointerEvent*) event;
						unfocus();
						buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
						buttonGrabStack_.add
						(
							BDevices::DeviceGrab<BDevices::MouseDevice>
							(
								widget,
								BDevices::MouseDevice(be->getButton (), be->getPosition())
							)
						);
						widget->onButtonPressed (be);
					}
					break;

				case BEvents::BUTTON_RELEASE_EVENT:
					{
						BEvents::PointerEvent* be = (BEvents::PointerEvent*) event;
						unfocus ();
						buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
						buttonGrabStack_.remove
						(
							BDevices::DeviceGrab<BDevices::MouseDevice>
							(
								widget,
								BDevices::MouseDevice(be->getButton (), be->getPosition())
							)
						);
						widget->onButtonReleased (be);
					}
					break;

				case BEvents::BUTTON_CLICK_EVENT:
					{
						BEvents::PointerEvent* be = (BEvents::PointerEvent*) event;
						unfocus ();
						buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
						buttonGrabStack_.remove
						(
							BDevices::DeviceGrab<BDevices::MouseDevice>
							(
								widget,
								BDevices::MouseDevice(be->getButton (), be->getPosition())
							)
						);
						widget->onButtonClicked (be);
					}
					break;

				case BEvents::POINTER_MOTION_EVENT:
					{
						BEvents::PointerEvent* be = (BEvents::PointerEvent*) event;
						unfocus ();
						buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
						BUtilities::Point p = widget->getAbsolutePosition() + be->getPosition();
						Widget* w = getWidgetAt (p, [] (Widget* f) {return f->isVisible() && f->isFocusable();});
						if (w)
						{
							buttonGrabStack_.add
							(
								BDevices::DeviceGrab<BDevices::MouseDevice>
								(
									w,
									BDevices::MouseDevice(BDevices::NO_BUTTON, p - w->getAbsolutePosition())
								)
							);
						}
						widget->onPointerMotion (be);
					}
					break;

				case BEvents::POINTER_DRAG_EVENT:
					unfocus ();
					buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
					widget->onPointerDragged((BEvents::PointerEvent*) event);
					break;

				case BEvents::WHEEL_SCROLL_EVENT:
					unfocus ();
					buttonGrabStack_.remove (BDevices::MouseDevice (BDevices::NO_BUTTON));
					widget->onWheelScrolled((BEvents::WheelEvent*) event);
					break;

				case BEvents::VALUE_CHANGED_EVENT:
					widget->onValueChanged((BEvents::ValueChangedEvent*) event);
					break;

				case BEvents::FOCUS_IN_EVENT:
					widget->onFocusIn((BEvents::FocusEvent*) event);
					break;

				case BEvents::FOCUS_OUT_EVENT:
					widget->onFocusOut((BEvents::FocusEvent*) event);
					break;

				case BEvents::MESSAGE_EVENT:
					widget->onMessage ((BEvents::MessageEvent*) event);
					break;

				default:
					break;
				}

			}
			delete event;
		}
	}
}

PuglStatus Window::translatePuglEvent (PuglView* view, const PuglEvent* puglEvent)
{
	Window* w = (Window*) puglGetHandle (view);
	if (!w) return PUGL_BAD_PARAMETER;

	switch (puglEvent->type) {

	case PUGL_KEY_PRESS:
		{
			if ((puglEvent->key.key >= PUGL_KEY_F1) && (puglEvent->key.key <= PUGL_KEY_PAUSE))
			{
				uint32_t key = puglEvent->key.key;
				BDevices::DeviceGrab<uint32_t>* grab = w->getKeyGrabStack()->getGrab(key);
				Widget* widget = (grab ? grab->getWidget() : nullptr);
				w->addEventToQueue
				(
					new BEvents::KeyEvent
					(
						widget,
						BEvents::KEY_PRESS_EVENT,
						puglEvent->key.x,
						puglEvent->key.y,
						key
					)
				);
			}
		}
		break;

	case PUGL_KEY_RELEASE:
		{
			if ((puglEvent->key.key >= PUGL_KEY_F1) && (puglEvent->key.key <= PUGL_KEY_PAUSE))
			{
				uint32_t key = puglEvent->key.key;
				BDevices::DeviceGrab<uint32_t>* grab = w->getKeyGrabStack()->getGrab(key);
				Widget* widget = (grab ? grab->getWidget() : nullptr);
				w->addEventToQueue
				(
					new BEvents::KeyEvent
					(
						widget,
						BEvents::KEY_RELEASE_EVENT,
						puglEvent->key.x,
						puglEvent->key.y,
						key
					)
				);
			}
		}
		break;

		case PUGL_TEXT:
			{
				uint32_t key = puglEvent->text.character;
				BDevices::DeviceGrab<uint32_t>* grab = w->getKeyGrabStack()->getGrab(key);
				Widget* widget = (grab ? grab->getWidget() : nullptr);
				w->addEventToQueue
				(
					new BEvents::KeyEvent
					(
						widget,
						BEvents::KEY_PRESS_EVENT,
						puglEvent->text.x,
						puglEvent->text.y,
						key
					)
				);
			}
			break;

		case PUGL_BUTTON_PRESS:
		{
			BUtilities::Point position = BUtilities::Point (puglEvent->button.x, puglEvent->button.y);
			Widget* widget = w->getWidgetAt (position, [] (Widget* w) {return w->isVisible () && w->isClickable ();});
			if (widget)
			{
				w->addEventToQueue
				(
					new BEvents::PointerEvent
					(
						widget,
						BEvents::BUTTON_PRESS_EVENT,
						position - widget->getAbsolutePosition (),
						position - widget->getAbsolutePosition (),
						BUtilities::Point (),
						(BDevices::ButtonCode) puglEvent->button.button
					)
				);
			}
			w->pointer_ = position;
		}
		break;

	case PUGL_BUTTON_RELEASE:
		{
			BUtilities::Point position = BUtilities::Point (puglEvent->button.x, puglEvent->button.y);
			BDevices::ButtonCode button = (BDevices::ButtonCode) puglEvent->button.button;
			BDevices::MouseDevice mouse = BDevices::MouseDevice (button);
			BDevices::DeviceGrab<BDevices::MouseDevice>* grab = w->getButtonGrabStack()->getGrab(mouse);
			if (grab)
			{
				Widget* widget = grab->getWidget();
				if (widget)
				{
					std::set<BDevices::MouseDevice> buttonDevices = grab->getDevices();
					std::set<BDevices::MouseDevice>::iterator it = buttonDevices.find(mouse);
					BUtilities::Point origin = (it != buttonDevices.end() ? it->position : BUtilities::Point ());

					w->addEventToQueue
					(
						new BEvents::PointerEvent
						(
							widget,
							BEvents::BUTTON_RELEASE_EVENT,
							position - widget->getAbsolutePosition (),
							origin,
							BUtilities::Point (),
							button
						)
					);


					// Also emit BUTTON_CLICK_EVENT ?
					Widget* widget2 = w->getWidgetAt (position, [] (Widget* w) {return w->isVisible () && w->isClickable ();});
					if (widget == widget2)
					{
						w->addEventToQueue
						(
							new BEvents::PointerEvent
							(
								widget,
								BEvents::BUTTON_CLICK_EVENT,
								position - widget->getAbsolutePosition (),
								origin,
								BUtilities::Point (),
								button
							)
						);
					}
				}
			}
			w->pointer_ = position;
		}
		break;

	case PUGL_MOTION:
		{
			BUtilities::Point position = BUtilities::Point (puglEvent->motion.x, puglEvent->motion.y);
			BDevices::ButtonCode button = BDevices::NO_BUTTON;

			// Scan for pressed buttons associated with a widget
			for (int i = BDevices::NO_BUTTON + 1; i < BDevices::NR_OF_BUTTONS; ++i)
			{
				BDevices::ButtonCode b = BDevices::ButtonCode (i);
				BDevices::MouseDevice mouse = BDevices::MouseDevice (b);
				BDevices::DeviceGrab<BDevices::MouseDevice>* grab = w->getButtonGrabStack()->getGrab(mouse);

				if (grab)
				{
					button = b;
					Widget* widget = grab->getWidget();

					if (widget && widget->isDraggable())
					{
						std::set<BDevices::MouseDevice> buttonDevices = grab->getDevices();
						std::set<BDevices::MouseDevice>::iterator it = buttonDevices.find(mouse);
						BUtilities::Point origin = (it != buttonDevices.end() ? it->position : BUtilities::Point ());

						// new
						w->addEventToQueue
						(
							new BEvents::PointerEvent
							(
								widget,
								BEvents::POINTER_DRAG_EVENT,
								position - widget->getAbsolutePosition (),
								origin,
								position - w->pointer_,
								button
							)
						);
					}
				}
			}
			// No button associated with a widget? Only POINTER_MOTION_EVENT
			if (button == BDevices::NO_BUTTON)
			{
				Widget* widget = w->getWidgetAt (position, BWidgets::isVisible);
				if (widget)
				{
					w->addEventToQueue
					(
						new BEvents::PointerEvent
						(
							widget,
							BEvents::POINTER_MOTION_EVENT,
							position - widget->getAbsolutePosition (),
							BUtilities::Point (),
							position - w->pointer_,
							button));
				}
			}
			w->pointer_ = position;
		}
		break;

	case PUGL_SCROLL:
		{
			BUtilities::Point position = BUtilities::Point (puglEvent->scroll.x, puglEvent->scroll.y);
			BUtilities::Point scroll = BUtilities::Point (puglEvent->scroll.dx, puglEvent->scroll.dy);
			Widget* widget = w->getWidgetAt (position, [] (Widget* wid) {return wid->isVisible() && wid->isScrollable();});
			if (widget)
			{
				w->addEventToQueue
				(
					new BEvents::WheelEvent
					(
						widget,
						BEvents::WHEEL_SCROLL_EVENT,
						position - widget->getAbsolutePosition (),
						scroll
					)
				);
			}
			w->pointer_ = position;
		}
		break;

	case PUGL_CONFIGURE:
		w->addEventToQueue
		(
			new BEvents::ExposeEvent
			(
				w, w,
				BEvents::CONFIGURE_REQUEST_EVENT,
				puglEvent->configure.x,
				puglEvent->configure.y,
				puglEvent->configure.width,
				puglEvent->configure.height
			)
		);
		break;

	// Expose events handled HERE
	case PUGL_EXPOSE:
		{
			BUtilities::RectArea area = BUtilities::RectArea (puglEvent->expose.x, puglEvent->expose.y, puglEvent->expose.width, puglEvent->expose.height);

			// Create a temporal storage surface and store all children surfaces on this
			cairo_surface_t* storageSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w->getWidth(), w->getHeight());
			if (cairo_surface_status (storageSurface) == CAIRO_STATUS_SUCCESS)
			{
				w->redisplay (storageSurface, area);

				// Copy storage surface onto pugl provided surface
				cairo_t* cr = w->getPuglContext ();
				if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
				{
					cairo_save (cr);
					cairo_set_source_surface (cr, storageSurface, 0, 0);
					cairo_paint (cr);
					cairo_restore (cr);
				}
			}
			cairo_surface_destroy (storageSurface);
		}
		break;

	case PUGL_CLOSE:
		w->addEventToQueue (new BEvents::WidgetEvent (w, w, BEvents::CLOSE_REQUEST_EVENT));
		break;

	default:
		break;
	}

	return PUGL_SUCCESS;
}

void Window::translateTimeEvent ()
{
	BDevices::MouseDevice mouse = BDevices::MouseDevice (BDevices::NO_BUTTON);
	BDevices::DeviceGrab<BDevices::MouseDevice>* grab = buttonGrabStack_.getGrab(mouse);
	if (grab)
	{
		Widget* widget = grab->getWidget();
		if (widget)
		{
			Focusable* focus = dynamic_cast<Focusable*> (widget);
			if (focus)
			{
				std::set<BDevices::MouseDevice> buttonDevices = grab->getDevices();
				std::set<BDevices::MouseDevice>::iterator it = buttonDevices.find(mouse);
				BUtilities::Point position = (it != buttonDevices.end() ? it->position : BUtilities::Point ());
				std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
				std::chrono::steady_clock::time_point pointerTime = (it != buttonDevices.end() ? it->getTime() : nowTime);
				std::chrono::milliseconds diffMs = std::chrono::duration_cast<std::chrono::milliseconds> (nowTime - pointerTime);

				if ((!focused_) && focus->isFocusActive (diffMs))
				{
					addEventToQueue (new BEvents::FocusEvent (widget, BEvents::FOCUS_IN_EVENT, position));
					focused_ = true;
				}

				else if (focused_ && (!focus->isFocusActive (diffMs)))
				{
					addEventToQueue (new BEvents::FocusEvent (widget, BEvents::FOCUS_OUT_EVENT, position));
					focused_ = false;
				}
			}
			else focused_ = false;
		}
		else focused_ = false;
	}
	else focused_ = false;
}

void Window::unfocus ()
{
	if (focused_)
	{
		BDevices::MouseDevice mouse = BDevices::MouseDevice (BDevices::NO_BUTTON);
		BDevices::DeviceGrab<BDevices::MouseDevice>* grab = buttonGrabStack_.getGrab (mouse);
		if (grab)
		{
			Widget* widget = grab->getWidget();
			if (widget)
			{
				Focusable* focus = dynamic_cast<Focusable*> (widget);
				if (focus)
				{
					std::set<BDevices::MouseDevice> buttonDevices = grab->getDevices();
					std::set<BDevices::MouseDevice>::iterator it = buttonDevices.find(mouse);
					BUtilities::Point position = (it != buttonDevices.end() ? it->position : BUtilities::Point ());
					addEventToQueue (new BEvents::FocusEvent (widget, BEvents::FOCUS_OUT_EVENT, position));
				}
			}
		}
		focused_ = false;
	}
}

void Window::purgeEventQueue (Widget* widget)
{
	for (std::deque<BEvents::Event*>::iterator it = eventQueue_.begin (); it != eventQueue_.end (); )
	{
		BEvents::Event* event = *it;
		if
		(
			(event) &&
			(
				// Nullptr = joker
				(widget == nullptr) ||
				// Hit
				(widget == event->getWidget ()) ||
				(
					// Hit in request widgets
					(
						(event->getEventType () == BEvents::CONFIGURE_REQUEST_EVENT) ||
						(event->getEventType () == BEvents::EXPOSE_REQUEST_EVENT) ||
						(event->getEventType () == BEvents::CLOSE_REQUEST_EVENT)
					) &&
					(widget == ((BEvents::WidgetEvent*)event)->getRequestWidget ())
				)
			)
		)
		{
			it = eventQueue_.erase (it);
			delete event;
		}
		else ++it;
	}
}

}
