/* FileChooser.cpp
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

#include "FileChooser.hpp"
#include <dirent.h>

namespace BWidgets
{
FileChooser::FileChooser () : FileChooser (0.0, 0.0, 0.0, 0.0, "FileChooser") {}

FileChooser::FileChooser (const double x, const double y, const double width, const double height, const std::string& name) :
		FileChooser (x, y, width, height, name, ".", {}, "OK") {}

FileChooser::FileChooser (const double x, const double y, const double width, const double height, const std::string& name,
			  const std::string& path) :
		FileChooser (x, y, width, height, name, path, {}, "OK") {}

FileChooser::FileChooser (const double x, const double y, const double width, const double height, const std::string& name,
			  const std::string& path, const std::vector<FileFilter>& filters) :
		FileChooser (x, y, width, height, name, path, filters, "OK") {}

FileChooser::FileChooser (const double x, const double y, const double width, const double height, const std::string& name,
			  const std::string& path, const std::vector<FileFilter>& filters, const std::string& buttonText) :
		FileChooser (x, y, width, height, name, path, filters, std::vector<std::string>{"OK"}) {}

FileChooser::FileChooser (const double x, const double y, const double width, const double height, const std::string& name,
     			  const std::string& path, const std::vector<FileFilter>& filters, const std::vector<std::string>& texts) :
		ValueWidget (x, y, width, height, name, 0.0),
		filters (filters),
		dirs (),
		files (),
		labels ({"OK", "Open", "Cancel"}),
		bgColors (BWIDGETS_DEFAULT_BGCOLORS),
		pathNameBox (0, 0, 0, 0, name + "/textbox", ""),
		fileListBox (0, 0, 0, 0, name + "/listbox"),
		fileNameLabel (0, 0, 0, 0, name + "/label", "File:"),
		fileNameBox (0, 0, 0, 0, name + "/textbox", ""),
		filterPopupListBox (),
		cancelButton (0, 0, 0, 0, name + "/button", "Cancel"),
		okButton (0, 0, 0, 0, name + "/button", labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX]),
		fileListBoxFileLabel (0, 0, 0, 0, name + "/listbox/item/file", ""),
		fileListBoxDirLabel (0, 0, 0, 0, name + "/listbox/item/dir", ""),
		filterPopupListBoxFilterLabel (0, 0, 0, 0, name + "/popup/listbox/item", "")
		//fileFont ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0, BStyles::TEXT_ALIGN_LEFT, BStyles::TEXT_VALIGN_MIDDLE),
		//dirFont ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 12.0, BStyles::TEXT_ALIGN_LEFT, BStyles::TEXT_VALIGN_MIDDLE),
		//filterFont ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0, BStyles::TEXT_ALIGN_LEFT, BStyles::TEXT_VALIGN_MIDDLE)
{
	for (int i = 0; (i < int(texts.size())) && (i < int(labels.size())); ++i) labels[i] = texts[i];
	okButton.getLabel()->setText (labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX]);
	background_ = BWIDGETS_DEFAULT_MENU_BACKGROUND;
	border_ = BWIDGETS_DEFAULT_MENU_BORDER;
	setDraggable (true);

	setPath (path);
	enterDir();

	pathNameBox.getFont()->setTextAlign (BStyles::TEXT_ALIGN_LEFT);
	pathNameBox.setBackground (BWIDGETS_DEFAULT_MENU_BACKGROUND);
	pathNameBox.setBorder ({{BColors::grey, 1.0}, 0.0, 3.0, 0.0});

	fileNameLabel.getFont()->setTextAlign (BStyles::TEXT_ALIGN_LEFT);

	fileNameBox.setEditable (true);
	fileNameBox.getFont()->setTextAlign (BStyles::TEXT_ALIGN_LEFT);
	fileNameBox.setBackground (BWIDGETS_DEFAULT_MENU_BACKGROUND);
	fileNameBox.setBorder ({{BColors::grey, 1.0}, 0.0, 3.0, 0.0});

	fileListBox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, fileListBoxClickedCallback);

	cancelButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, cancelButtonClickedCallback);
	okButton.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, okButtonClickedCallback);

	BItems::ItemList items;
	for (FileFilter const& f : filters)
	{
		items.push_back ("");
		Label* label = (Label*)items.back().getWidget();
		if (label)
		{
			*label = filterPopupListBoxFilterLabel;
			label->setText (f.name);
		}
	}
	filterPopupListBox = PopupListBox (0, 0, 0, 0, 0, 0, name + "/popup", items, (items.size() > 0 ? 1.0 : 0.0));
	filterPopupListBox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, filterPopupListBoxClickedCallback);

	add (pathNameBox);
	add (fileListBox);
	add (fileNameLabel);
	add (fileNameBox);
	add (cancelButton);
	add (okButton);
	add (filterPopupListBox);
}

FileChooser::FileChooser (const FileChooser& that) :
	ValueWidget (that),
	filters (that.filters),
	dirs (that.dirs),
	files (that.files),
	labels (that.labels),
	bgColors (that.bgColors),
	pathNameBox (that.pathNameBox),
	fileListBox (that.fileListBox),
	fileNameLabel (that.fileNameLabel),
	fileNameBox (that.fileNameBox),
	filterPopupListBox (that.filterPopupListBox),
	cancelButton (that.cancelButton),
	okButton (that.okButton),
	fileListBoxFileLabel (that.fileListBoxFileLabel),
	fileListBoxDirLabel (that.fileListBoxDirLabel),
	filterPopupListBoxFilterLabel (that.filterPopupListBoxFilterLabel)
{
	add (pathNameBox);
	add (fileListBox);
	add (fileNameLabel);
	add (fileNameBox);
	add (cancelButton);
	add (okButton);
	add (filterPopupListBox);
}

FileChooser& FileChooser::operator= (const FileChooser& that)
{
	filters = that.filters;
	dirs = that.dirs;
	files = that.files;
	labels = that.labels;
	bgColors = that.bgColors;
	//fileFont = that.fileFont;
	//dirFont = that.dirFont;
	//filterFont = that.filterFont;
	pathNameBox = that.pathNameBox;
	fileListBox = that.fileListBox;
	fileNameLabel = that.fileNameLabel;
	fileNameBox = that.fileNameBox;
	filterPopupListBox = that.filterPopupListBox;
	cancelButton = that.cancelButton;
	okButton = that.okButton;
	fileListBoxFileLabel = that.fileListBoxFileLabel;
	fileListBoxDirLabel = that.fileListBoxDirLabel;
	filterPopupListBoxFilterLabel = that.filterPopupListBoxFilterLabel;
	ValueWidget::operator= (that);
	return *this;
}

Widget* FileChooser::clone () const {return new FileChooser (*this);}

void FileChooser::setPath (const std::string& path)
{
	if (path != pathNameBox.getText())
	{
		char buf[PATH_MAX];
		char *rp = realpath(path.c_str(), buf);
		if (rp) pathNameBox.setText (rp);
		else pathNameBox.setText (path);

		update();
	}
}

std::string FileChooser::getPath () const {return pathNameBox.getText();}

void FileChooser::setFileName (const std::string& filename)
{
	if (filename != fileNameBox.getText())
	{
		fileNameBox.setText (filename);
		BItems::ItemList* il = fileListBox.getItemList();
		if (!il) return;
		for (BItems::Item const& it : *il)
		{
			if (it.getWidget())
			{
				BWidgets::Label* l = (BWidgets::Label*)it.getWidget();
				if (l->getText() == filename)
				{
					fileListBox.setValue (it.getValue());
					break;
				}
			}
		}
	}
}

std::string FileChooser::getFileName () const {return fileNameBox.getText();}

void FileChooser::setFilters (const std::vector<FileFilter>& filters)
{
	this->filters = filters;

	BItems::ItemList items;
	for (FileFilter const& f : filters)
	{
		items.push_back ("");
		Label* label = (Label*)items.back().getWidget();
		if (label)
		{
			*label = filterPopupListBoxFilterLabel;
			label->setText (f.name);
		}
	}
	filterPopupListBox = PopupListBox (0, 0, 0, 0, 0, 0, getName() + "/popup", items, (items.size() > 0 ? 1.0 : 0.0));
	filterPopupListBox.setCallbackFunction (BEvents::VALUE_CHANGED_EVENT, filterPopupListBoxClickedCallback);

	enterDir();
	update ();
}

std::vector<FileFilter> FileChooser::getFilters () const {return filters;}

void FileChooser::selectFilter (const std::string& name)
{
	int select = 1;
	for (FileFilter const& f : filters)
	{
		if (f.name == name)
		{
			filterPopupListBox.setValue (select);
			enterDir();
			update();
			break;
		}
		++select;
	}
}

void FileChooser::setButtonText (const std::string& buttonText)
{
	if (buttonText != labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX])
	{
		labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX] = buttonText;
		update();
	}
}

std::string FileChooser::getButtonText () {return labels[BWIDGETS_DEFAULT_FILECHOOSER_OK_INDEX];}

void FileChooser::setLabels (const std::vector<std::string>& texts)
{
	for (int i = 0; (i < int(texts.size())) && (i < int(labels.size())); ++i) labels[i] = texts[i];
	update();
}

std::vector<std::string> FileChooser::getLabels () const {return labels;}

// TODO calculate minimal size
void FileChooser::resize () {resize (BWIDGETS_DEFAULT_FILECHOOSER_WIDTH, BWIDGETS_DEFAULT_FILECHOOSER_HEIGHT);}

void FileChooser::resize (const double width, const double height) {resize (BUtilities::Point (width, height));}

void FileChooser::resize (const BUtilities::Point extends) {Widget::resize (extends);}

void FileChooser::update ()
{
	double x0 = getXOffset();
	double y0 = getYOffset();
	double w = getEffectiveWidth();
	double h = getEffectiveHeight();

	if ((w >= 20) && (h >= 20))
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

		if (h > pathNameHeight + okHeight + fileNameHeight + 50)
		{
			fileListBox.moveTo (x0 + 10, y0 + pathNameHeight + 20);
			fileListBox.resize (w - 20, h - pathNameHeight - okHeight - fileNameHeight - 50);
			fileListBox.resizeItems (BUtilities::Point (fileListBox.getEffectiveWidth(), 20));
			fileListBox.show();
		}
		else fileListBox.hide();
	}

	else
	{
		okButton.hide();
		cancelButton.hide();
		fileListBox.hide();
		fileNameLabel.hide();
		fileNameBox.hide();
		filterPopupListBox.hide ();
	}

	Widget::update();
}

void FileChooser::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void FileChooser::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);
	pathNameBox.applyTheme (theme, name + "/textbox");
	fileNameLabel.applyTheme (theme, name + "/label");
	fileNameBox.applyTheme (theme, name + "/textbox");
	cancelButton.applyTheme (theme, name + "/button");
	okButton.applyTheme (theme, name + "/button");
	fileListBox.applyTheme (theme, name + "/listbox");
	filterPopupListBox.applyTheme (theme, name + "/popup");
	fileListBoxFileLabel.applyTheme (theme, name + "/listbox/item/file");
	fileListBoxDirLabel.applyTheme (theme, name + "/listbox/item/dir");
	filterPopupListBoxFilterLabel.applyTheme (theme, name + "/popup/listbox/item");

	BItems::ItemList* il = fileListBox.getItemList();
	if (il)
	{
		for (BItems::Item const& i : *il)
		{
			Label* l = (Label*)i.getWidget();
			if (l)
			{
				if (l->getName() == name + "/listbox/item/file") l->applyTheme (theme, name + "/listbox/item/file");
				else if (l->getName() == name + "/listbox/item/dir") l->applyTheme (theme, name + "/listbox/item/dir");
			}
		}
	}

	// Color
	void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
	if (bgPtr) bgColors = *((BColors::ColorSet*) bgPtr);

	// TODO Store dir and file styles

	if (bgPtr) update ();
}

void FileChooser::fileListBoxClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	ListBox* w = (ListBox*)event->getWidget();
	if (!w) return;
	FileChooser* fc = (FileChooser*)w->getParent();
	if (!fc) return;

	double val = w->getValue();

	if ((val != UNSELECTED) && (!fc->fileNameBox.getEditMode()))
	{
		// Directory selected -> one click chdir
		if (val <= fc->dirs.size())
		{
			fc->fileNameBox.setText ("");
			BEvents::ValueChangedEvent dummyEvent = BEvents::ValueChangedEvent (&fc->okButton, 1.0);
			fc->okButtonClickedCallback (&dummyEvent);
		}

		// File selected
		else
		{
			BItems::Item* ai = w->getActiveItem();
			if (ai)
			{
				Label* ail = (Label*)ai->getWidget();
				if (ail) fc->setFileName (ail->getText());
			}
		}

		fc->update();
	}
}

void FileChooser::filterPopupListBoxClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	PopupListBox* w = (PopupListBox*)event->getWidget();
	if (!w) return;
	FileChooser* fc = (FileChooser*)w->getParent();
	if (!fc) return;

	fc->enterDir();
	fc->update();
}

void FileChooser::cancelButtonClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	TextButton* w = (TextButton*)event->getWidget();
	if (!w) return;
	FileChooser* fc = (FileChooser*)w->getParent();
	if (!fc) return;

	BEvents::ValueChangedEvent* ev = (BEvents::ValueChangedEvent*)event;
	if (ev->getValue() == 1.0)
	{
		fc->setValue (0.0);
		fc->postCloseRequest();
	}
}

void FileChooser::okButtonClickedCallback (BEvents::Event* event)
{
	if (!event) return;
	TextButton* w = (TextButton*)event->getWidget();
	if (!w) return;
	FileChooser* fc = (FileChooser*)w->getParent();
	if (!fc) return;

	BEvents::ValueChangedEvent* ev = (BEvents::ValueChangedEvent*)event;
	if (ev->getValue() == 1.0)
	{
		double lb = fc->fileListBox.getValue();

		if (lb != UNSELECTED)
		{
			// Dircectory selected: Open directory
			if (lb <= fc->dirs.size())
			{
				BItems::Item* ai = fc->fileListBox.getActiveItem();
				if (ai)
				{
					Label* ail = (Label*)ai->getWidget();
					if (ail)
					{
						std::string newPath = fc->getPath() + "/" + ail->getText();
						char buf[PATH_MAX];
				    		char *rp = realpath(newPath.c_str(), buf);
				    		if (rp) fc->setPath (rp);

						fc->enterDir();
						fc->update();
					}
				}
			}

			// File selected: OK on file
			else
			{
				fc->setValue (1.0);
				fc->postCloseRequest();
			}
		}

		// File name set: OK on file
		else if (fc->fileNameBox.getText() != "")
		{
			fc->setValue (1.0);
			fc->postCloseRequest();
		}
	}
}

void FileChooser::enterDir ()
{
	std::vector<std::string> newFiles;
	std::vector<std::string> newDirs;
	DIR *dir = opendir (getPath().c_str());

	// Scan directory
	// TODO Use C++17 for future versions
	if (dir)
	{
		for (struct dirent* entry = readdir(dir); entry ; entry = readdir(dir))
		{
			if (entry->d_type == DT_DIR)
			{
				std::string s = entry->d_name;
				if ((std::regex_match (s, std::regex ("(\\.{1,2})|([^\\.].*)"))))	// Exclude hidden
				{
					newDirs.push_back (entry->d_name);
				}
			}

			else
			{
				std::string s = entry->d_name;
				if (!std::regex_match (s, std::regex ("\\..*")))	// Exclude hidden
				{
					int filterNr = LIMIT ((filterPopupListBox.getValue() - 1), 0, int (filters.size() - 1));

					if  (filters.size() != 0)
					{
						if (std::regex_match (s, filters[filterNr].regex)) newFiles.push_back (s);
					}
					else newFiles.push_back (s);
				}
			}
		}
		closedir (dir);
	}

	std::sort (newFiles.begin(), newFiles.end());
	std::sort (newDirs.begin(), newDirs.end());

	if ((files != newFiles) || (dirs != newDirs))
	{
		files = newFiles;
		dirs = newDirs;

		fileListBox.removeItems();
		fileListBox.setValue (UNSELECTED);
		size_t count = 1;

		// Directories
		for (std::string const& d : dirs)
		{
			BItems::Item item = BItems::Item (count, "");
			Label* label = (Label*)item.getWidget();
			if (label)
			{
				*label = fileListBoxDirLabel;
				label->setText (d);
				fileListBox.addItem (item);
				++count;
			}
		}

		// Files
		for (std::string const& f : files)
		{
			BItems::Item item = BItems::Item (count, "");
			Label* label = (Label*)item.getWidget();
			if (label)
			{
				*label = fileListBoxFileLabel;
				label->setText (f);
				fileListBox.addItem (item);
				++count;
			}
		}

		fileListBox.setTop (1);
	}
}

std::function<void (BEvents::Event*)> FileChooser::getFileListBoxClickedCallback()
{
	return fileListBoxClickedCallback;
}

}
