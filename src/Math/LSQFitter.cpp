#include "./LSQFitter.hpp"

void LSQFitter::Init(const vec4& a, const vec4& b, unsigned int k) {
	this->a = a;
	this->b = b;
	this->k = k;

	// TODO: needs to be dynamic for (n x m) matrices and degree-k polynomials
	A[ 0] = 1.0f;  A[ 4] = a.x;  A[ 8] = a.x * a.x;  A[12] = A[ 8] * a.x;
	A[ 1] = 1.0f;  A[ 5] = a.y;  A[ 9] = a.y * a.y;  A[13] = A[ 9] * a.y;
	A[ 2] = 1.0f;  A[ 6] = a.z;  A[10] = a.z * a.z;  A[14] = A[10] * a.z;
	A[ 3] = 1.0f;  A[ 7] = a.w;  A[11] = a.w * a.w;  A[15] = A[11] * a.w;
}

void LSQFitter::Solve(vec4& c) const {
	matrix44 AT		= A.Transpose();
	matrix44 ATAInv	= (AT.Mul(A)).Inv();
	c				= (ATAInv.Mul(AT)).Mul(b);
}

// TODO: needs to be dynamic for degree-k polynomials
// evaluate the 3rd-degree polynomial f(t)
// with coefficients <c> at the point <t>
float LSQFitter::Evaluate(const float t, const vec4& c) const {
	return (c.x + c.y * t + c.z * t*t + c.w * t*t*t);
}
