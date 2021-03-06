#ifndef RNG_HPP
#define RNG_HPP

#include <cstdlib>
#define FRAND_MAX float(RAND_MAX)

struct RNG {
	RNG() {
		srand(time(0));
	}

	// random float in range [0, range]
	// (rand() can return RAND_MAX)
	float RandFloat(float range = 1.0f) const {
		range = (range < 0.0f)? -range: range;
		return ((rand() / FRAND_MAX) * range);
	}
	// random int in range [0, range]
	// (rand() can return RAND_MAX)
	int RandInt(int range = 1) const {
		range = (range < 0)? -range: range;
		return int((rand() / FRAND_MAX) * range);
	}
};

// static, so srand() only called once
const static RNG rng;

#endif
