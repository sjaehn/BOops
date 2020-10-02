#ifndef PADMESSAGE_HPP_
#define PADMESSAGE_HPP_

#include "Pad.hpp"

#define ENDPADMESSAGE -1.0, 0.0, Pad()

struct PadMessage : Pad
{
	PadMessage () : PadMessage (ENDPADMESSAGE) {}
	PadMessage (float row, float step, Pad pad) :
		Pad (pad), row (row), step (step) {}
	bool operator== (PadMessage& that) {return (Pad::operator== (that) && (step == that.step) && (row == that.row));}

	float row;
	float step;
};



#endif /* PADMESSAGE_HPP_ */
