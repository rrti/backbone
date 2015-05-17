#ifndef FASTMATH_HPP
#define FASTMATH_HPP

#include "./Constants.hpp"

namespace fastmath {
	// returns (1.0f / sqrt(x))
	inline float isqrt(float x) {
		float xh = 0.5f * x;
		int i = *(int*) &x;
		i = 0x5f375a86 - (i >> 1);
		x = *(float*) &i;
		x = x * (1.5f - xh * (x * x));
		return x;
	}

	inline float isqrt2(float x) {
		float xh = 0.5f * x;
		int i = *(int*) &x;
		i = 0x5f375a86 - (i >> 1);
		x = *(float*) &i;
		x = x * (1.5f - xh * (x * x));
		x = x * (1.5f - xh * (x * x));
		return x;
	}

	inline float sqrt(float x) { return (isqrt(x) * x); }
	inline float sqrt2(float x) { return (isqrt2(x) * x); }



	inline float sin(float x) {
		x = x - ((int) (x * INVPI2)) * PI2;
		if (x > HALFPI) {
			x = -x + PI;
		} else if (x < NEGHALFPI ) {
			x = -x - PI;
		}

		x = (PIU4) * x + (PISUN4) * x * fabsf(x);
		x = 0.225 * (x * fabsf(x) - x) + x;
		return x;
	}

	inline float cos(float x) {
		return sin(x + HALFPI);
	}
}

#endif
