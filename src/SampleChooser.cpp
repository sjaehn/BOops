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

#include "SampleChooser.hpp"
#include <new>

#ifndef SF_FORMAT_MP3
#ifndef MINIMP3_IMPLEMENTATION
#define MINIMP3_IMPLEMENTATION
#endif
#endif
#include "Sample.hpp"

SampleChooser::SampleChooser () : SampleChooser (0.0, 0.0, 0.0, 0.0, "FileChooser") {}

SampleChooser::SampleChooser (const double x, const double y, const double width, const double height, const std::string& name) :
	SampleChooser (x, y, width, height, name, ".", {}, "OK") {}

SampleChooser::SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path):
	SampleChooser (x, y, width, height, name, path, {}, "OK") {}

SampleChooser::SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path, const std::vector<BWidgets::FileFilter>& filters) :
	SampleChooser (x, y, width, height, name, path, filters, "OK") {}

SampleChooser::SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::string& buttonText) :
	FileChooser (x, y, width, height, name, path, filters, buttonText),
	waveform (0, 0, 0, 0, name + "/textbox"),
	scrollbar (0, 0, 0, 0, name + "/scrollbar", 0.0, 1.0, 0.0, 1.0, 0.0),
	startMarker (0, 0, 0, 0, name + "/maker"),
	endMarker (0, 0, 0, 0, name + "/maker"),
	sample (nullptr)
{
	fileListBox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, sfileListBoxClickedCallback);
	waveform.setBackground (BWIDGETS_DEFAULT_MENU_BACKGROUND);
	waveform.setBorder ({{BColors::grey, 1.0}, 0.0, 3.0, 0.0});
	scrollbar.minButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, scrollbarChangedCallback);
	scrollbar.maxButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, scrollbarChangedCallback);
	startMarker.setDraggable (true);
	startMarker.setCallbackFunction (BEvents::POINTER_DRAG_EVENT, lineDraggedCallback);
	endMarker.setDraggable (true);
	endMarker.setCallbackFunction (BEvents::POINTER_DRAG_EVENT, lineDraggedCallback);
	add (waveform);
	waveform.add (startMarker);
	waveform.add (endMarker);
	add (scrollbar);
}

SampleChooser::SampleChooser (const SampleChooser& that) :
	FileChooser (that), waveform (that.waveform), scrollbar (that.scrollbar),
	startMarker (that.startMarker), endMarker (that.endMarker), sample (nullptr)
{
	try {sample = new Sample (*that.sample);}
	catch (std::exception& exc) {throw exc;}

	add (waveform);
	waveform.add (startMarker);
	waveform.add (endMarker);
	add (scrollbar);
}

SampleChooser::~SampleChooser()
{
	if (sample) delete (sample);
}

SampleChooser& SampleChooser::operator= (const SampleChooser& that)
{
	waveform.release (&scrollbar);
	waveform.release (&startMarker);
	release (&waveform);
	release (&endMarker);
	if (sample) delete (sample);

	waveform = that.waveform;
	scrollbar = that.scrollbar;
	startMarker = that. startMarker;
	endMarker = that.endMarker;
	try {sample = new Sample (*that.sample);}
	catch (std::exception& exc) {throw exc;}
	FileChooser::operator= (that);

	add (waveform);
	waveform.add (startMarker);
	waveform.add (endMarker);
	add (scrollbar);

	return *this;
}

BWidgets::Widget* SampleChooser::clone () const {return new SampleChooser (*this);}

int64_t SampleChooser::getStart() const {return (sample ? LIMIT (sample->start, 0, sample->info.frames - 1) : 0);}

int64_t SampleChooser::getEnd() const {return (sample ? LIMIT (sample->end, 1, sample->info.frames) : 0);}

void SampleChooser::resize () {resize (BWIDGETS_DEFAULT_SAMPLECHOOSER_WIDTH, BWIDGETS_DEFAULT_SAMPLECHOOSER_HEIGHT);}

void SampleChooser::resize (const double width, const double height)  {resize (BUtilities::Point (width, height));}

void SampleChooser::resize (const BUtilities::Point extends) {Widget::resize (extends);}

void SampleChooser::update ()
{
	double x0 = getXOffset();
	double y0 = getYOffset();
	double w = getEffectiveWidth();
	double h = getEffectiveHeight();

	if ((w >= 20) && (h >= 20))
	{
		double val = fileListBox.getValue();
		if ((val == UNSELECTED) || (val > dirs.size())) okButton.getLabel()->setText (okButtonText);
		else okButton.getLabel()->setText ("Open");

		// Get extends first
		okButton.resize();
		cancelButton.resize ();
		double okWidth = (okButton.getWidth() > cancelButton.getWidth() ? okButton.getWidth() : cancelButton.getWidth());
		double okHeight = (okButton.getHeight() > cancelButton.getHeight() ? okButton.getHeight() : cancelButton.getHeight());
		pathNameBox.resize();
		double pathNameHeight = pathNameBox.getHeight();
		fileNameBox.resize();
		double fileNameHeight = fileNameBox.getHeight();
		fileNameLabel.resize();
		double fileNameWidth = fileNameLabel.getWidth();

		pathNameBox.moveTo (x0 + 10, y0 + 10);
		pathNameBox.resize (w - 20, pathNameHeight);

		okButton.moveTo (x0 + w - okWidth - 10, y0 + h - okHeight - 10);
		okButton.resize (okWidth, okHeight);

		cancelButton.moveTo (x0 + w - 2 * okWidth - 20, y0 + h - okHeight - 10);
		cancelButton.resize (okWidth, okHeight);

		fileNameLabel.moveTo (x0 + 10, y0 + h - okHeight - fileNameHeight - 20);
		fileNameLabel.resize (fileNameWidth, fileNameHeight);

		fileNameBox.moveTo (x0 + fileNameWidth + 30, y0 + h - okHeight - fileNameHeight - 20);
		fileNameBox.resize (w - fileNameWidth - 40, fileNameHeight);

		filterPopupListBox.moveTo (x0 + 10, y0 + h - okHeight - 10);
		filterPopupListBox.resize (w - 2 * okWidth - 40, okHeight);
		filterPopupListBox.resizeListBox (BUtilities::Point (w - 2 * okWidth - 40, filters.size() * okHeight + 20)); // TODO Limit
		filterPopupListBox.resizeListBoxItems (BUtilities::Point (w - 2 * okWidth - 40, okHeight));

		okButton.show();
		cancelButton.show();
		fileNameLabel.show();
		fileNameBox.show();
		filterPopupListBox.show ();

		if (h > pathNameHeight + okHeight + fileNameHeight + 60)
		{
			fileListBox.moveTo (x0 + 10, y0 + pathNameHeight + 20);
			fileListBox.resize (0.4 * w - 15, h - pathNameHeight - okHeight - fileNameHeight - 50);
			fileListBox.resizeItems (BUtilities::Point (fileListBox.getEffectiveWidth(), 20));
			fileListBox.show();

			startMarker.resize (6.0, h - pathNameHeight - okHeight - fileNameHeight - 50);
			startMarker.show();
			endMarker.resize (6.0, h - pathNameHeight - okHeight - fileNameHeight - 50);
			endMarker.show();

			waveform.moveTo (x0 + 0.4 * w + 5, y0 + pathNameHeight + 20);
			waveform.resize (0.6 * w - 15, h - pathNameHeight - okHeight - fileNameHeight - 50);
			drawWaveform();
			waveform.show();

			scrollbar.moveTo (x0 + 0.4 * w + 5, y0 + h - okHeight - fileNameHeight - 42);
			scrollbar.resize (0.6 * w - 15, 10);
			if (sample && (sample->info.frames > 0))
			{
				startMarker.show();
				endMarker.show();
				scrollbar.show();
			}
			else
			{
				startMarker.hide();
				endMarker.hide();
				scrollbar.hide();
			}
		}
		else
		{
			fileListBox.hide();
			waveform.hide();
			scrollbar.hide();
			startMarker.hide();
			endMarker.hide();
		}
	}

	else
	{
		okButton.hide();
		cancelButton.hide();
		fileListBox.hide();
		waveform.hide();
		scrollbar.hide();
		startMarker.hide();
		endMarker.hide();
		fileNameLabel.hide();
		fileNameBox.hide();
		filterPopupListBox.hide ();
	}

	Widget::update();
}

void SampleChooser::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void SampleChooser::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	FileChooser::applyTheme (theme, name);
	waveform.applyTheme (theme, name + "/textbox");
	scrollbar.applyTheme (theme, name + "/scrollbar");
	startMarker.applyTheme (theme, name + "/marker");
	endMarker.applyTheme (theme, name + "/marker");
}

void SampleChooser::sfileListBoxClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ListBox* w = (BWidgets::ListBox*)event->getWidget();
	if (!w) return;
	SampleChooser* fc = (SampleChooser*)w->getParent();
	if (!fc) return;

	double val = w->getValue();

	if ((val != UNSELECTED) && (!fc->fileNameBox.getEditMode()))
	{
		// Directory selected -> one click chdir
		if (val <= fc->dirs.size())
		{
			fc->fileNameBox.setText ("");
			if (fc->sample)
			{
				delete (fc->sample);
				fc->sample = nullptr;
			}
			BEvents::ValueChangedEvent dummyEvent = BEvents::ValueChangedEvent (&fc->okButton, 1.0);
			fc->okButtonClickedCallback (&dummyEvent);
		}

		// File selected
		else
		{
			BItems::Item* ai = w->getActiveItem();
			if (ai)
			{
				BWidgets::Label* ail = (BWidgets::Label*)ai->getWidget();
				if (ail) fc->fileNameBox.setText (ail->getText());
				std::string newPath = fc->getPath() + "/" + ail->getText();
				char buf[1024];
				char *rp = realpath(newPath.c_str(), buf);
				if (fc->sample)
				{
					delete (fc->sample);
					fc->sample = nullptr;
				}
				try {fc->sample = new Sample (rp);}
				catch (std::exception& exc) {fprintf(stderr, "Can't load %s\n", rp);}

				fc->scrollbar.minButton.setValue (0.0);
				fc->scrollbar.maxButton.setValue (1.0);
			}
		}

		fc->update();
	}
}

void SampleChooser::scrollbarChangedCallback (BEvents::Event* event)
{
	if (!event) return;
	HRangeScrollbar::EndButton* w = (HRangeScrollbar::EndButton*)event->getWidget();
	if (!w) return;
	HRangeScrollbar* hrs = (HRangeScrollbar*)w->getParent();
	if (!hrs) return;
	SampleChooser* fc = (SampleChooser*)hrs->getParent();
	if (!fc) return;

	fc->drawWaveform();
}

void SampleChooser::lineDraggedCallback (BEvents::Event* event)
{
	if (!event) return;
	BEvents::PointerEvent* pev = (BEvents::PointerEvent*)event;
	VLine* w = (VLine*)event->getWidget();
	if (!w) return;
	BWidgets::DrawingSurface* ds = (BWidgets::DrawingSurface*)w->getParent();
	if (!ds) return;
	SampleChooser* fc = (SampleChooser*)ds->getParent();
	if ((!fc) || (!fc->sample) || (fc->sample->info.frames == 0) || (fc->waveform.getEffectiveWidth() <= 0.0)) return;

	const double start = fc->scrollbar.minButton.getValue();
	const double range = fc->scrollbar.maxButton.getValue() - start;
	const double dp = pev->getDelta().x / fc->waveform.getEffectiveWidth();
	const double df = dp * range * double (fc->sample->info.frames);

	if (w == &fc->startMarker) fc->sample->start = LIMIT (fc->sample->start + df, 0, fc->sample->info.frames - 1);
	else if (w == &fc->endMarker) fc->sample->end = LIMIT (fc->sample->end + df, 1, fc->sample->info.frames);

	if (fc->sample->start >= fc->sample->end) fc->sample->start = fc->sample->end - 1;
	fc->drawWaveform();
}

void SampleChooser::drawWaveform()
{
	const double x0 = waveform.getXOffset();
	const double y0 = waveform.getYOffset();
	const double w = waveform.getEffectiveWidth();
	const double h = waveform.getEffectiveHeight();

	cairo_surface_clear (waveform.getDrawingSurface());
	cairo_t* cr = cairo_create (waveform.getDrawingSurface ());
	if (cr && cairo_status (cr) == CAIRO_STATUS_SUCCESS)
	{
		if (sample)
		{
			// Scan for min/max
			const double start = scrollbar.minButton.getValue();
			const double range = scrollbar.maxButton.getValue() - start;
			double max = 1.0;
			for (double i = 0.0; i < 1.0; i += 0.25 / w)
			{
				if (fabs (sample->get (i * double (sample->info.frames), 0, sample->info.samplerate)) > max)
				{
					max = fabs (sample->get (i * double (sample->info.frames), 0, sample->info.samplerate));
				}
			}

			// Draw
			cairo_set_line_width (cr, 1.0);
			cairo_move_to (cr, x0, y0 + 0.5 * h - 0.5 * h * sample->get (start * double (sample->info.frames), 0, sample->info.samplerate));
			for (double i = 0.1 / w; i < 1.0; i += 0.25 / w)
			{
				const double frame = (start + i * range) * double (sample->info.frames);
				if ((frame >= sample->start) && (frame <= sample->end)) cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.25);
				else cairo_set_source_rgba (cr, 0.25, 0.25, 0.25, 0.25);
				const double yp = y0 + 0.5 * h - 0.5 * h * sample->get (frame, 0, sample->info.samplerate) / max;
				cairo_line_to (cr, x0 + i * w, yp);
				cairo_stroke (cr);
				cairo_move_to (cr, x0 + i * w, yp);
			}

			// Set start and end line
			if (range > 0)
			{
				const double sp = (sample->start / double (sample->info.frames) - start) / range;
				startMarker.moveTo (x0 + sp * w - 0.5 * startMarker.getWidth(), 0.0);
				const double ep = (sample->end / double (sample->info.frames) - start) / range;
				endMarker.moveTo (x0 + ep * w - 0.5 * endMarker.getWidth(), 0.0);
			}

			else
			{
				startMarker.moveTo (-startMarker.getWidth(), 0.0);
				endMarker.moveTo (-startMarker.getWidth(), 0.0);
			}
		}

		else
		{

		}
		cairo_destroy (cr);
	}
	waveform.update ();
}

std::function<void (BEvents::Event*)> SampleChooser::getFileListBoxClickedCallback()
{
	return sfileListBoxClickedCallback;
}
