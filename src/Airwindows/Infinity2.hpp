/* ========================================
 *  Infinity2 - Infinity2.h
 *  Created 8/12/11 by SPIAdmin
 *  Copyright (c) 2011 __MyCompanyName__, All rights reserved
 * ======================================== */

/* MIT License

 Copyright (c) 2018 Chris Johnson
 Copyright (C) 2021 Sven Jähnichen

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/


#ifndef INFINITY2_HPP_
#define INFINITY2_HPP_



#include <cstring>
#include <cstdint>
#include <math.h>

class Infinity2
{
public:
	Infinity2();
	Infinity2 (const double rate, const float filter, const float size, const float damp, const float allpass, const float feedback, const float mix);
	Infinity2 (const Infinity2& that) = delete;
	Infinity2& operator= (const Infinity2& that) = delete;

	void process (const float* input1, const float* input2, float* output1, float* output2, int32_t sampleFrames);
	float getParameter (size_t index);                   // get the parameter value at the specified index
	void setParameter (size_t index, float value);       // set the parameter at index to value
private:
	long double biquadA[11];
	long double biquadB[11];
	long double biquadC[11];

	double aAL[8111];
	double aBL[7511];
	double aCL[7311];
	double aDL[6911];
	double aEL[6311];
	double aFL[6111];
	double aGL[5511];
	double aHL[4911];
	double aIL[4511];
	double aJL[4311];
	double aKL[3911];
	double aLL[3311];
	double aML[3111];

	double aAR[8111];
	double aBR[7511];
	double aCR[7311];
	double aDR[6911];
	double aER[6311];
	double aFR[6111];
	double aGR[5511];
	double aHR[4911];
	double aIR[4511];
	double aJR[4311];
	double aKR[3911];
	double aLR[3311];
	double aMR[3111];

	int countA, delayA;
	int countB, delayB;
	int countC, delayC;
	int countD, delayD;
	int countE, delayE;
	int countF, delayF;
	int countG, delayG;
	int countH, delayH;
	int countI, delayI;
	int countJ, delayJ;
	int countK, delayK;
	int countL, delayL;
	int countM, delayM;

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackEL;
	double feedbackFL;
	double feedbackGL;
	double feedbackHL;

	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	double feedbackER;
	double feedbackFR;
	double feedbackGR;
	double feedbackHR;

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

	double samplerate;
	float params[6];
};

#endif
