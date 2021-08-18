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

#ifndef PATTERNCHOOSER_HPP_
#define PATTERNCHOOSER_HPP_

#include "BWidgets/FileChooser.hpp"
#include "BWidgets/DrawingSurface.hpp"
#include "Pattern.hpp"
#define BWIDGETS_DEFAULT_PATTERNCHOOSER_WIDTH 800
#define BWIDGETS_DEFAULT_PATTERNCHOOSER_HEIGHT 320
#define BWIDGETS_DEFAULT_PATTERNCHOOSER_FILTERS std::regex (".*\\.pat$", std::regex_constants::icase)
#define BWIDGETS_DEFAULT_PATTERNCHOOSER_NO_FILE_INDEX 7

class PatternChooser : public BWidgets::FileChooser, public Pattern
{
public:
	PatternChooser ();
	PatternChooser (const double x, const double y, const double width, const double height, const std::string& name);
	PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       		const std::string& path);
	PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       		const std::string& path, const std::vector<BWidgets::FileFilter>& filters);
	PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       		const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::string& buttonText);
	PatternChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       		const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::vector<std::string>& texts);
	PatternChooser (const PatternChooser& that);

	PatternChooser& operator= (const PatternChooser& that);
	virtual Widget* clone () const override;

	virtual void setFileName (const std::string& filename) override;
	bool isValid () const;
	std::string getAdditionalData() const;

	virtual void resize () override;
	virtual void resize (const double width, const double height) override;
	virtual void resize (const BUtilities::Point extends) override;
	virtual void update () override;
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;
	static void sfileListBoxClickedCallback (BEvents::Event* event);
	static void filenameEnteredCallback (BEvents::Event* event);

protected:
	BWidgets::DrawingSurface patternDisplay;
	BWidgets::Label noFileLabel;
	bool patternValid;
	std::string additionalData;
	std::array<bool, NR_SLOTS> ads;

	void drawPattern();
	virtual std::function<void (BEvents::Event*)> getFileListBoxClickedCallback() override;
};

#endif /* PATTERNCHOOSER_HPP_ */
