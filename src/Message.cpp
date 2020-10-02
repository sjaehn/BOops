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

#include "Message.hpp"

Message::Message () : messageBits (0), scheduled (true) {}

void Message::clearMessages ()
{
	messageBits = 0;
	scheduled = true;
}

void Message::setMessage (MessageNr messageNr)
{
	if ((messageNr > NO_MSG) && (messageNr <= MAX_MSG) && (!isMessage (messageNr)))
	{
		messageBits = messageBits | (1 << (messageNr - 1));
		scheduled = true;
	}
}

void Message::deleteMessage (MessageNr messageNr)
{
	if ((messageNr > NO_MSG) && (messageNr <= MAX_MSG) && (isMessage (messageNr)))
	{
		messageBits = messageBits & (~(1 << (messageNr - 1)));
		scheduled = true;
	}
}

bool Message::isMessage (MessageNr messageNr)
{
	if ((messageNr > NO_MSG) && (messageNr <= MAX_MSG)) return ((messageBits & (1 << (messageNr - 1))) != 0);
	else if (messageNr == NO_MSG) return (messageBits == 0);
	else return false;
}

MessageNr Message::loadMessage ()
{
	scheduled = false;
	for (int i = NO_MSG + 1; i <= MAX_MSG; ++i)
	{
		MessageNr messageNr = MessageNr (i);
		if (isMessage (messageNr)) return messageNr;
	}
	return NO_MSG;
}

bool Message::isScheduled () {return scheduled;}
