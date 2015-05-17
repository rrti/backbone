#ifndef INTERPOLATORS_HPP
#define INTERPOLATORS_HPP

struct LinearInterpolator {
	float Interpolate(float y0, float y1, float mu) {
		return (y0 * (1.0f - mu) + y1 * mu);
	}
};

struct HermiteInterpolator {
	/*
	Tension:
		 1 is high
		 0 is normal
		-1 is low
	Bias:
		 0 is even
		>0 is towards first segment
		<0 is towards the other
	*/
	float Interpolate(float y0, float y1, float y2, float y3, float mu, float tension, float bias) {
		float m0, m1, mu2, mu3;
		float a0, a1, a2, a3;

		mu2 = mu * mu;
		mu3 = mu2 * mu;
		m0  = (y1 - y0) * (1.0f + bias) * (1.0f - tension) * 0.5f;
		m0 += (y2 - y1) * (1.0f - bias) * (1.0f - tension) * 0.5f;
		m1  = (y2 - y1) * (1.0f + bias) * (1.0f - tension) * 0.5f;
		m1 += (y3 - y2) * (1.0f - bias) * (1.0f - tension) * 0.5f;

		// the Hermite basis functions
		a0 =  2.0f * mu3 - 3.0f * mu2 + 1.0f;
		a1 =         mu3 - 2.0f * mu2 + mu;
		a2 =         mu3 -        mu2;
		a3 = -2.0f * mu3 + 3.0f * mu2;

		return (a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
	}
};

#endif
