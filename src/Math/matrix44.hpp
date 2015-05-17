#ifndef MATRIX44_HPP
#define MATRIX44_HPP

#include "./vec3.hpp"
// translate a (row, col) index to 1D in column-major order
#define GetIdx1D(i, j) (i + (j * 4))

struct matrix44 {
	matrix44(void);
	matrix44(const vec3& pos, const vec3& x, const vec3& y, const vec3& z);
	matrix44(const matrix44& n);
	~matrix44(void);

	void LoadIdentity();

	inline float& operator[](int a) { return m[a]; }
	inline float operator[](int a) const { return m[a]; }
	void operator = (const matrix44& other) { for (int a = 0; a < 16; ++a) m[a] = other[a]; }

	void RotateX(float rad);
	void RotateY(float rad);
	void RotateZ(float rad);
	void Rotate(float rad, vec3& axis);
	void Translate(float x, float y, float z);
	matrix44 Mul(const matrix44& other) const;

	matrix44& TransposeInPlace();
	matrix44 Transpose() const;
	matrix44& InvertInPlace();
	matrix44 Invert() const;

	int Pivot(int, float [4][4], float* [4]) const;
	matrix44 Inv() const;

	vec3 Mul(const vec3&) const;
	vec4 Mul(const vec4&) const;

	inline vec3 GetPos(void) {
		return vec3(m[12], m[13], m[14]);
	}
	inline vec3 GetDir(int i) const {
		if (i == 0) { return vec3(m[ 0], m[ 1], m[ 2]); } // rotated x-axis
		if (i == 1) { return vec3(m[ 4], m[ 5], m[ 6]); } // rotated y-axis
		if (i == 2) { return vec3(m[ 8], m[ 9], m[10]); } // rotated z-axis
		if (i == 3) { return vec3(m[12], m[13], m[14]); } // translation vector
		return NVec;
	}

	// OpenGL ordered (ie. column-major)
	float m[16];

	void SetUpVector(vec3& up);
	void Translate(const vec3& pos);

	/*
	void PrintMe() {
		printf("%f\t%f\t%f\t%f\n", m[0], m[4], m[ 8], m[12]);
		printf("%f\t%f\t%f\t%f\n", m[1], m[5], m[ 9], m[13]);
		printf("%f\t%f\t%f\t%f\n", m[2], m[6], m[10], m[14]);
		printf("%f\t%f\t%f\t%f\n", m[3], m[7], m[11], m[15]);
	}
	*/
};

#endif
