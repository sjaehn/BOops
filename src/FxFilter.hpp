/* B.Noname01
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

#ifndef FXFILTER_HPP_
#define FXFILTER_HPP_

#include "Fx.hpp"
#include "ButterworthBandPassFilter.hpp"

#define FX_FILTER_LOW 0
#define FX_FILTER_LOWRAND 1
#define FX_FILTER_HIGH 2
#define FX_FILTER_HIGHRAND 3
#define FX_FILTER_ORDER 4

class BNoname01; // Forward declaration

class FxFilter : public Fx
{
public:
	FxFilter ();
	FxFilter (RingBuffer<Stereo>** buffer, float* params, Pad* pads, BNoname01* ui);

	virtual void start (const double position) override;
	virtual Stereo play (const double position) override;

protected:
	BNoname01* ui;
	ButterworthBandPassFilter filter;
};

#endif /* FXFILTER_HPP_ */
