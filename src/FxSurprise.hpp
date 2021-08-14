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

#ifndef FXSURPRISE_HPP_
#define FXSURPRISE_HPP_

#include "Fx.hpp"
#include "BOops.hpp"
#include <array>

#define FX_SURPRISE_RATIO 0
#define FX_SURPRISE_SLOT 1
#define FX_SURPRISE_NR 6

class FxSurprise : public Fx
{
public:
	FxSurprise () = delete;

	FxSurprise (RingBuffer<Stereo>** buffer, float* params, Pad* pads, BOops* plugin) :
		Fx (buffer, params, pads), plugin (plugin), act (0)
	{
		if (!plugin) throw std::invalid_argument ("Fx initialized with plugin nullptr");
		ratios.fill (0);
		slots.fill (-1);
	}

	virtual void init (const double position) override
	{
		Fx::init (position);

		float sr = 0;
		for (int i = 0; i < FX_SURPRISE_NR; ++i)
		{
			ratios[i] = params[SLOTS_OPTPARAMS + i * 2 +  FX_SURPRISE_RATIO];
			slots[i] = LIMIT (16.0 * params[SLOTS_OPTPARAMS + i * 2 +  FX_SURPRISE_SLOT], 0.0, 12.0) - 1.0;
			if (slots[i] >= 0) sr += ratios[i];
		}

		act = -1;

		if (sr)
		{
			float r = unidist (rnd) * sr;
			act = 0;
			sr = 0;
			for (int i = 0; i < FX_SURPRISE_NR; ++i)
			{
				if (slots[i] >= 0) sr += ratios[i];
				if (sr >= r)
				{
					act = i;
					break;
				}
			}
		}
	}

	virtual Stereo playPad (const double position, const double size, const double mixf) override
	{
		if (playing && pads && plugin)
		{
			for (int i = 0; i < FX_SURPRISE_NR; ++i)
			{
				if (slots[i] >= 0) plugin->slots[slots[i]].mixf = (i == act ? adsr (position, size) : 0);
			}
		}

		return (**buffer).front();
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf) override
	{
		if (playing && plugin)
		{
			if (shapePaused && (mx >= 0.0001)) init (position);
			shapePaused = (mx < 0.0001);

			for (int i = 0; i < FX_SURPRISE_NR; ++i)
			{
				if (slots[i] >= 0) plugin->slots[slots[i]].mixf = (i == act ? mx : 0);
			}
		}

		return (**buffer).front();
	}

protected:
	BOops* plugin;
	std::array<float, FX_SURPRISE_NR> ratios;
	std::array<int, FX_SURPRISE_NR> slots;
	int act;
};

#endif /* FXSURPRISE_HPP_ */
