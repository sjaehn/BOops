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

#ifndef KEYS_HPP_
#define KEYS_HPP_

#include "StaticArrayList.hpp"
#include "MidiKey.hpp"

template<std::size_t sz> class Keys : protected StaticArrayList<MidiKey, sz>
{
public:
    Keys () : StaticArrayList<MidiKey, sz> () {}

    // Stores a MIDI_ON signal in the list. Newest first.
    void startNote (const MidiKey& key)
    {
        MidiKey** it;
        for (it = begin(); (it < end()) && (key.position >= (**it).position); ++it) {}
        MidiKey k = key;
        k.msg = 0x90 | (k.msg & 0x0F);
        insert (it, key);
    }

    // Stores a MIDI_OFF signal in the list. Newest first.
    void endNote  (const MidiKey& key)
    {
        MidiKey** it;
        for (it = begin(); (it < end()) && (key.position >= (**it).position); ++it) {}
        MidiKey k = key;
        k.msg = 0x80 | (k.msg & 0x0F);
        insert (it, key);
    }

    // Stores a 0x0 signal in the list. Newest first.
    void removeNote (const MidiKey& key)
    {
        MidiKey** it;
        for (it = begin(); (it < end()) && (key.position >= (**it).position); ++it) {}
        MidiKey k = key;
        k.msg &= 0x0F;
        insert (it, key);
    }

    // Removes all outdated signals
    void clean (const double position)
    {
        for (MidiKey** it1 = begin(); it1 < end(); ++it1)
        {
            if (position >= (**it1).position)
            {
                 for (MidiKey** it2 = it1 + 1; it2 < end(); )
                 {
                     if ((**it2).note == ((**it1).note)) it2 = erase (it2);
                     else ++it2
                 }
            }
        }
    }

    // Empties list
    void reset () {clear ();}

    // Returns the MIDI msg byte
    uint8_t getStatus (const uint8_t note, const double position)
    {
        for (MidiKey** it = begin(); it < end(); ++it) 
        {
            if (((**it).note == note) && (position >= (**it).position)) return (**it).msg;
        }

        return 0;
    }
};

#endif /* KEYS_HPP_ */