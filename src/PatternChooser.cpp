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

#include "PatternChooser.hpp"
#include <limits.h>		// PATH_MAX
#include <fstream>

PatternChooser::PatternChooser () : PatternChooser (0.0, 0.0, 0.0, 0.0, "FileChooser") {}

PatternChooser::PatternChooser (const double x, const double y, const double width, const double height, const std::string& name) :
	PatternChooser (x, y, width, height, name, ".", {}, "OK") {}

PatternChooser::PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path):
	PatternChooser (x, y, width, height, name, path, {}, "OK") {}

PatternChooser::PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path, const std::vector<BWidgets::FileFilter>& filters) :
	PatternChooser (x, y, width, height, name, path, filters, "OK") {}

PatternChooser::PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::string& buttonText) :
	PatternChooser (x, y, width, height, name, path, filters, std::vector<std::string>{"OK"}) {}

PatternChooser::PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
			      const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::vector<std::string>& texts) :
	FileChooser (x, y, width, height, name, path, filters, texts),
	Pattern (),
	patternDisplay (0, 0, 0, 0, name + "/textbox"),
	noFileLabel (0, 0, 0, 0, name + "/label"),
	patternValid (false),
	additionalData ("")
{
	clear();

	std::vector<std::string> patternLabels = {"No pattern file selected"};
	labels.insert (labels.end(), patternLabels.begin(), patternLabels.end());
	for (int i = BWIDGETS_DEFAULT_PATTERNCHOOSER_NO_FILE_INDEX; (i < int(texts.size())) && (i < int(labels.size())); ++i) labels[i] = texts[i];
	noFileLabel.setText (labels[BWIDGETS_DEFAULT_PATTERNCHOOSER_NO_FILE_INDEX]);
	noFileLabel.hide();

	fileListBox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, sfileListBoxClickedCallback);
	patternDisplay.setBackground (BWIDGETS_DEFAULT_MENU_BACKGROUND);
	patternDisplay.setBorder ({{BColors::grey, 1.0}, 0.0, 3.0, 0.0});
	fileNameBox.setCallbackFunction (BEvents::MESSAGE_EVENT, filenameEnteredCallback);
	add (patternDisplay);
	add (noFileLabel);
}

PatternChooser::PatternChooser (const PatternChooser& that) :
	FileChooser (that), 
	Pattern (that),
	patternDisplay (that.patternDisplay), 
	noFileLabel (that.noFileLabel),
	patternValid (that.patternValid),
	additionalData (that.additionalData)
{
	add (patternDisplay);
	add (noFileLabel);
}

PatternChooser& PatternChooser::operator= (const PatternChooser& that)
{
	release (&patternDisplay);
	release (&noFileLabel);

	patternDisplay = that.patternDisplay;
	noFileLabel = that.noFileLabel;
	patternValid = that.patternValid;
	additionalData = that.additionalData;
	Pattern::operator= (that);
	FileChooser::operator= (that);

	add (patternDisplay);
	add (noFileLabel);

	return *this;
}

BWidgets::Widget* PatternChooser::clone () const {return new PatternChooser (*this);}

bool PatternChooser::isValid () const {return patternValid;}

std::string PatternChooser::getAdditionalData() const {return additionalData;}

void PatternChooser::setFileName (const std::string& filename)
{
	if (filename != fileNameBox.getText())
	{
		FileChooser::setFileName (filename);
		clear();
		patternValid = false;
		std::string newPath = getPath() + "/" + filename;
		char buf[PATH_MAX];
		char *rp = realpath(newPath.c_str(), buf);
		std::ifstream file (rp);
		if (file.good())
		{
			std::string text = "";
			std::string line;
			while (getline (file, line)) text += line;

			// Check header
			size_t pos = text.find ("appliesTo:");
			const size_t epos = text.find ("Additional data:");
			const std::string uri = BOOPS_URI;
			if (pos != std::string::npos)
			{
				pos = text.find ("<", pos + 1);
				if (pos != std::string::npos)
				{
					if (text.substr (pos + 1, uri.size()) == uri)
					{
						pos += uri.size() + 1;
						if (text.substr (pos, 1) == ">")
						{
							// Parse file
							pos += 1;
							fromString (text.substr (pos, epos - pos), std::array<std::string, 5> {"sl", "st", "gt", "sz", "mx"});
							patternValid = true;
						}
					}
				}
			}

			if (epos != std::string::npos) additionalData = text.substr (epos + 16);
			else additionalData = "";

			// Close file
			if (file.is_open()) file.close();
		}

		update();
	}
}

void PatternChooser::resize () {resize (BWIDGETS_DEFAULT_PATTERNCHOOSER_WIDTH, BWIDGETS_DEFAULT_PATTERNCHOOSER_HEIGHT);}

void PatternChooser::resize (const double width, const double height)  {resize (BUtilities::Point (width, height));}

void PatternChooser::resize (const BUtilities::Point extends) {Widget::resize (extends);}

void PatternChooser::update ()
{
	double x0 = getXOffset();
	double y0 = getYOffset();
	double w = getEffectiveWidth();
	double h = getEffectiveHeight();

	if ((w >= 40) && (h >= 20))
	{
		double val = fileListBox.getValue();
		if ((val == UNSELECTED) || (val > dirs.size())) okButton.getLabel()->setText (labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX]);
		else okButton.getLabel()->setText (labels[BWIDGETS_DEFAULT_FILECHOOSER_OPEN_INDEX]);
		cancelButton.getLabel()->setText(labels[BWIDGETS_DEFAULT_FILECHOOSER_CANCEL_INDEX]);

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
		pathNameBox.resize (w - pathNameHeight - 30, pathNameHeight);

		newFolderButton.moveTo (x0 + w - 10 - pathNameHeight, y0 + 10);
		newFolderButton.resize (pathNameHeight, pathNameHeight);

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
			const double fileListBoxHeight = h - pathNameHeight - okHeight - fileNameHeight - 50;

			fileListBox.moveTo (x0 + 10, y0 + pathNameHeight + 20);
			fileListBox.resize (0.4 * w - 15, fileListBoxHeight);
			fileListBox.resizeItems (BUtilities::Point (fileListBox.getEffectiveWidth(), 20));
			fileListBox.show();

			if (patternValid)
			{
				noFileLabel.hide();
				patternDisplay.moveTo (x0 + 0.4 * w + 5, y0 + pathNameHeight + 20);
				patternDisplay.resize (0.6 * w - 15, fileListBoxHeight);
				drawPattern();
				patternDisplay.show();
			}

			else
			{
				patternDisplay.hide();
				noFileLabel.resize ();
				noFileLabel.moveTo
				(
					x0 + 0.4 * w + 5 + 0.3 * w - 7.5 - 0.5 * noFileLabel.getWidth(),
					y0 + pathNameHeight + 20 + 0.5 * fileListBoxHeight - 0.5 * noFileLabel.getHeight()
				);
				noFileLabel.show();
			}
		}
		else
		{
			fileListBox.hide();
			patternDisplay.hide();
			noFileLabel.hide();
		}

		confirmLabel.resize();
		const double confirmBoxWidth = (confirmLabel.getWidth() + 40 > 2 * okWidth + 60 ? confirmLabel.getWidth() + 40 : 2 * okWidth + 60);
		const double confirmBoxHeight = confirmLabel.getHeight() + okHeight + 60;
		confirmBox.resize (confirmBoxWidth, confirmBoxHeight);
		confirmBox.moveTo (0.5 * getWidth() - 0.5 * confirmBoxWidth, 0.5 * getHeight() - 0.5 * confirmBoxHeight);
		confirmLabel.moveTo (20, 20);
		confirmCancelButton.moveTo (0.5 * confirmBoxWidth - okWidth - 10, confirmBoxHeight - okHeight - 10);
		confirmCancelButton.resize (okWidth, okHeight);
		confirmOkButton.moveTo (0.5 * confirmBoxWidth + 10, confirmBoxHeight - okHeight - 10);
		confirmOkButton.resize (okWidth, okHeight);
		confirmBox.show();

		createLabel.resize();
		createInput.resize();
		createError.resize();
		const double createLabelsWidth = (createLabel.getWidth() > createError.getWidth() ? createLabel.getWidth() : createError.getWidth());
		const double createBoxWidth = (createLabelsWidth + 40 > 2 * okWidth + 100 ? createLabelsWidth + 40 : 2 * okWidth + 100);
		const double createBoxHeight = createLabel.getHeight() + createInput.getHeight() + createError.getHeight() + okHeight + 60;
		createBox.resize (createBoxWidth, createBoxHeight);
		createBox.moveTo (0.5 * getWidth() - 0.5 * createBoxWidth, 0.5 * getHeight() - 0.5 * createBoxHeight);
		createLabel.moveTo (20, 20);
		createInput.resize (createBoxWidth - 40, createInput.getHeight());
		createInput.moveTo (20, 30 + createLabel.getHeight());
		createError.moveTo (20, 40 + createLabel.getHeight() + createInput.getHeight());
		createCancelButton.moveTo (0.5 * createBoxWidth - okWidth - 10, createBoxHeight - okHeight - 10);
		createCancelButton.resize (okWidth, okHeight);
		createOkButton.moveTo (0.5 * createBoxWidth + 10, createBoxHeight - okHeight - 10);
		createOkButton.resize (okWidth, okHeight);
		createBox.show();
	}

	else
	{
		okButton.hide();
		cancelButton.hide();
		fileListBox.hide();
		patternDisplay.hide();
		noFileLabel.hide();
		fileNameLabel.hide();
		fileNameBox.hide();
		filterPopupListBox.hide ();
		confirmBox.hide();
		createBox.hide();
	}

	Widget::update();
}

void PatternChooser::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void PatternChooser::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	FileChooser::applyTheme (theme, name);
	patternDisplay.applyTheme (theme, name + "/textbox");
	noFileLabel.applyTheme (theme, name + "/label");
}

void PatternChooser::sfileListBoxClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	BWidgets::ListBox* w = (BWidgets::ListBox*)event->getWidget();
	if (!w) return;
	PatternChooser* fc = (PatternChooser*)w->getParent();
	if (!fc) return;

	double val = w->getValue();

	if ((val != UNSELECTED) && (!fc->fileNameBox.getEditMode()))
	{
		// Directory selected -> one click chdir
		if (val <= fc->dirs.size())
		{
			fc->fileNameBox.setText ("");
			fc->patternValid = false;
			BEvents::ValueChangedEvent dummyEvent = BEvents::ValueChangedEvent (&fc->okButton, 1.0);
			fc->noFileLabel.setText (fc->labels[BWIDGETS_DEFAULT_PATTERNCHOOSER_NO_FILE_INDEX]);
			fc->okButtonClickedCallback (&dummyEvent);
			fc->update();
		}

		// File selected
		else
		{
			BItems::Item* ai = w->getActiveItem();
			if (ai)
			{
				BWidgets::Label* ail = (BWidgets::Label*)ai->getWidget();
				if (ail) fc->setFileName (ail->getText());
			}
		}
	}
}

void PatternChooser::filenameEnteredCallback (BEvents::Event* event)
{
	if (!event) return;
	if (!event->getWidget()) return;
	BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
	PatternChooser* p = (PatternChooser*)l->getParent();
	if (!p) return;

	const std::string s = l->getText();
	l->setText ("");
	p->setFileName (s);
}

void PatternChooser::drawPattern()
{
	const double x0 = patternDisplay.getXOffset();
	const double y0 = patternDisplay.getYOffset();
	const double w = patternDisplay.getEffectiveWidth();
	const double h = patternDisplay.getEffectiveHeight();

	cairo_surface_clear (patternDisplay.getDrawingSurface());
	cairo_t* cr = cairo_create (patternDisplay.getDrawingSurface ());
	if (cr && cairo_status (cr) == CAIRO_STATUS_SUCCESS)
	{
		if (patternValid && (w >= 1.0))
		{
			const double pw = 0.9 * w;
			const double ph = NR_SLOTS * w / NR_STEPS;
			for (int r = 0; r < NR_SLOTS; ++r)
			{
				for (int s = 0; s < NR_STEPS; )
				{
					Pad pad = getPad (r, s);
					if (pad.size >= 1.0)
					{
						int size = (s + pad.size <= NR_STEPS ? pad.size : NR_STEPS - s);
						cairo_rectangle 
						(
							cr, 
							x0 + 0.5 * w - 0.5 * pw + s * pw / NR_STEPS, 
							y0 + 0.5 * h - 0.5 * ph + r * ph / NR_SLOTS,
							size * pw / NR_STEPS,
							ph / NR_SLOTS
						);
						cairo_set_line_width (cr, 1.0);
						cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
						cairo_fill_preserve (cr);
						cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.75);
						cairo_stroke (cr);
						s += pad.size;
					}

					else s++;
				}
			}
			
		}

		cairo_destroy (cr);
	}
	//patternDisplay.update ();
}

std::function<void (BEvents::Event*)> PatternChooser::getFileListBoxClickedCallback()
{
	return sfileListBoxClickedCallback;
}
