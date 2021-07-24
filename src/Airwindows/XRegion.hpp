/* ========================================
 *  XRegion - XRegion.h
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

#ifndef XREGION_HPP_
#define XREGION_HPP_

#include <cstdint>
#include <cmath>


class XRegion
{
public:
    XRegion (const double rate);
    XRegion (const XRegion& that) = delete;
    ~XRegion();
    XRegion& operator= (const XRegion& that) = delete;

    void process (float* input1, float* input2, float* output1, float* output2, int32_t sampleFrames);
	float* getParameters ();
    void setParameters (const float* values);

private:
    double rate;
    float biquad[15];
	float biquadA[15];
	float biquadB[15];
	float biquadC[15];
	float biquadD[15];
	uint32_t fpdL;
	uint32_t fpdR;
    float params[6];
};

#endif /* XREGION_HPP_ */
