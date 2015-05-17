#ifndef LSQFITTER_HPP
#define LSQFITTER_HPP

#include "./matrix44.hpp"

class LSQFitter {
	public:
		LSQFitter() {}
		~LSQFitter() {}

		void Init(const vec4&, const vec4&, unsigned int);
		void Solve(vec4& c) const;
		float Evaluate(const float t, const vec4& c) const;

	private:
		int k;			// degree of the polynomial

		vec4 a;			// the first components of the datapoints (eg. x)
		vec4 b;			// the second components of the datapoints (eg. y)

		matrix44 A;		// the initial matrix
};

#endif
