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

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <cstdint>
#include "MessageDefinitions.hpp"

class Message
{
public:
	Message ();
	void clearMessages ();
	void setMessage (MessageNr messageNr);
	void deleteMessage (MessageNr messageNr);
	bool isMessage (MessageNr messageNr);
	MessageNr loadMessage ();
	bool isScheduled ();
private:
	uint32_t messageBits;
	bool scheduled;
};

#endif /* MESSAGE_HPP_*/
