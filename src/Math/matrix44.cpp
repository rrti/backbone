#include "./matrix44.hpp"

matrix44::matrix44(void) {
	LoadIdentity();
}

matrix44::~matrix44(void) {
}


matrix44::matrix44(const vec3& pos, const vec3& x, const vec3& y, const vec3& z) {
	m[ 0] = x.x;   m[ 1] = x.y;   m[ 2] = x.z;   m[ 3] = 0.0f;
	m[ 4] = y.x;   m[ 5] = y.y;   m[ 6] = y.z;   m[ 7] = 0.0f;
	m[ 8] = z.x;   m[ 9] = z.y;   m[10] = z.z;   m[11] = 0.0f;
	m[12] = pos.x; m[13] = pos.y; m[14] = pos.z; m[15] = 1.0f;
}

matrix44::matrix44(const matrix44& n) {
	m[ 0] = n[ 0];  m[ 1] = n[ 1];  m[ 2] = n[ 2];  m[ 3] = n[ 3];
	m[ 4] = n[ 4];  m[ 5] = n[ 5];  m[ 6] = n[ 6];  m[ 7] = n[ 7];
	m[ 8] = n[ 8];  m[ 9] = n[ 9];  m[10] = n[10];  m[11] = n[11];
	m[12] = n[12];  m[13] = n[13];  m[14] = n[14];  m[15] = n[15];
}

void matrix44::LoadIdentity() {
	m[ 0] = m[ 5] = m[10] = m[15] = 1.0f;
	m[ 1] = m[ 2] = m[ 3] = 0.0f;
	m[ 4] = m[ 6] = m[ 7] = 0.0f;
	m[ 8] = m[ 9] = m[11] = 0.0f;
	m[12] = m[13] = m[14] = 0.0f;
}


void matrix44::RotateX(float rad) {
	const float sr = sin(rad);
	const float cr = cos(rad);
	float a = m[4];

	m[4] = cr * a - sr * m[8];
	m[8] = sr * a + cr * m[8];

	a = m[5];
	m[5] = cr * a - sr * m[9];
	m[9] = sr * a + cr * m[9];

	a = m[6];
	m[ 6] = cr * a - sr * m[10];
	m[10] = sr * a + cr * m[10];

	a = m[7];
	m[ 7] = cr * a - sr * m[11];
	m[11] = sr * a + cr * m[11];
}

void matrix44::RotateY(float rad) {
	const float sr = sin(rad);
	const float cr = cos(rad);
	float a = m[0];

	m[0] =  cr * a + sr * m[8];
	m[8] = -sr * a + cr * m[8];

	a = m[1];
	m[1] =  cr * a + sr * m[9];
	m[9] = -sr * a + cr * m[9];

	a = m[2];
	m[ 2] =  cr * a + sr * m[10];
	m[10] = -sr * a + cr * m[10];

	a = m[3];
	m[ 3] =  cr * a + sr * m[11];
	m[11] = -sr * a + cr * m[11];
}

void matrix44::RotateZ(float rad) {
	const float sr = sin(rad);
	const float cr = cos(rad);
	float a = m[0];

	m[0] = cr * a - sr * m[4];
	m[4] = sr * a + cr * m[4];

	a = m[1];
	m[1] = cr * a - sr * m[5];
	m[5] = sr * a + cr * m[5];

	a = m[2];
	m[2] = cr * a - sr * m[6];
	m[6] = sr * a + cr * m[6];

	a = m[3];
	m[3] = cr * a - sr * m[7];
	m[7] = sr * a + cr * m[7];
}


void matrix44::Translate(float x, float y, float z) {
	m[12] += x * m[0] + y * m[4] + z * m[ 8];
	m[13] += x * m[1] + y * m[5] + z * m[ 9];
	m[14] += x * m[2] + y * m[6] + z * m[10];
	m[15] += x * m[3] + y * m[7] + z * m[11];
}

void matrix44::Translate(const vec3& pos) {
	const float x = pos.x;
	const float y = pos.y;
	const float z = pos.z;
	m[12] += x * m[0] + y * m[4] + z * m[ 8];
	m[13] += x * m[1] + y * m[5] + z * m[ 9];
	m[14] += x * m[2] + y * m[6] + z * m[10];
	m[15] += x * m[3] + y * m[7] + z * m[11];
}


matrix44 matrix44::Mul(const matrix44& m2) const {
	matrix44 res;
	float m20, m21, m22, m23;

	for (int i = 0; i < 16; i += 4) {
		m20 = m2[i    ];
		m21 = m2[i + 1];
		m22 = m2[i + 2];
		m23 = m2[i + 3];

		res[i    ] = m[0] * m20 + m[4] * m21 + m[ 8] * m22 + m[12] * m23;
		res[i + 1] = m[1] * m20 + m[5] * m21 + m[ 9] * m22 + m[13] * m23;
		res[i + 2] = m[2] * m20 + m[6] * m21 + m[10] * m22 + m[14] * m23;
		res[i + 3] = m[3] * m20 + m[7] * m21 + m[11] * m22 + m[15] * m23;
	}

	return res;
}


vec3 matrix44::Mul(const vec3& v) const {
	// technically illegal to apply a matrix44 to a vec3,
	// but we only want the rotation part of the matrix
	// (upper-left 3x3 elements)
	const float x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + m[12];
	const float y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + m[13];
	const float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
	return vec3(x, y, z);
}

vec4 matrix44::Mul(const vec4& v) const {
	const float x = v.x * m[0] + v.y * m[4] + v.z * m[ 8] + v.w * m[12];
	const float y = v.x * m[1] + v.y * m[5] + v.z * m[ 9] + v.w * m[13];
	const float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + v.w * m[14];
	const float w = v.x * m[3] + v.y * m[7] + v.z * m[11] + v.w * m[15];
	return vec4(x, y, z, w);
}

void matrix44::SetUpVector(vec3& up) {
	vec3 zdir(m[8], m[9], m[10]);
	vec3 xdir(zdir.cross(up));

	xdir.norm();
	zdir = up.cross(xdir);

	m[ 0] = xdir.x;
	m[ 1] = xdir.y;
	m[ 2] = xdir.z;

	m[ 4] = up.x;
	m[ 5] = up.y;
	m[ 6] = up.z;

	m[ 8] = zdir.x;
	m[ 9] = zdir.y;
	m[10] = zdir.z;
}

// rotate <rad> radians around axis <axis>
void matrix44::Rotate(float rad, vec3& axis) {
	const float sr = sin(rad);
	const float cr = cos(rad);

	for (int a = 0; a < 3; ++a) {
		vec3 v(m[a * 4], m[a * 4 + 1], m[a * 4 + 2]);

		vec3 va(axis * v.dot3D(axis));
		vec3 vp(v - va);
		vec3 vp2(axis.cross(vp));

		vec3 vpnew(vp * cr + vp2 * sr);
		vec3 vnew(va + vpnew);

		m[a * 4    ] = vnew.x;
		m[a * 4 + 1] = vnew.y;
		m[a * 4 + 2] = vnew.z;
	}
}



matrix44& matrix44::TransposeInPlace() {
	float t = 0.0f;
	// first row <==> first col
	t = m[ 1]; m[ 1] = m[ 4]; m[ 4] = t;
	t = m[ 2]; m[ 2] = m[ 8]; m[ 8] = t;
	t = m[ 3]; m[ 3] = m[12]; m[12] = t;

	// second row <==> second col
	t = m[ 6]; m[ 6] = m[ 9]; m[ 9] = t;
	t = m[ 7]; m[ 7] = m[13]; m[13] = t;

	// third row <==> third col
	t = m[11]; m[11] = m[14]; m[14] = t;

	return *this;
}

matrix44 matrix44::Transpose() const {
	matrix44 trans(*this);

	// first row <==> first col
	trans[ 1] = m[ 4]; trans[ 4] = m[ 1];
	trans[ 2] = m[ 8]; trans[ 8] = m[ 2];
	trans[ 3] = m[ 3]; trans[12] = m[12];

	// second row <==> second col
	trans[ 6] = m[ 9]; trans[ 9] = m[ 6];
	trans[ 7] = m[13]; trans[13] = m[ 7];

	// third row <==> third col
	trans[11] = m[14]; trans[14] = m[11];

	return trans;
}



// note: assumes this matrix only
// does translation and rotation
matrix44& matrix44::InvertInPlace() {
	// transpose the rotation
	float t = 0.0f;
	t = m[1]; m[1] = m[4]; m[4] = t;
	t = m[2]; m[2] = m[8]; m[8] = t;
	t = m[6]; m[6] = m[9]; m[9] = t;

	// get the inverse translation
	vec3 tr(-m[12], -m[13], -m[14]);

	// do the actual inversion
	m[12] = tr.x * m[0] + tr.y * m[4] + tr.z * m[ 8];
	m[13] = tr.x * m[1] + tr.y * m[5] + tr.z * m[ 9];
	m[14] = tr.x * m[2] + tr.y * m[6] + tr.z * m[10];

	return *this;
}

// note: assumes this matrix only
// does translation and rotation;
// m.Mul(m.Invert()) is identity
matrix44 matrix44::Invert() const {
	matrix44 mInv(*this);

	// transpose the rotation
	mInv[1] = m[4]; mInv[4] = m[1];
	mInv[2] = m[8]; mInv[8] = m[2];
	mInv[6] = m[9]; mInv[9] = m[6];

	// get the inverse translation
	vec3 t(-m[12], -m[13], -m[14]);

	// do the actual inversion
	mInv[12] = t.x * mInv[0] + t.y * mInv[4] + t.z * mInv[ 8];
	mInv[13] = t.x * mInv[1] + t.y * mInv[5] + t.z * mInv[ 9];
	mInv[14] = t.x * mInv[2] + t.y * mInv[6] + t.z * mInv[10];

	return mInv;
}






int matrix44::Pivot(int row, float mrows[4][4], float* rmptrs[4]) const {
	const int nrows = 4;
	int k = row;
	float max = -1.0f;
	float* rptr = 0x0;

	for (int i = row; i < nrows; i++) {
		// NOTE: [i][row] ????
		float t = fabsf(mrows[i][row]);

		if (t > max && t != 0.0f) {
			max = t;
			k = i;
		}
	}

	// NOTE: [k][row] ????
	if (mrows[k][row] == 0.0f) {
		return -1;
	}

	if (k != row) {
		// swap rows <k> and <row>
		rptr        = rmptrs[k  ];
		rmptrs[k  ] = rmptrs[row];
		rmptrs[row] = rptr;
		return k;
	}

	return 0;
}

// special-case implementation of the generic
// inversion algorithm for our 4x4 matrices
// (which are always square and non-singular)
matrix44 matrix44::Inv() const {
	matrix44 mInv;
	const int nrows = 4;

	// copy the current matrix
	float mrows[nrows][nrows] = {
		{m[0], m[4], m[ 8], m[12]},		// 1st row
		{m[1], m[5], m[ 9], m[13]},		// 2nd row
		{m[2], m[6], m[10], m[14]},		// 3rd row
		{m[3], m[7], m[11], m[15]}		// 4th row
	};

	// prepare storage for the inverted matrix
	float irows[nrows][nrows] = {
		{mInv.m[0], mInv.m[4], mInv.m[ 8], mInv.m[12]},
		{mInv.m[1], mInv.m[5], mInv.m[ 9], mInv.m[13]},
		{mInv.m[2], mInv.m[6], mInv.m[10], mInv.m[14]},
		{mInv.m[3], mInv.m[7], mInv.m[11], mInv.m[15]}
	};

	// arrays of pointers to each row in <mrows> and <irows>
	float* rmptrs[nrows] = {mrows[0], mrows[1], mrows[2], mrows[3]};
	float* riptrs[nrows] = {irows[0], irows[1], irows[2], irows[3]};
	// temporary row pointer
	float* riptr = 0x0;

	for (int row = 0; row < nrows; row++) {
		int idx = Pivot(row,   mrows, rmptrs);

		if (idx == -1) {
			// error: matrix is singular
			// printf("[matrix44::Inv()] error: singular matrix\n");
			return mInv;
		}

		if (idx != 0) {
			// "swap" rows <row> and <idx> of the to-be
			// inverted matrix by exchanging their row
			// pointers (so that riptrs[row] points to
			// a different row after the swap)
			riptr       = riptrs[row];
			riptrs[row] = riptrs[idx];
			riptrs[idx] = riptr;
		}

		float a1 = rmptrs[row][row];

		for (int j = 0; j < nrows; j++) {
			rmptrs[row][j] /= a1;
			riptrs[row][j] /= a1;
		}

		for (int i = 0; i < nrows; i++)
			if (i != row) {
				float a2 = rmptrs[i][row];

				for (int j = 0; j < nrows; j++) {
					rmptrs[i][j] -= a2 * rmptrs[row][j];
					riptrs[i][j] -= a2 * riptrs[row][j];
				}
			}
	}

	// copy the elements of irows to mInv
	// 1st column
	mInv.m[ 0] = riptrs[0][0];
	mInv.m[ 1] = riptrs[1][0];
	mInv.m[ 2] = riptrs[2][0];
	mInv.m[ 3] = riptrs[3][0];

	// 2nd column
	mInv.m[ 4] = riptrs[0][1];
	mInv.m[ 5] = riptrs[1][1];
	mInv.m[ 6] = riptrs[2][1];
	mInv.m[ 7] = riptrs[3][1];

	// 3rd column
	mInv.m[ 8] = riptrs[0][2];
	mInv.m[ 9] = riptrs[1][2];
	mInv.m[10] = riptrs[2][2];
	mInv.m[11] = riptrs[3][2];

	// 4th column
	mInv.m[12] = riptrs[0][3];
	mInv.m[13] = riptrs[1][3];
	mInv.m[14] = riptrs[2][3];
	mInv.m[15] = riptrs[3][3];

	return mInv;
}
