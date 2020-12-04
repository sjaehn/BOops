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

#ifndef SAMPLECHOOSER_HPP_
#define SAMPLECHOOSER_HPP_

#include "BWidgets/FileChooser.hpp"
#include "BWidgets/DrawingSurface.hpp"
#include "HRangeScrollbar.hpp"
#include "VLine.hpp"
#include "Checkbox.hpp"
#define BWIDGETS_DEFAULT_SAMPLECHOOSER_WIDTH 800
#define BWIDGETS_DEFAULT_SAMPLECHOOSER_HEIGHT 320
#define BWIDGETS_DEFAULT_SAMPLECHOOSER_FILTERS std::regex (".*\\.((wav)|(wave)|(aif)|(aiff)|(au)|(sd2)|(flac)|(caf)|(ogg)|(mp3))$", std::regex_constants::icase)

class Sample;	// Forward declaration

class SampleChooser : public BWidgets::FileChooser
{
public:
	SampleChooser ();
	SampleChooser (const double x, const double y, const double width, const double height, const std::string& name);
	SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       const std::string& path);
	SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       const std::string& path, const std::vector<BWidgets::FileFilter>& filters);
	SampleChooser (const double x, const double y, const double width, const double height, const std::string& name,
		       const std::string& path, const std::vector<BWidgets::FileFilter>& filters, const std::string& buttonText);
	SampleChooser (const SampleChooser& that);
	~SampleChooser();

	SampleChooser& operator= (const SampleChooser& that);
	virtual Widget* clone () const override;

	int64_t getStart() const;
	int64_t getEnd() const;
	bool getLoop() const;

	virtual void resize () override;
	virtual void resize (const double width, const double height) override;
	virtual void resize (const BUtilities::Point extends) override;
	virtual void update () override;
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;
	static void sfileListBoxClickedCallback (BEvents::Event* event);
	static void scrollbarChangedCallback (BEvents::Event* event);
	static void lineDraggedCallback (BEvents::Event* event);

protected:
	BWidgets::DrawingSurface waveform;
	HRangeScrollbar scrollbar;
	VLine startMarker;
	VLine endMarker;
	BWidgets::Label sizeLabel;
	BWidgets::Label startLabel;
	BWidgets::Label endLabel;
	Checkbox loopCheckbox;
	BWidgets::Label loopLabel;

	Sample* sample;

	void drawWaveform();
	virtual std::function<void (BEvents::Event*)> getFileListBoxClickedCallback() override;
};

#endif /* SAMPLECHOOSER_HPP_ */
