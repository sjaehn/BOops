/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef OPTIONWIDGET_HPP_
#define OPTIONWIDGET_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "Ports.hpp"
#include "BNoname01GUI.hpp"
#include "Shape.hpp"

class OptionWidget : public BWidgets::Widget
{
public:
	OptionWidget () : OptionWidget (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionWidget (const double x, const double y, const double width, const double height, const std::string& name) :
		Widget (x, y, width, height, name),
		zoom_ (1.0)
	{
		options.fill (nullptr);
	}

	OptionWidget (const OptionWidget& that) : Widget (that), zoom_ (that.zoom_)
	{
		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (that.options[i]) options[i] = that.options[i]->clone();
			else options[i] = nullptr;

			if (options[i]) add (*options[i]);
		}
	}

	~OptionWidget()
	{
		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (options[i])
			{
				release (options[i]);
				delete (options[i]);
				options[i] = nullptr;
			}
		}
	}

	OptionWidget& operator= (const OptionWidget& that)
	{
		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (options[i])
			{
				release (options[i]);
				delete (options[i]);
				options[i] = nullptr;
			}
		}

		for (int i = 0; i < NR_SLOTS; ++i)
		{
			if (that.options[i]) options[i] = that.options[i]->clone();
			else options[i] = nullptr;

			if (options[i]) add (*options[i]);
		}

		zoom_ = that.zoom_;
		Widget::operator= (that);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionWidget (*this);}

	virtual void setOption (const int option, const double value)
	{
		if (options[option]) ((BWidgets::ValueWidget*)options[option])->setValue (value);
	}

	virtual double getOption (const int option) const
	{
		if (options[option]) return ((BWidgets::ValueWidget*)options[option])->getValue ();
		return 0;
	}

	virtual BWidgets::Widget* getWidget (const int option) {return options[option];}

	virtual void setShape (const Shape<SHAPE_MAXNODES>& shape) {}

	virtual Shape<SHAPE_MAXNODES> getShape() const {return Shape<SHAPE_MAXNODES>();}

	virtual void zoom (const double f)
	{
		if (f == zoom_) return;

		for (Widget* c : getChildren())
		{
			if (c)
			{
				c->moveTo (c->getPosition().x * f / zoom_, c->getPosition().y * f / zoom_);
				c->resize (c->getWidth() * f / zoom_, c->getHeight() * f / zoom_);
			}
		}

		moveTo (getPosition().x * f / zoom_, getPosition().y * f / zoom_);
		resize (getWidth() * f / zoom_, getHeight() * f / zoom_);
		
		zoom_ = f;
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);
		for (BWidgets::Widget* o : options) if (o) o->applyTheme (theme);
	}

	static void valueChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		BNoname01GUI* ui = (BNoname01GUI*) widget->getMainWindow();
		if (ui) ui->optionChangedCallback (event);
	}

protected:
	std::array<BWidgets::Widget*, NR_SLOTS> options;
	double zoom_;
};

#endif /* OPTIONWIDGET_HPP_ */
