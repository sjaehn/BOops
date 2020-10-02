/* BDevices.hpp
 * Copyright (C) 2019  Sven Jähnichen
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

#ifndef BWIDGETS_BDEVICES_HPP_
#define BWIDGETS_BDEVICES_HPP_

#include <set>
#include <list>
#include <chrono>
#include <algorithm>
#include "../BUtilities/Point.hpp"

namespace BWidgets
{
class Widget;	// Forward declaration
}

namespace BDevices
{

/**
 * Enumeration of mouse buttons as input device for event handling
 */
enum ButtonCode
{
	NO_BUTTON	= 0,
	LEFT_BUTTON	= 1,
	MIDDLE_BUTTON	= 2,
	RIGHT_BUTTON	= 3,
	NR_OF_BUTTONS	= 4
};

enum KeyCode {
	KEY_F1 = 0xE000,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_LEFT,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_INSERT,
	KEY_SHIFT,
	KEY_CTRL,
	KEY_ALT,
	KEY_SUPER
};

/**
 * Class BDevices::DeviceGrab<T>
 *
 * Links a BWidgets::Widget to a std::set<T> of devices
 */
template<typename T> class DeviceGrab
{
protected:
	BWidgets::Widget* widget_;
	std::set<T> devices_;

public:
	DeviceGrab () : DeviceGrab (nullptr, std::set<T> {}) {}
	DeviceGrab (BWidgets::Widget* widget) :
			DeviceGrab (widget, std::set<T> {}) {}
	DeviceGrab (BWidgets::Widget* widget, const T& device) :
			DeviceGrab (widget, std::set<T> {device}) {}
	DeviceGrab (BWidgets::Widget* widget, const std::set<T>& devices) :
			widget_ (widget), devices_ (devices) {}

	/* Get (the pointer to) the widget of this DeviceGrab
	 * @return	Pointer to widget
	 */
	BWidgets::Widget* getWidget () const {return widget_;}

	/* Gets the devices of this DeviceGrab
	 * @return	std::set<T> of devices
	 */
	std::set<T> getDevices () const {return devices_;}

	/* Gets infomation whether this DeviceGrab contains a given device or
	 * not.
	 * @param device	<T> of device
	 * @return		True if device is in this DeviceGrab, otherwise
	 *			false
	 */
	bool contains (const T& device) const
	{
		if (devices_.empty()) return true;	// Empty devices_ used as joker
		return (devices_.find (device) != devices_.end());
	}

};

template<typename T> class DeviceGrabStack : std::list<DeviceGrab<T>>
{
protected:
	std::list<DeviceGrab<T>> stack_;

	bool contains (BWidgets::Widget* widget)
	{
		for (typename std::list<DeviceGrab<T>>::iterator it = stack_.begin(); it != stack_.end(); ++it)
		{
			DeviceGrab<T>& dg = *it;
			if (dg.getWidget() == widget) return true;
		}
		return false;
	}

	std::set<T> getDevices (BWidgets::Widget* widget)
	{
		std::set<T> devices {};
		for (typename std::list<DeviceGrab<T>>::iterator it = stack_.begin(); it != stack_.end(); ++it)
		{
			DeviceGrab<T>& dg = *it;
			if (dg.getWidget() == widget)
			{
				std::set<T> d = dg.getDevices ();
				devices.insert (d.begin(), d.end());
			}
		}

		return devices;
	}

public:
	using std::list<DeviceGrab<T>>::clear;

	/* Removes DeviceGrab devices from the stack. If the DeviceGrab devices
	 * are completely depleted, the empty DeviceGrab is removed from the
	 * stack.
	 * @param widget	All DeviceGrabs with this widget will be removed
	 *			from the stack.
	 * @param device 	Devices device from all DeviceGrabs of the stack
	 *			will be removed.
	 * @param devices	All given devices will be removed from all
	 *			DeviceGrabs of the stack.
	 * @param deviceGrab	For all DeviceGrabs of the stack that match with
	 *			deviceGrab.widget all deviceGrab.devices will be
	 *			removed.
	 */
	void remove (BWidgets::Widget* widget) {remove (DeviceGrab<T> (widget));}
	void remove (const T& device) {remove (DeviceGrab<T> (nullptr, device));}
	void remove (const std::set<T>& devices) {remove (DeviceGrab<T> (nullptr, devices));}
	void remove (const DeviceGrab<T>& deviceGrab)
	{
		bool done = true;
		std::set<T> devices = deviceGrab.getDevices();
		BWidgets::Widget* widget = deviceGrab.getWidget();

		do
		{
			done = true;

			for (typename std::list<DeviceGrab<T>>::iterator it = stack_.begin(); it != stack_.end(); ++it)
			{
				DeviceGrab<T>& dg = *it;
				if ((!widget) || (dg.getWidget() == widget))
				{
					// Erase list item if joker (std::set<T>{}) used
					if (devices.empty())
					{
						stack_.erase (it);
						done = false;
						break;
					}

					std::set<T> stackDevices = dg.getDevices();

					// Deletion of individual devices is not allowed
					// if joker (std::set<T>{}) is set
					if (stackDevices.empty()) {}

					else
					{
						// Build difference
						std::set<T> diff = {};
						std::set_difference
						(
							stackDevices.begin(), stackDevices.end(),
							devices.begin(), devices.end(),
							std::inserter (diff, diff.begin())
						);

						// Erase list item if all devices are deleted
						if (diff.empty())
						{
							stack_.erase (it);
							done = false;
							break;
						}

						// Otherwise replace list item
						else dg = DeviceGrab<T> (dg.getWidget(), diff);
					}
				}
			}
		} while (!done);
	}

	/* Adds a widget to the top of DeviceGrab stack. If the widget is
	 * already inside the stack, it is moved to the top and the linked
	 * devices are combined.
	 * @param deviceGrab	DeviceGrab
	 */
	void add (BWidgets::Widget* widget) {add (DeviceGrab<T> (widget, std::set<T>{}));}
	void add (const DeviceGrab<T>& deviceGrab)
	{
		BWidgets::Widget* widget = deviceGrab.getWidget();
		std::set<T> devices = deviceGrab.getDevices ();

		if (contains(widget))
		{
			std::set<T> d2 = getDevices (widget);
			if (devices.empty() || d2.empty()) devices.clear();
			else devices.insert (d2.begin(), d2.end());
			remove (deviceGrab.getWidget());
		}

		stack_.push_back (DeviceGrab<T> (widget, devices));
	}

	/* Gets (the pointer to) the DeviceGrab containing the respective
	 * device. Starts from the top of the DeviceGrab stack.
	 * @param device	<T> of the respective device.
	 * @return		Pointer to the respective DeviceGrab or nullptr.
	 */
	DeviceGrab<T>* getGrab (const T& device)
	{
		for (typename std::list<DeviceGrab<T>>::reverse_iterator rit = stack_.rbegin (); rit != stack_.rend (); ++rit)
		{
			DeviceGrab<T>& dg = *rit;
			if (dg.contains (device)) return &dg;
		}

		return nullptr;
	}

};

class MouseDevice
{
public:
	ButtonCode button;
	BUtilities::Point position;

protected:
	std::chrono::steady_clock::time_point time_;

public:
	MouseDevice () : MouseDevice (NO_BUTTON, BUtilities::Point ()) {}
	MouseDevice (const ButtonCode but) : MouseDevice (but, BUtilities::Point ()) {}
	MouseDevice (const ButtonCode but, const BUtilities::Point& pos) :
			button (but), position (pos),
			time_ (std::chrono::steady_clock::now()) {}

	std::chrono::steady_clock::time_point getTime () const {return time_;}

	friend inline bool operator< (const MouseDevice& lhs, const MouseDevice& rhs)
	{
		if( lhs.button < rhs.button ) return true;
		return false;
	}

	friend inline bool operator== (const MouseDevice& lhs, const MouseDevice& rhs)
	{
		if( lhs.button == rhs.button ) return true;
		return false;
	}

	friend inline bool operator> (const MouseDevice& lhs, const MouseDevice& rhs) {return rhs < lhs;}
	friend inline bool operator<=(const MouseDevice& lhs, const MouseDevice& rhs) {return !(lhs > rhs);}
	friend inline bool operator>=(const MouseDevice& lhs, const MouseDevice& rhs) {return !(lhs < rhs);}
	friend inline bool operator!=(const MouseDevice& lhs, const MouseDevice& rhs) {return !(lhs==rhs);}
};


}

#endif /*BWIDGETS_BDEVICES_HPP_*/
