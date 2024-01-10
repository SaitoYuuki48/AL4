#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

inline float EaseInBack(float frame) { 
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	float ease = (float)c3 * frame * frame * frame - (float)c1 * frame * frame;
	return ease;
}

inline float EaseInSine(float frame) { 

	return 1 - cosf((frame * (float)M_PI) / 2); }