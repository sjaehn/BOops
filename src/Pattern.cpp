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

#include "Pattern.hpp"

#ifndef LIMIT
#define LIMIT(val, min, max) (val < min ? min : (val < max ? val : max))
#endif /* LIMIT */

#include "BUtilities/to_string.hpp"
#include "BUtilities/stof.hpp"

void Pattern::clear ()
{
	Pad pad0 = Pad ();

	changes.oldMessage.clear ();
	changes.newMessage.clear ();
	journal.clear ();

	for (int r = 0; r < NR_SLOTS; ++r)
	{
		for (int s = 0; s < NR_STEPS; ++s)
		{
			setPad (r, s, pad0);
		}
	}

	store ();
}

Pad Pattern::getPad (const size_t row, const size_t step) const
{
	return pads[LIMIT (row, 0, NR_SLOTS)][LIMIT (step, 0, NR_STEPS)];
}
void Pattern::setPad (const size_t row, const size_t step, const Pad& pad)
{
	size_t r = LIMIT (row, 0, NR_SLOTS);
	size_t s = LIMIT (step, 0, NR_STEPS);
	changes.oldMessage.push_back (PadMessage (r, s, pads[r][s]));
	changes.newMessage.push_back (PadMessage (r, s, pad));
	pads[r][s] = pad;
}

std::vector<PadMessage> Pattern::undo ()
{
	store ();

	std::vector<PadMessage> padMessages = journal.undo ();
	std::reverse (padMessages.begin (), padMessages.end ());
	for (PadMessage const& p : padMessages)
	{
		size_t r = LIMIT (p.row, 0, NR_SLOTS);
		size_t s = LIMIT (p.step, 0, NR_STEPS);
		pads[r][s] = Pad (p);
	}

	return padMessages;
}

std::vector<PadMessage> Pattern::redo ()
{
	store ();

	std::vector<PadMessage> padMessages = journal.redo ();
	for (PadMessage const& p : padMessages)
	{
		size_t r = LIMIT (p.row, 0, NR_SLOTS);
		size_t s = LIMIT (p.step, 0, NR_STEPS);
		pads[r][s] = Pad (p);
	}

	return padMessages;
}

void Pattern::store ()
{
	if (changes.newMessage.empty ()) return;

	journal.push (changes.oldMessage, changes.newMessage);
	changes.oldMessage.clear ();
	changes.newMessage.clear ();
}

std::string Pattern::toString (const std::array<std::string, 2 + sizeof (Pad) / sizeof (float)>& symbols) const
{
        std::string text = "";

        for (size_t r = 0; r < pads.size(); ++r)
        {
                for (size_t s = 0; s < pads[r].size(); ++s)
                {
                        const Pad& p = pads[r][s];
                        if (p != Pad())
                        {
                                if (text != "") text += ";\n";
                                text += symbols[0] + ":" + std::to_string (r) + "; " + symbols[1] + ":" + std::to_string (s) + ";";
                                const float* values = (const float*)&p;
                                for (size_t i = 2; i < symbols.size(); ++i)
                                {
                                        text += " " + symbols[i] + ":" + BUtilities::to_string (values[i - 2], "%1.3f");
                                        if (i != symbols.size() - 1) text += ";";
                                }
                        }
                }
        }

        text += "\n";

        return text;
}

void Pattern::fromString (const std::string& text, const std::array<std::string, 2 + sizeof (Pad) / sizeof (float)>& symbols)
{
        // Clear old data
        for (int r = 0; r < NR_SLOTS; ++r)
	{
		for (int s = 0; s < NR_STEPS; ++s)
		{
			setPad (r, s, Pad());
		}
	}

        // Parse from string
        size_t pos = 0;
        while (pos < text.size())
        {
                // Look for mandatory row
                size_t r = -1;
                size_t strPos = text.find (symbols[0] + ":", pos);
                size_t nextPos = 0;
                if (strPos == std::string::npos) break;	// No row found => end
                if (strPos + symbols[0].size() + 1 > text.size()) break;	// Nothing more after row => end
                pos = strPos + symbols[0].size() + 1;
                try {r = BUtilities::stof (text.substr (pos, 63), &nextPos);}
                catch  (const std::exception& e)
                {
                        fprintf (stderr, "BOops.lv2#GUI: Import pattern incomplete. Can't parse row nr from \"%s...\"\n", text.substr (pos, 63).c_str());
                        break;
                }

                pos += nextPos;

                if (r >= pads.size())
                {
                        fprintf (stderr, "BOops.lv2#GUI: Import pattern incomplete. Invalid row nr %li.\n", r);
                        break;
                }

                // Look for mandatory step
                size_t s = -1;
                strPos = text.find (symbols[1] + ":", pos);
                nextPos = 0;
                if (strPos == std::string::npos) break;	// No step found => end
                if (strPos + symbols[1].size() + 1 > text.size()) break;	// Nothing more after step => end
                pos = strPos + symbols[1].size() + 1;
                try {s = BUtilities::stof (text.substr (pos, 63), &nextPos);}
                catch  (const std::exception& e)
                {
                        fprintf (stderr, "BOops.lv2#GUI: Import pattern incomplete. Can't parse step nr from \"%s...\"\n", text.substr (pos, 63).c_str());
                        break;
                }

                pos += nextPos;
                if (s >= pads[r].size())
                {
                        fprintf (stderr, "BOops.lv2#GUI: Import pattern incomplete. Invalid step nr %li.\n", s);
                        break;
                }

                // Look for pad data
                Pad p = Pad();
                float* padValues = (float*)&p;
                for (size_t i = 2; i < symbols.size(); ++i)
                {
                        nextPos = 0;
                        strPos = text.find (symbols[i] + ":", pos);
                        if (strPos == std::string::npos) continue;	// Keyword not found => next keyword
                        if (strPos + symbols[i].size() + 1 > text.size())	// Nothing more after keyword => end
                        {
                                pos = std::string::npos;
                                break;
                        }
                        pos = strPos + symbols[0].size() + 1;
                        float val = 0;
                        try {val = BUtilities::stof (text.substr (pos, 63), &nextPos);}
                        catch  (const std::exception& e)
                        {
                                fprintf (stderr, "BOops.lv2#GUI: Import pattern incomplete. Can't parse %s from \"%s...\"\n",
                                                 symbols[i].c_str(), text.substr (pos, 63).c_str());
                                break;
                        }

                        switch (i)
                        {
                                case 2:	val = LIMIT (val, 0, 1);
                                        break;

                                case 3:	val = LIMIT (val, 1, NR_STEPS - s);
                                        break;

                                case 4:	val = LIMIT (val, 0, 1);
                                        break;

                                default:break;
                        }

                        pos += nextPos;

                        padValues[i - 2] = val;
                }

                setPad (r, s, p);
        }

        store ();
}
