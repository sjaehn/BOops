/* Widget.cpp
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

#include "Widget.hpp"
#include "Window.hpp"
#include <algorithm>

namespace BWidgets
{

Widget::Widget () : Widget (0.0, 0.0, BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, "widget") {}

Widget::Widget (const double x, const double y, const double width, const double height) : Widget (x, y, width, height, "widget") {}

Widget::Widget(const double x, const double y, const double width, const double height, const std::string& name) :
		area_ (x, y, width, height),
		visible_ (true), clickable_ (true), draggable_ (false),
		scrollable_ (true), focusable_ (true), scheduleDraw_ (false), stacking_ (STACKING_NORMAL),
		main_ (nullptr), parent_ (nullptr), children_ (), border_ (BWIDGETS_DEFAULT_BORDER), background_ (BWIDGETS_DEFAULT_BACKGROUND),
		name_ (name), widgetSurface_ (), widgetState_ (BWIDGETS_DEFAULT_STATE)
{
	mergeable_.fill (false);
	mergeable_[BEvents::CONFIGURE_REQUEST_EVENT] = true;
	mergeable_[BEvents::EXPOSE_REQUEST_EVENT] = true;
	mergeable_[BEvents::POINTER_MOTION_EVENT] = true;
	mergeable_[BEvents::POINTER_DRAG_EVENT] = true;
	mergeable_[BEvents::WHEEL_SCROLL_EVENT] = true;
	cbfunction_.fill (Widget::defaultCallback);
	cbfunction_[BEvents::EventType::POINTER_DRAG_EVENT] = Widget::dragAndDropCallback;
	widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
}

Widget::Widget (const Widget& that) :
		area_ (that.area_),
		visible_ (that.visible_), clickable_ (that.clickable_), draggable_ (that.draggable_), scrollable_ (that.scrollable_),
		focusable_ (that.focusable_), mergeable_ (that.mergeable_), stacking_ (that.stacking_),
		main_ (nullptr), parent_ (nullptr), children_ (), border_ (that.border_), background_ (that.background_), name_ (that.name_),
		cbfunction_ (that.cbfunction_), widgetSurface_ (), widgetState_ (that.widgetState_)
{
	widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, that.area_.getWidth (), that.area_.getHeight ());
}

Widget::~Widget()
{
	// Release from parent (and main) if still linked
	if (parent_) parent_->release (this);

	// Release children
	while (!children_.empty ())
	{
		Widget* w = children_.back ();
		release (w);

		// Hard kick out if release failed
		if (!children_.empty () && (w == children_.back ())) children_.pop_back ();
	}

	cairo_surface_destroy (widgetSurface_);
}

Widget& Widget::operator= (const Widget& that)
{
	area_ = that.area_;
	visible_ = that.visible_;
	clickable_ = that.clickable_;
	draggable_ = that.draggable_;
	scrollable_ = that.scrollable_;
	focusable_ = that.focusable_;
	mergeable_ = that.mergeable_;
	stacking_ = that.stacking_;
	border_ = that.border_;
	background_ = that.background_;
	name_ = that.name_;
	cbfunction_ = that.cbfunction_;
	widgetState_ = that.widgetState_;

	if (widgetSurface_) cairo_surface_destroy (widgetSurface_);
	widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, that.area_.getWidth (), that.area_.getHeight ());
	update ();
	return *this;
}

Widget* Widget::clone () const {return new Widget (*this);}

void Widget::show ()
{
	visible_ = true;

	if (isVisible ())
	{
		// (Re-)draw children as they may become visible too
		forEachChild ([] (Widget* w)
		{
			if (w->isVisible ()) w->draw (BUtilities::RectArea (0, 0, w->area_.getWidth (), w->area_.getHeight ()));
			return w->isVisible ();
		});

		// (Re-)draw this widget and post redisplay
		update ();
	}
}

void Widget::hide ()
{
	bool wasVisible = isVisible ();

	// Get area occupied by this widget and its children
	BUtilities::RectArea hideArea = getAbsoluteTotalArea (BWidgets::isVisible);
	visible_ = false;

	if (wasVisible && main_)
	{
		// Limit area to main boundaries
		hideArea.intersect (main_->getAbsoluteArea());

		// Find closest parent that includes the area
		Widget* p = getParent();
		for ( ; p && (!p->getAbsoluteArea().includes (hideArea)); p = p->getParent()) {}

		// Redisplay hidden area
		if (p) p->postRedisplay ();
		else main_->postRedisplay (hideArea);
	}
}

void Widget::add (Widget& child)
{
	// Check if already added? -> Release first
	if (child.parent_) child.parent_->release (&child);

	child.main_ = main_;
	child.parent_ = this;

	children_.push_back (&child);

	// Link all children of child to main_ and update children of child as
	// they may become visible too
	if (main_)
	{
		forEachChild ([this] (Widget* w)
		{
			w->main_ = this->main_;
			w->update ();
			return true;
		});
	}

	//TODO Stacking

	// (Re-)draw child widget and post redisplay
	if (child.isVisible ()) child.update ();
}

void Widget::release (Widget* child)
{
	if (child)
	{
		std::vector<Widget*>::iterator it = std::find (children_.begin(), children_.end(), child);

		if (it != children_.end())
		{
			// Store redisplay area information
			bool wasVisible = child->isVisible ();

			// Hide child
			child->hide();

			// Release child and all children of child
			// from main window and from main windows input connections
			forEachChild (it, it + 1, [] (Widget* w)
			{
				if (w->main_)
				{
					w->main_->purgeEventQueue (w);
					w->main_->getButtonGrabStack()->remove (w);
					w->main_->getKeyGrabStack()->remove (w);
					w->main_ = nullptr;
				}
				return true;
			});

			// Delete child's connection to this widget
			child->parent_ = nullptr;

			// Delete from children_ vector
			children_.erase (it);

			// Restore visibility information
			if (wasVisible) postRedisplay (child->getArea());
		}

		else
		{
			std::cerr << "Msg from BWidgets::Widget::release(): Child " << child->name_ << ":" << &(*child)
				  << " is not a child of " << name_ << ":" << &(*this) << std::endl;
		}
	}
}

void Widget::moveTo (const double x, const double y) {moveTo (BUtilities::Point (x, y));}

void Widget::moveTo (const BUtilities::Point& position)
{
	if ((area_.getX () != position.x) || (area_.getY () != position.y))
	{
		area_.moveTo (position);
		if (stacking_ == STACKING_CATCH) stackingCatch();
		if (isVisible () && parent_) parent_->postRedisplay ();
	}
}

BUtilities::Point Widget::getPosition () const {return area_.getPosition();}

BUtilities::Point Widget::getAbsolutePosition () const
{
	BUtilities::Point p = BUtilities::Point (0, 0);
	for (const Widget* w = this; w->parent_; w = w->parent_) p += w->area_.getPosition ();
	return p;
}

void Widget::raiseFrontwards ()
{
	if (parent_)
	{
		int size = parent_->children_.size ();
		for (int i = 0; (i + 1) < size; ++i)
		{
			if (parent_->children_[i] == this)
			{
				// Swap
				Widget* w = parent_->children_[i + 1];
				parent_->children_[i + 1] = parent_->children_[i];
				parent_->children_[i] = w;

				if (parent_->isVisible ()) parent_->postRedisplay ();
				return;
			}
		}
	}
}

void Widget::pushBackwards ()
{
	if (parent_)
	{
		int size = parent_->children_.size ();
		for (int i = 1; i < size; ++i)
		{
			if (parent_->children_[i] == this)
			{
				// Swap
				Widget* w = parent_->children_[i];
				parent_->children_[i] = parent_->children_[i - 1];
				parent_->children_[i - 1] = w;

				if (parent_->isVisible ()) parent_->postRedisplay ();
				return;
			}
		}
	}
}



void Widget::pushToBottom ()
{
	if (parent_)
	{
		// Delete old connection from parent to this widget
		for (std::vector<Widget*>::iterator it = parent_->children_.begin (); it !=parent_->children_.end (); ++it)
		{
			if ((Widget*) *it == this)
			{
				parent_->children_.erase (it);
				break;
			}
		}
		parent_->children_.insert (parent_->children_.begin(), this);

		if (parent_->isVisible ()) parent_->postRedisplay ();
	}
}

void Widget::raiseToTop ()
{
	if (parent_)
	{
		// Delete old connection from parent to this widget
		for (std::vector<Widget*>::iterator it = parent_->children_.begin (); it !=parent_->children_.end (); ++it)
		{
			if ((Widget*) *it == this)
			{
				parent_->children_.erase (it);
				break;
			}
		}
		parent_->children_.push_back (this);

		if (parent_->isVisible ()) parent_->postRedisplay ();
	}
}

void Widget::setWidth (const double width)
{
	if (getWidth () != width)
	{
		area_.resize (width, getHeight ());
		cairo_surface_destroy (widgetSurface_);	// destroy old surface first
		widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, getWidth (), getHeight ());
		update ();
		if (stacking_ == STACKING_CATCH) stackingCatch();
		for (Widget* c : children_)
		{
			if (c && (c->getStacking() == STACKING_CATCH)) c->stackingCatch();
		}
		if (isVisible () && parent_) parent_->postRedisplay ();
	}
}

double Widget::getWidth () const {return area_.getWidth ();}

void Widget::setHeight (const double height)
{
	if (getHeight () != height)
	{
		area_.resize (getWidth (), height);
		cairo_surface_destroy (widgetSurface_);	// destroy old surface first
		widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, getWidth (), getHeight ());
		update ();
		if (stacking_ == STACKING_CATCH) stackingCatch();
		for (Widget* c : children_)
		{
			if (c && (c->getStacking() == STACKING_CATCH)) c->stackingCatch();
		}
		if (isVisible () && parent_) parent_->postRedisplay ();
	}
}

double Widget::getHeight () const {return area_.getHeight ();}

void Widget::resize ()
{
	double height = 2 * getYOffset ();
	double width = 2 * getXOffset ();

	for (Widget* w : children_)
	{
		if (w->getPosition ().x + w->getWidth() > width) width = w->getPosition ().x + w->getWidth();
		if (w->getPosition ().y + w->getHeight() > height) height = w->getPosition ().y + w->getHeight();
	}
	Widget::resize (BUtilities::Point (width, height));
}

void Widget::resize (const double width, const double height) {Widget::resize (BUtilities::Point (width, height));}

void Widget::resize (const BUtilities::Point extends)
{
	if (area_.getExtends () != extends)
	{
		area_.resize (extends.x, extends.y);
		cairo_surface_destroy (widgetSurface_);	// destroy old surface first
		widgetSurface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, getWidth (), getHeight ());
		update ();
		if (stacking_ == STACKING_CATCH) stackingCatch();
		for (Widget* c : children_)
		{
			if (c && (c->getStacking() == STACKING_CATCH)) c->stackingCatch();
		}
		if (isVisible () && parent_) parent_->postRedisplay ();
	}
}

BUtilities::Point Widget::getExtends () const {return BUtilities::Point (area_.getWidth (), area_.getHeight ());}

void Widget::setState (const BColors::State state)
{
	if (state != widgetState_)
	{
		widgetState_ = state;
		update ();
	}
}

BColors::State Widget::getState () const {return widgetState_;}

void Widget::setBorder (const BStyles::Border& border)
{
	border_ = border;
	if (stacking_ == STACKING_CATCH) stackingCatch();
	for (Widget* c : children_)
	{
		if (c && (c->getStacking() == STACKING_CATCH)) c->stackingCatch();
	}
	update ();
}

BStyles::Border* Widget::getBorder () {return &border_;}

void Widget::setBackground (const BStyles::Fill& background)
{
	background_ = background;
	update ();
}

BStyles::Fill* Widget::getBackground () {return &background_;}

Window* Widget::getMainWindow () const {return main_;}

Widget* Widget::getParent () const {return parent_;}

bool Widget::hasChildren () const {return (children_.size () > 0 ? true : false);}

bool Widget::isChild (Widget* child)
{
	for (Widget* w : children_)
	{
		if (w == child) return true;
		if ((!w->children_.empty()) && w->isChild (child)) return true;
	}

	return false;
}

std::vector<Widget*> Widget::getChildren () const {return children_;}

void Widget::rename (const std::string& name) {name_ = name;}

std::string Widget::getName () const {return name_;}

void Widget::setCallbackFunction (const BEvents::EventType eventType, const std::function<void (BEvents::Event*)>& callbackFunction)
{
	if (eventType <= BEvents::EventType::NO_EVENT) cbfunction_[eventType] = callbackFunction;
}

bool Widget::isVisible()
{
	Widget* w;
	for (w = this; w; w = w->parent_)			// Go backwards in widget tree until nullptr
	{
		if (!w->visible_ || !main_) return false;	// widget invisible? -> break as invisible
		if (w == main_) return true;			// main reached ? -> visible
	}
	return false;						// nullptr reached ? -> not connected to main -> invisible
}

void Widget::setClickable (const bool status) {clickable_ = status;}

bool Widget::isClickable () const {return clickable_;}

void Widget::setDraggable (const bool status) {draggable_ = status;}

bool Widget::isDraggable () const {return draggable_;}

void Widget::setScrollable (const bool status) {scrollable_ = status;}

bool Widget::isScrollable () const {return scrollable_;}

void Widget::setFocusable (const bool status) {focusable_ = status;}

bool Widget::isFocusable () const {return focusable_;}

void Widget::setMergeable (const BEvents::EventType eventType, const bool status) {mergeable_[eventType] = status;}

bool Widget::isMergeable (const BEvents::EventType eventType) const {return mergeable_[eventType];}

void Widget::setStacking (const WidgetStacking stacking) {stacking_ = stacking;}

WidgetStacking Widget::getStacking () const {return stacking_;};

void Widget::update ()
{
	//draw (0, 0, width_, height_);
	scheduleDraw_ = true;
	if (isVisible ()) postRedisplay ();
}

BUtilities::RectArea Widget::getArea () const {return area_;}

BUtilities::RectArea Widget::getAbsoluteArea () const
{
	BUtilities::RectArea a = area_;
	a.moveTo (getAbsolutePosition ());
	return a;
}

void Widget::forEachChild (std::function<bool (Widget* widget)> func)
{
	forEachChild (children_.begin(), children_.end(), func);
}

void Widget::forEachChild (std::vector<Widget*>::iterator first, std::vector<Widget*>::iterator last,
		   std::function<bool (Widget* widget)> func)
{
	for (std::vector<Widget*>::iterator it = first; it != last; ++it)
	{
		Widget* w = *it;
		if (w && func(w)) w->forEachChild (func);
	}
}

BUtilities::RectArea Widget::getTotalArea (std::function<bool (Widget* widget)> func)
{
	BUtilities::RectArea a = getAbsoluteTotalArea (func);
	a.moveTo (a.getPosition() - getAbsolutePosition());
	return a;
}

BUtilities::RectArea Widget::getAbsoluteTotalArea (std::function<bool (Widget* widget)> func)
{
	BUtilities::RectArea a = getAbsoluteArea();
	forEachChild ([&a, func] (Widget* w)
	{
		bool check = func (w);
		if (check && (w->getStacking() == STACKING_OVERSIZE)) a.extend (w->getAbsoluteArea());
		return check;
	});

	return a;
}

Widget* Widget::getWidgetAt (const BUtilities::Point& position, std::function<bool (Widget* widget)> func)
{
	BUtilities::RectArea absarea = getAbsoluteArea ();
	return getWidgetAt (getAbsolutePosition () + position, absarea, absarea, func);
}

Widget* Widget::getWidgetAt (const BUtilities::Point& abspos, const BUtilities::RectArea& outerArea,
			     const BUtilities::RectArea& area, std::function<bool (Widget* widget)> func)
{
	BUtilities::RectArea a = (getStacking() == STACKING_OVERSIZE? outerArea : area);
	BUtilities::RectArea thisArea = area_;
	thisArea.moveTo (getAbsolutePosition());
	thisArea.intersect (a);
	if (main_)
	{
		Widget* finalw = (((thisArea != BUtilities::RectArea ()) && thisArea.contains (abspos) && func (this)) ? this : nullptr);

		for (Widget* w : children_)
		{
			if (w)
			{
				Widget* nextw = nullptr;
				if (filter (w)) nextw = w->getWidgetAt (abspos, outerArea, thisArea, func);
				if (nextw) finalw = nextw;
			}
		}
		return finalw;
	}

	else return nullptr;
}

void Widget::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void Widget::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	// Border
	void* borderPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BORDER);
	if (borderPtr) setBorder (*((BStyles::Border*) borderPtr));

	// Background
	void* backgroundPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BACKGROUND);
	if (backgroundPtr) setBackground (*((BStyles::Fill*) backgroundPtr));

	if (borderPtr || backgroundPtr)
	{
		update ();
	}
}

void Widget::onConfigureRequest (BEvents::ExposeEvent* event) {cbfunction_[BEvents::EventType::CONFIGURE_REQUEST_EVENT] (event);}
void Widget::onExposeRequest (BEvents::ExposeEvent* event) {cbfunction_[BEvents::EventType::EXPOSE_REQUEST_EVENT] (event);}

void Widget::onCloseRequest (BEvents::WidgetEvent* event)
{
	cbfunction_[BEvents::EventType::CLOSE_REQUEST_EVENT] (event);

	if ((event) && (event->getWidget () == this))
	{
		Widget* requestWidget = event->getRequestWidget ();
		Widget* parent = (requestWidget ? requestWidget->getParent () : nullptr);
		if (parent && parent->isChild (requestWidget)) release (requestWidget);
	}
}

void Widget::onKeyPressed (BEvents::KeyEvent* event) {cbfunction_[BEvents::EventType::KEY_PRESS_EVENT] (event);}
void Widget::onKeyReleased (BEvents::KeyEvent* event) {cbfunction_[BEvents::EventType::KEY_RELEASE_EVENT] (event);}
void Widget::onButtonPressed (BEvents::PointerEvent* event) {cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);}
void Widget::onButtonReleased (BEvents::PointerEvent* event) {cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);}
void Widget::onButtonClicked (BEvents::PointerEvent* event) {cbfunction_[BEvents::EventType::BUTTON_CLICK_EVENT] (event);}
void Widget::onPointerMotion (BEvents::PointerEvent* event) {cbfunction_[BEvents::EventType::POINTER_MOTION_EVENT] (event);}
void Widget::onPointerDragged (BEvents::PointerEvent* event) {cbfunction_[BEvents::EventType::POINTER_DRAG_EVENT] (event);}
void Widget::onWheelScrolled (BEvents::WheelEvent* event){cbfunction_[BEvents::EventType::WHEEL_SCROLL_EVENT] (event);}
void Widget::onValueChanged (BEvents::ValueChangedEvent* event) {cbfunction_[BEvents::EventType::VALUE_CHANGED_EVENT] (event);}
void Widget::onFocusIn (BEvents::FocusEvent* event) {cbfunction_[BEvents::EventType::FOCUS_IN_EVENT] (event);}
void Widget::onFocusOut (BEvents::FocusEvent* event) {cbfunction_[BEvents::EventType::FOCUS_OUT_EVENT] (event);}
void Widget::onMessage (BEvents::MessageEvent* event) {cbfunction_[BEvents::EventType::MESSAGE_EVENT] (event);}

void Widget::defaultCallback (BEvents::Event* event) {}

void Widget::dragAndDropCallback (BEvents::Event* event)
{
	if (event && event->getWidget())
	{
		Widget* w = event->getWidget();
		BEvents::PointerEvent* pev = (BEvents::PointerEvent*) event;

		w->moveTo (w->getPosition () + pev->getDelta ());
	}
}

void Widget::focusInCallback (BEvents::Event* event) {}
void Widget::focusOutCallback (BEvents::Event* event) {}

double Widget::getXOffset () {return border_.getMargin () + border_.getLine()->getWidth() + border_.getPadding ();}

double Widget::getYOffset () {return border_.getMargin () + border_.getLine()->getWidth() + border_.getPadding ();}

double Widget::getEffectiveWidth ()
{
	double totalBorderWidth = getXOffset ();
	return (getWidth () > 2 * totalBorderWidth ? getWidth () - 2 * totalBorderWidth : 0);
}

double Widget::getEffectiveHeight ()
{
	double totalBorderHeight = getYOffset ();
	return (getHeight () > 2 * totalBorderHeight ? getHeight () - 2 * totalBorderHeight : 0);
}

BUtilities::RectArea Widget::getEffectiveArea ()
{
	return BUtilities::RectArea (getPosition().x + getXOffset(), getPosition().y + getYOffset(), getEffectiveWidth(), getEffectiveHeight());
}

void Widget::postMessage (const std::string& name, const BUtilities::Any content)
{
	if (main_)
	{
		BEvents::MessageEvent* event = new BEvents::MessageEvent (this, name, content);
		main_->addEventToQueue (event);
	}
}

void Widget::postRedisplay ()
{
	BUtilities::RectArea area = getTotalArea (BWidgets::isVisible);
	area.moveTo (getAbsolutePosition ());
	postRedisplay (area);
}

void Widget::postRedisplay (const BUtilities::RectArea& area)
{
	if (main_)
	{
		BEvents::ExposeEvent* event = new BEvents::ExposeEvent (main_, this, BEvents::EXPOSE_REQUEST_EVENT, area);
		main_->addEventToQueue (event);
	}
}

void Widget::postCloseRequest () {postCloseRequest (main_);}

void Widget::postCloseRequest (Widget* handle)
{
	if (handle)
	{
		BEvents::WidgetEvent* event = new BEvents::WidgetEvent (handle, this, BEvents::CLOSE_REQUEST_EVENT);
		if (event) main_->addEventToQueue (event);
	}
}

void Widget::redisplay (cairo_surface_t* surface, const BUtilities::RectArea& area)
{
	if (isVisible())
	{
		// Calculate absolute area position and start private core method
		BUtilities::RectArea absArea = area;
		absArea.moveTo (absArea.getPosition() + getAbsolutePosition());
		redisplay (surface, absArea, absArea);
	}
}

void Widget::redisplay (cairo_surface_t* surface, const BUtilities::RectArea& outerArea, const BUtilities::RectArea& area)
{
	BUtilities::RectArea a = (getStacking() == STACKING_OVERSIZE ? outerArea : area);
	BUtilities::RectArea thisArea = area_; thisArea.moveTo (getAbsolutePosition());
	a.intersect (thisArea);
	if (main_ && visible_)
	{
		if (a != BUtilities::RectArea ())
		{
			// Update draw
			if (scheduleDraw_) draw (BUtilities::RectArea (0, 0, getWidth (), getHeight ()));

			// Copy widgets surface onto main surface
			cairo_t* cr = cairo_create (surface);
			cairo_set_source_surface (cr, widgetSurface_, thisArea.getX(), thisArea.getY());
			cairo_rectangle (cr, a.getX (), a.getY (), a.getWidth (), a.getHeight ());
			cairo_fill (cr);
			cairo_destroy (cr);
		}

		for (Widget* w : children_)
		{
			if (w && filter (w)) w->redisplay (surface, outerArea, a);
		}
	}
}

void Widget::stackingCatch ()
{
	if (parent_)
	{
		BUtilities::RectArea a = area_;
		BUtilities::RectArea pea = BUtilities::RectArea (parent_->getXOffset(), parent_->getYOffset(), parent_->getEffectiveWidth(), parent_->getEffectiveHeight());

		if (a.getX() + a.getWidth() - getXOffset() <= parent_->getXOffset()) a.setX (parent_->getXOffset() - a.getWidth() + getXOffset());
		if (a.getY() + a.getHeight() - getYOffset() <= parent_->getYOffset()) a.setY (parent_->getYOffset() - a.getHeight() + getYOffset());
		if (a.getX() >= parent_->getXOffset() + parent_->getEffectiveWidth() - getXOffset()) a.setX (parent_->getXOffset() + parent_->getEffectiveWidth() - getXOffset());
		if (a.getY() >= parent_->getYOffset() + pea.getHeight() - getYOffset()) a.setY (parent_->getYOffset() + pea.getHeight() - getYOffset());

		if (a != area_)
		{
			area_ = a;
			if (isVisible()) parent_->postRedisplay ();
		}
	}
}

bool Widget::filter (Widget* widget) {return true;}

void Widget::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;
	cairo_surface_clear (widgetSurface_);
	cairo_t* cr = cairo_create (widgetSurface_);

	if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
	{
		scheduleDraw_ = false;

		// Limit cairo-drawing area
		cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
		cairo_clip (cr);

		double radius = border_.getRadius ();

		// Draw background
		double innerBorders = getXOffset ();
		double innerRadius = (radius > border_.getPadding () ? radius - border_.getPadding () : 0);
		cairo_surface_t* fillSurface = background_.getCairoSurface ();
		BColors::Color bc = *background_.getColor();

		if ((getEffectiveWidth () > 0) && (getEffectiveHeight () > 0))
		{
			if ((fillSurface && cairo_surface_status (fillSurface) == CAIRO_STATUS_SUCCESS) || (bc.getAlpha() != 0.0))
			{
				// Background_image ?
				if (fillSurface && cairo_surface_status (fillSurface) == CAIRO_STATUS_SUCCESS) cairo_set_source_surface (cr, fillSurface, 0, 0);

				// Plain Background color ?
				else cairo_set_source_rgba (cr, CAIRO_RGBA(bc));

				// If drawing area < background are, draw only a rectangle for the drawing area (faster)
				if
				(
					(area.getX () >= innerBorders) && (area.getX () + area.getWidth () <= getWidth () - innerBorders) &&
					(area.getY () >= innerBorders) && (area.getY () + area.getHeight () <= getHeight () - innerBorders)
				)
				{
					cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				}
				else
				{
					cairo_rectangle_rounded (cr, innerBorders, innerBorders, getEffectiveWidth (), getEffectiveHeight (), innerRadius, 0b1111);
				}
				cairo_fill (cr);
			}

		}

		// Draw border frame
		double outerBorders = border_.getMargin ();
		BColors::Color lc = *border_.getLine()->getColor();

		if
		(
			(lc.getAlpha() != 0.0) &&
			(border_.getLine()->getWidth() != 0.0) &&
			(getWidth () >= 2 * outerBorders) &&
			(getHeight () >= 2 * outerBorders)
		)
		{
			double lw = border_.getLine()->getWidth();
			cairo_rectangle_rounded
			(
				cr,
				outerBorders + lw / 2,
				outerBorders + lw / 2,
				getWidth () - 2 * outerBorders - lw,
				getHeight () - 2 * outerBorders - lw,
				radius, 0b1111);

			cairo_set_source_rgba (cr, CAIRO_RGBA (lc));
			cairo_set_line_width (cr, lw);
			cairo_stroke (cr);
		}
	}

	cairo_destroy (cr);
}

bool isVisible (Widget* widget) {return widget->isVisible();}
bool isClicklable (Widget* widget) {return widget->isClickable();}
bool isDraggable (Widget* widget) {return widget->isDraggable();}
bool isScrollable (Widget* widget) {return widget->isScrollable();}
bool isFocusable (Widget* widget) {return widget->isFocusable();}

}
