/* ========================================
 *  Galactic - Galactic.h
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

#ifndef Galactic_HPP
#define Galactic_HPP

#include <cstring>
#include <cmath>
#include <cstdint>

class Galactic
{
public:
	Galactic ();
	Galactic (const double rate, const float replace, const float brightness, const float detune, const float bigness, const float mix);
	Galactic (const Galactic& that) = delete;
	Galactic& operator= (const Galactic& that) = delete;

	void process (const float* input1, const float* input2, float* output1, float* output2, int32_t sampleFrames);
	float getParameter (size_t index);                   // get the parameter value at the specified index
	void setParameter (size_t index, float value);       // set the parameter at index to value

private:
	double iirAL;
	double iirBL;

	double aIL[6480];
	double aJL[3660];
	double aKL[1720];
	double aLL[680];

	double aAL[9700];
	double aBL[6000];
	double aCL[2320];
	double aDL[940];

	double aEL[15220];
	double aFL[8460];
	double aGL[4540];
	double aHL[3200];

	double aML[3111];
	double aMR[3111];
	double vibML, vibMR, depthM, oldfpd;

	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;

	double lastRefL[7];
	double thunderL;

	double iirAR;
	double iirBR;

	double aIR[6480];
	double aJR[3660];
	double aKR[1720];
	double aLR[680];

	double aAR[9700];
	double aBR[6000];
	double aCR[2320];
	double aDR[940];

	double aER[15220];
	double aFR[8460];
	double aGR[4540];
	double aHR[3200];

	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;

	double lastRefR[7];
	double thunderR;

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
	int cycle; //all these ints are shared across channels, not duplicated

	double vibM;

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

	double samplerate;
	float params[5];

};

#endif
