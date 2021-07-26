/* B.Oops
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

#ifndef OPTIONEQ_HPP_
#define OPTIONEQ_HPP_

#include <new>
#include "OptionWidget.hpp"
#include "BWidgets/Label.hpp"
#include "DialRange.hpp"
#include "BiquadPeakFilter.hpp"

class EQDisplay : public BWidgets::Widget
{
public:
	EQDisplay () : EQDisplay (0, 0, 0, 0, "pad0") {}
	EQDisplay (const double x, const double y, const double width, const double height, const std::string& name):
		Widget (x, y, width, height, name),
		fgColors (BWIDGETS_DEFAULT_FGCOLORS),
		bgColors (BWIDGETS_DEFAULT_BGCOLORS),
		lbfont (BWIDGETS_DEFAULT_FONT),
		gains {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		freqs {30.0f, 80.0f, 300.0f, 1500.0f, 4000.0f, 15000.0f},
		Qs {M_SQRT1_2, 1.0, M_SQRT1_2, 1.0, 1.0, 1.0}
	{
		for (int i = 0; i < 6; ++i) filters[i].set (freqs[i], Qs[i], gains[i]);
	}

	virtual Widget* clone () const override {return new EQDisplay (*this);}

	void setGain (const int index, const float value) 
	{
		gains[index] = value;
		filters[index].setPeakGain (value);
		update();
	}
	
	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr) fgColors = *((BColors::ColorSet*) fgPtr);

		void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
		if (bgPtr) bgColors = *((BColors::ColorSet*) bgPtr);

		void* fontPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FONT);
		if (fontPtr) lbfont = *((BStyles::Font*) fontPtr);

		if (fgPtr || bgPtr || fontPtr) update ();
	}

protected:
	BColors::ColorSet fgColors;
	BColors::ColorSet bgColors;
	BStyles::Font lbfont;
	float gains[6];
	float freqs[6];
	float Qs[6];
	std::array<BiquadPeakFilter, 6> filters;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 1) && (getHeight () >= 1))
		{
			// Draw super class widget elements first
			Widget::draw (area);

			
			cairo_t* cr = cairo_create (widgetSurface_);
			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				double x0 = getXOffset ();
				double y0 = getYOffset ();
				double w = getEffectiveWidth ();
				double h = getEffectiveHeight ();

				if ((w > 20) && (h > 20))
				{
					BColors::Color fg = *fgColors.getColor (getState ());
					BColors::Color bg = *bgColors.getColor (getState ());
					cairo_text_extents_t ext;
					cairo_select_font_face (cr, lbfont.getFontFamily ().c_str (), lbfont.getFontSlant (), lbfont.getFontWeight ());
					cairo_set_font_size (cr, 0.8 * lbfont.getFontSize ());

					// Frame
					cairo_set_line_width (cr, 2.0);
					cairo_set_source_rgba (cr, CAIRO_RGBA (bg));
					cairo_move_to (cr, x0 + 20, y0);
					cairo_line_to (cr, x0 + 20, y0 + h - 20);
					cairo_line_to (cr, x0 + w, y0 + h - 20);
					cairo_stroke (cr);

					// X steps
					for (int i = 1; i <= 3; ++i)
					{
						for (int j = 2; j <= 10; ++j)
						{
							if (j == 10) cairo_set_line_width (cr, 1.0);
							else cairo_set_line_width (cr, 0.5);
							const double f = j * pow (10, i);
							const double x = (w - 20.0) * log10 (f / 20.0) / 3.0;
							cairo_move_to (cr, x0 + 20 + x, y0);
							cairo_line_to (cr, x0 + 20 + x, y0 + h - 20);
							cairo_stroke (cr);
						}

						const int lval = pow (10, i + 1);
						std::string label = (lval >= 1000 ? std::to_string (int (lval / 1000)) + " kHz" : std::to_string (lval) + " Hz");
						cairo_text_extents (cr, label.c_str(), &ext);
						const double x = (w - 20.0) * log10 (lval / 20.0) / 3.0;
						cairo_move_to (cr, x0 + 20.0 + x - 0.5 * ext.width - ext.x_bearing, y0 + h - 10 - ext.height / 2 - ext.y_bearing);
						cairo_show_text (cr, label.c_str ());
					}

					// Y Steps
					cairo_set_line_width (cr, 1.0);
					for (int g = -36; g <= 36; g += 12)
					{
						if (g == 0) cairo_set_line_width (cr, 1.0);
						else cairo_set_line_width (cr, 0.5);
						const double y = y0 + 0.5 * (h - 20.0) - (double (g) / 48.0f) * 0.5 * (h - 20.0);
						cairo_move_to (cr, x0 + 20.0, y);
						cairo_line_to (cr, x0 + w, y);
						cairo_stroke (cr);

						std::string label = std::to_string (g);
						cairo_text_extents (cr, label.c_str(), &ext);
						cairo_move_to (cr, x0 + 10.0 - 0.5 * ext.width - ext.x_bearing, y - ext.height / 2 - ext.y_bearing);
						cairo_show_text (cr, label.c_str ());
					}

					cairo_set_source_rgba (cr, CAIRO_RGBA (fg));
					cairo_set_line_width (cr, 2.0);

					// Curve
					for (double x = 0; x <= w - 20; ++x)
					{
						float f = 20.0 * pow (10.0, 3.0 * x / (w - 20.0));
						float g = 0.0f;
						for (int i = 0; i < 6; ++i)
						{
							g += filters[i].result (f);
						}
						if (x == 0) cairo_move_to (cr, x0 + 20.0 + x, y0 + 0.5 * (h - 20.0) - (g / 48.0f) * 0.5 * (h - 20.0));
						cairo_line_to (cr, x0 + 20.0 + x, y0 + 0.5 * (h - 20.0) - (g / 48.0f) * 0.5 * (h - 20.0));
					}

					cairo_stroke_preserve (cr);

					// Area under the curve
					cairo_set_line_width (cr, 0.0);
					cairo_line_to (cr, x0 + w, y0 + h - 20);
					cairo_line_to (cr, x0 + 20, y0 + h - 20);
					cairo_close_path (cr);
					cairo_pattern_t* pat = cairo_pattern_create_linear (0, y0 + h, 0, y0);
					cairo_pattern_add_color_stop_rgba (pat, 0, fg.getRed (), fg.getGreen (), fg.getBlue (), 0);
					cairo_pattern_add_color_stop_rgba (pat, 1, fg.getRed (), fg.getGreen (), fg.getBlue (), 0.5 * fg.getAlpha ());
					cairo_set_source (cr, pat);
					cairo_fill (cr);
					cairo_pattern_destroy (pat);
				}
				
				cairo_destroy (cr);
			}
		}
	}
};

class OptionEQ : public OptionWidget
{
public:
	OptionEQ () : OptionEQ (0.0, 0.0, 0.0, 0.0, "widget") {}
	OptionEQ (const double x, const double y, const double width, const double height, const std::string& name) :
		OptionWidget (x, y, width, height, name),
		subLabel (0, 90, 80, 20, "ctlabel", BOOPS_LABEL_SUB),
		boomLabel (80, 90, 80, 20, "ctlabel", BOOPS_LABEL_BOOM),
		warmthLabel (160, 90, 80, 20, "ctlabel", BOOPS_LABEL_WARMTH),
		eqDisplay (240, 20, 240, 100, "pad0"),
		clarityLabel (480, 90, 80, 20, "ctlabel", BOOPS_LABEL_CLARITY),
		presenceLabel (560, 90, 80, 20, "ctlabel", BOOPS_LABEL_PRESENCE),
		airLabel (640, 90, 80, 20, "ctlabel", BOOPS_LABEL_AIR)
	{
		try
		{
			for (int i = 0; i < 3; ++i)
			{
				options[2 * i] = new DialRange (i * 80 + 10, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "db", [] (double x) {return 72.0 * x - 36.0;}, [] (double x) {return 0.5 * (LIMIT (x, -36.0, 36.0) + 1.0);});
				options[2 * i + 1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			}

			for (int i = 3; i < 6; ++i)
			{
				options[2 * i] = new DialRange (i * 80 + 250, 20, 60, 60, "pad0", 0.5, 0.0, 1.0, 0.0, BIDIRECTIONAL, "%1.1f", "db", [] (double x) {return 72.0 * x - 36.0;}, [] (double x) {return 0.5 * (LIMIT (x, -36.0, 36.0) + 1.0);});
				options[2 * i + 1] = new BWidgets::ValueWidget (0, 0, 0, 0, "widget", 0.0);
			}

		}
		catch (std::bad_alloc& ba) {throw ba;}

		for (int i = 0; i < 12; i += 2)
		{
			options[i]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
			((DialRange*)options[i])->range.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, rangeChangedCallback);
			options[i + 1]->setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, valueChangedCallback);
		}

		add (subLabel);
		add (boomLabel);
		add (warmthLabel);
		add (clarityLabel);
		add (presenceLabel);
		add (airLabel);
		add (eqDisplay);
		for (int i = 0; i < 12; ++i) add (*options[i]);
	}

	OptionEQ (const OptionEQ& that) :
		OptionWidget (that),
		subLabel (that.subLabel), boomLabel (that.boomLabel),
		warmthLabel (that.warmthLabel), eqDisplay (that.eqDisplay),
		clarityLabel (that.clarityLabel), 
		presenceLabel (that.presenceLabel), airLabel (that.airLabel)
	{
		add (subLabel);
		add (boomLabel);
		add (warmthLabel);
		add (clarityLabel);
		add (presenceLabel);
		add (airLabel);
		add (eqDisplay);
	}

	OptionEQ& operator= (const OptionEQ& that)
	{
		release (&subLabel);
		release (&boomLabel);
		release (&warmthLabel);
		release (&clarityLabel);
		release (&presenceLabel);
		release (&airLabel);
		release (&eqDisplay);
		OptionWidget::operator= (that);
		subLabel = that.subLabel;
		boomLabel = that.boomLabel;
		warmthLabel = that.warmthLabel;
		clarityLabel = that.clarityLabel;
		presenceLabel = that.presenceLabel;
		airLabel = that.airLabel;
		eqDisplay = that.eqDisplay;
		add (subLabel);
		add (boomLabel);
		add (warmthLabel);
		add (clarityLabel);
		add (presenceLabel);
		add (airLabel);
		add (eqDisplay);

		return *this;
	}

	virtual Widget* clone () const override {return new OptionEQ (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		OptionWidget::applyTheme (theme, name);
		subLabel.applyTheme (theme);
		boomLabel.applyTheme (theme);
		warmthLabel.applyTheme (theme);
		clarityLabel.applyTheme (theme);
		presenceLabel.applyTheme (theme);
		airLabel.applyTheme (theme);
		eqDisplay.applyTheme (theme);
	}

	static void valueChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		OptionEQ* p = (OptionEQ*) widget->getParent();
		if (!p) return;
		BOopsGUI* ui = (BOopsGUI*) widget->getMainWindow();
		if (!ui) return;

		// options[i + 1] changed ? Send to range
		for (int i = 0; i < 12; i += 2)
		{
			if (widget == p->getWidget(i + 1))
			{
				((DialRange*)p->getWidget(i))->range.setValue (((BWidgets::ValueWidget*)widget)->getValue());
				break;
			}

			else if (widget == p->getWidget(i))
			{
				p->eqDisplay.setGain (i / 2, 72.0 * ((BWidgets::ValueWidget*)widget)->getValue() - 36.0);
				break;
			}
		}

		// Forward all changed options to ui
		ui->optionChangedCallback (event);
	}

	static void rangeChangedCallback(BEvents::Event* event)
	{
		if (!event) return;
		BWidgets::Widget* widget = event->getWidget ();
		if (!widget) return;
		DialRange* p = (DialRange*) widget->getParent();
		if (!p) return;
		OptionWidget* pp = (OptionWidget*) p->getParent();
		if (!pp) return;

		// Send changed range to options[i + 1]
		for (int i = 0; i < 12; i += 2)
		{
			if ((p == (DialRange*)pp->getWidget(i)) && (widget == (BWidgets::Widget*)&p->range))
			{
				p->update();
				((BWidgets::ValueWidget*)pp->getWidget(i + 1))->setValue (p->range.getValue ());
			}
		}
	}

protected:
	BWidgets::Label subLabel;
	BWidgets::Label boomLabel;
	BWidgets::Label warmthLabel;
	BWidgets::Label clarityLabel;
	BWidgets::Label presenceLabel;
	BWidgets::Label airLabel;
	EQDisplay eqDisplay;
};

#endif /* OPTIONEQ_HPP_ */
