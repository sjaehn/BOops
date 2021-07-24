/* ========================================
 *  XRegion - XRegion.h
 *  Copyright (c) 2016 airwindows, All rights reserved
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

#include "XRegion.hpp"
#include <cstring>

XRegion::XRegion (const double rate) :
    rate (rate),
    params {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f}
{
	for (int x = 0; x < 15; x++) {biquad[x] = 0.0f; biquadA[x] = 0.0f; biquadB[x] = 0.0f; biquadC[x] = 0.0f; biquadD[x] = 0.0f;}
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
}

XRegion::~XRegion() {}

void XRegion::process (float* input1, float* input2, float* output1, float* output2, int32_t sampleFrames)
{
    float gain = powf (params[0] + 0.5f, 4);
	
	float high = params[1];
	float low = params[2];
	float mid = (high + low) * 0.5f;
	float spread = 1.001f - fabsf (high - low);
    float nuke = params[3];
	
	biquad[0] = high * high * high *20000.0f / rate;
	if (biquad[0] < 0.00009f) biquad[0] = 0.00009f;
	float compensation = sqrtf (biquad[0]) * 6.4f * spread;
	float clipFactor = 0.75f + (biquad[0] * nuke * 37.0f);
	
    const float hm = 0.5f * (high + mid);
	biquadA[0] = hm * hm * hm * 20000.0f / rate;
	if (biquadA[0] < 0.00009f) biquadA[0] = 0.00009f;
	float compensationA = sqrtf (biquadA[0]) * 6.4f * spread;
	float clipFactorA = 0.75f + (biquadA[0] * nuke * 37.0f);
	
	biquadB[0] = mid * mid * mid *20000.0f / rate;
	if (biquadB[0] < 0.00009f) biquadB[0] = 0.00009f;
	float compensationB = sqrtf (biquadB[0]) * 6.4f * spread;
	float clipFactorB = 0.75f + (biquadB[0] * nuke * 37.0f);
	
    const float ml = 0.5f * (mid + low);
	biquadC[0] = ml * ml * ml * 20000.0f / rate;
	if (biquadC[0] < 0.00009f) biquadC[0] = 0.00009f;
	float compensationC = sqrtf (biquadC[0]) * 6.4f * spread;
	float clipFactorC = 0.75f + (biquadC[0] * nuke * 37.0f);
	
	biquadD[0] = low * low * low * 20000.0f / rate;
	if (biquadD[0] < 0.00009f) biquadD[0] = 0.00009f;
	float compensationD = sqrtf (biquadD[0]) * 6.4f * spread;
	float clipFactorD = 0.75f + (biquadD[0] * nuke * 37.0f);
	
	float K = tanf (M_PI * biquad[0]);
	float norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquad[2] = K / 0.7071f * norm;
	biquad[4] = -biquad[2];
	biquad[5] = 2.0f * (K * K - 1.0f) * norm;
	biquad[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tanf (M_PI * biquadA[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadA[2] = K / 0.7071f * norm;
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tanf (M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadB[2] = K / 0.7071f * norm;
	biquadB[4] = -biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tanf (M_PI * biquadC[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadC[2] = K / 0.7071f * norm;
	biquadC[4] = -biquadC[2];
	biquadC[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadC[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	K = tanf (M_PI * biquadD[0]);
	norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadD[2] = K / 0.7071f * norm;
	biquadD[4] = -biquadD[2];
	biquadD[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadD[6] = (1.0f - K / 0.7071f + K * K) * norm;	
	
	float aWet = 1.0f;
	float bWet = 1.0f;
	float cWet = 1.0f;
	float dWet = params[3] * 4.0f;
	float wet = params[4];
    float pan = params[5];
	
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
	else {dWet -= 3.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	float outSample = 0.0f;
	
    while (--sampleFrames >= 0)
    {
		float inputSampleL = *input1;
		float inputSampleR = *input2;
		if (fabsf(inputSampleL)<1.18e-37) inputSampleL = fpdL * 1.18e-37;
		if (fabsf(inputSampleR)<1.18e-37) inputSampleR = fpdR * 1.18e-37;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		if (gain != 1.0f) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		}
		
		float nukeLevelL = inputSampleL;
		float nukeLevelR = inputSampleR;
		
		inputSampleL *= clipFactor;
		if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
		if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
		inputSampleL = sinf(inputSampleL);
		outSample = biquad[2]*inputSampleL+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSampleL; biquad[10] = biquad[9];
		biquad[9] = outSample; //DF1 left
		inputSampleL = outSample / compensation; nukeLevelL = inputSampleL;
		
		inputSampleR *= clipFactor;
		if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
		if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
		inputSampleR = sinf(inputSampleR);
		outSample = biquad[2]*inputSampleR+biquad[4]*biquad[12]-biquad[5]*biquad[13]-biquad[6]*biquad[14];
		biquad[12] = biquad[11]; biquad[11] = inputSampleR; biquad[14] = biquad[13];
		biquad[13] = outSample; //DF1 right
		inputSampleR = outSample / compensation; nukeLevelR = inputSampleR;
		
		if (aWet > 0.0) {
			inputSampleL *= clipFactorA;
			if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
			if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
			inputSampleL = sinf(inputSampleL);
			outSample = biquadA[2]*inputSampleL+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSampleL; biquadA[10] = biquadA[9];
			biquadA[9] = outSample; //DF1 left
			inputSampleL = outSample / compensationA; inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0f-aWet));
			nukeLevelL = inputSampleL;
			
			inputSampleR *= clipFactorA;
			if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
			if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
			inputSampleR = sinf(inputSampleR);
			outSample = biquadA[2]*inputSampleR+biquadA[4]*biquadA[12]-biquadA[5]*biquadA[13]-biquadA[6]*biquadA[14];
			biquadA[12] = biquadA[11]; biquadA[11] = inputSampleR; biquadA[14] = biquadA[13];
			biquadA[13] = outSample; //DF1 right
			inputSampleR = outSample / compensationA; inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0f-aWet));
			nukeLevelR = inputSampleR;
		}
		if (bWet > 0.0) {
			inputSampleL *= clipFactorB;
			if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
			if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
			inputSampleL = sinf(inputSampleL);
			outSample = biquadB[2]*inputSampleL+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSampleL; biquadB[10] = biquadB[9];
			biquadB[9] = outSample; //DF1 left
			inputSampleL = outSample / compensationB; inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0f-bWet));
			nukeLevelL = inputSampleL;
			
			inputSampleR *= clipFactorB;
			if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
			if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
			inputSampleR = sinf(inputSampleR);
			outSample = biquadB[2]*inputSampleR+biquadB[4]*biquadB[12]-biquadB[5]*biquadB[13]-biquadB[6]*biquadB[14];
			biquadB[12] = biquadB[11]; biquadB[11] = inputSampleR; biquadB[14] = biquadB[13];
			biquadB[13] = outSample; //DF1 right
			inputSampleR = outSample / compensationB; inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0f-bWet));
			nukeLevelR = inputSampleR;
		}
		if (cWet > 0.0) {
			inputSampleL *= clipFactorC;
			if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
			if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
			inputSampleL = sinf(inputSampleL);
			outSample = biquadC[2]*inputSampleL+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSampleL; biquadC[10] = biquadC[9];
			biquadC[9] = outSample; //DF1 left
			inputSampleL = outSample / compensationC; inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0f-cWet));
			nukeLevelL = inputSampleL;
			
			inputSampleR *= clipFactorC;
			if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
			if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
			inputSampleR = sinf(inputSampleR);
			outSample = biquadC[2]*inputSampleR+biquadC[4]*biquadC[12]-biquadC[5]*biquadC[13]-biquadC[6]*biquadC[14];
			biquadC[12] = biquadC[11]; biquadC[11] = inputSampleR; biquadC[14] = biquadC[13];
			biquadC[13] = outSample; //DF1 right
			inputSampleR = outSample / compensationC; inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0f-cWet));
			nukeLevelR = inputSampleR;
		}
		if (dWet > 0.0) {
			inputSampleL *= clipFactorD;
			if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
			if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
			inputSampleL = sinf(inputSampleL);
			outSample = biquadD[2]*inputSampleL+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSampleL; biquadD[10] = biquadD[9];
			biquadD[9] = outSample; //DF1 left
			inputSampleL = outSample / compensationD; inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0f-dWet));
			nukeLevelL = inputSampleL;
			
			inputSampleR *= clipFactorD;
			if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
			if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
			inputSampleR = sinf(inputSampleR);
			outSample = biquadD[2]*inputSampleR+biquadD[4]*biquadD[12]-biquadD[5]*biquadD[13]-biquadD[6]*biquadD[14];
			biquadD[12] = biquadD[11]; biquadD[11] = inputSampleR; biquadD[14] = biquadD[13];
			biquadD[13] = outSample; //DF1 right
			inputSampleR = outSample / compensationD; inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0f-dWet));
			nukeLevelR = inputSampleR;
		}
		
		if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
		if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
		inputSampleL = sinf(inputSampleL);
		if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
		if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;
		inputSampleR = sinf(inputSampleR);
		
		if (wet < 1.0f) {
			inputSampleL = (drySampleL * (1.0f-wet))+(inputSampleL * wet);
			inputSampleR = (drySampleR * (1.0f-wet))+(inputSampleR * wet);
		}
		
		*output1 = inputSampleL * (1.0f - (pan > 0.0f) * pan);
		*output2 = inputSampleR * (1.0f + (pan < 0.0f) * pan);

		input1++;
		input2++;
		output1++;
		output2++;
    }
}

void XRegion::setParameters (const float* values) 
{
    memcpy (params, values, 6 * sizeof (float));
}

float* XRegion::getParameters () {return params;}
