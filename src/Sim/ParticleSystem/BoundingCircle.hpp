#ifndef BOUNDINGCIRCLE_HPP
#define BOUNDINGCIRCLE_HPP

#include <vector>
#include "../../Math/Trig.hpp"
#include "../../Math/vec3.hpp"
#include "../../Math/matrix44.hpp"

// "tunnel-slice" (circle) centered at (0, 0) in XY-plane
// (after inverse-transforming the interpolated slice)
struct BoundingCircle {
	BoundingCircle(const vec3& pos, const vec3& zAxis, int nSegs = 4, float rad = 1.0f) {
		m = GetTransformMatrix(pos, zAxis);
		n = m.Invert();

		numSegments = nSegs;
		radius = rad;
		segmentAngle = 360.0f / numSegments;

		segmentNormals.resize(numSegments);
		segmentDirects.resize(numSegments);

		// create the vertices counter-clockwise
		// (eg. 0, 45, 90, 135, ...) starting at
		// (1, 0) in slice-local space
		for (int i = 0; i < numSegments; i++) {
			const float a1 = DTOR((i    ) * segmentAngle);
			const float a2 = DTOR((i + 1) * segmentAngle);

			const float x1 = radius * cosf(a1);
			const float y1 = radius * sinf(a1);
			const float x2 = radius * cosf(a2);
			const float y2 = radius * sinf(a2);

			vertices.push_back(vec3(x1, y1, 0.0f));

			// cross with (0, 0, 1) so we get the proper vector in XY-space
			// note that the normals face outward while we want them to face
			// inward for our force calculations, so we flip each one
			segmentDirects[i] = (vec3(x2, y2) - vec3(x1, y1)).norm();
			segmentNormals[i] = -segmentDirects[i].cross(ZVec);
		}
	}


	// we need this to convert a particle's angle
	// with the positive x-axis as given by atan2
	// to a segment index, segments are numbered
	// COUNTER-clockwise starting at 0 at (1, 0)
	//
	// pass in the particle's pos after multiplying
	// by the slice's inverse transformation matrix
	float GetAngle(float x, float y) const {
		float a = RTOD(atan2(y, x));
		float r = 0.0f;

		if (a >= 0.0f) {
			r = a;
		} else {
			r = a + 360.0f;
		}

		return r;
	}

	int GetSegmentIndex(float angle) const {
		return int(angle / segmentAngle);
	}

	float GetSegmentDistance(const vec3& pos, int segment) const {
		// last segment's second vertex is first vertex of first
		// (segment <s> runs from vertices[s] to vertices[s + 1])
		const float x2 = vertices[(segment + 1) % numSegments].x;
		const float y2 = vertices[(segment + 1) % numSegments].y;
		const float x1 = vertices[segment                    ].x;
		const float y1 = vertices[segment                    ].y;
		const float nn = fabsf((x2 - x1) * (y1 - pos.y) - (x1 - pos.x) * (y2 - y1));
		const float dd = sqrtf(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))) + 0.1f;
		return (nn / dd);
	}

	// get the force exerted on a particle in
	// the inverse-transformed slice and then
	// transform it back to world-space
	vec3 GetForce(const vec3& invPos) const {
		const float r = invPos.sqLen3D();

		if (r > (radius * radius) /*|| r < (radius * radius * 0.25f)*/) {
			// z = 0, so sqLen3D() only looks at 2D xy-distance
			return NVec;
		} else {
			const float a = GetAngle(invPos.x, invPos.y);
			const int seg = GetSegmentIndex(a);

			// get the world-space normal vector of this segment
			const vec3& v1 = vertices[(seg    )                  ];
			const vec3& v2 = vertices[(seg + 1) % vertices.size()];
			const vec3  v3 = (v1 + v2) * 0.5f;
			const vec3  v4 = v3 + segmentNormals[seg];
			const vec3  nv  = m.Mul(v4) - m.Mul(v3);

			// get orthogonal distance to segment edge
			// defined by vertices <s> and <s + 1>
			const float dist = GetSegmentDistance(invPos, seg);
			const float scalar = (radius * 2.0f) / (dist + 0.1f);

			return (nv * scalar);
		}
	}

	matrix44 GetTransformMatrix(const vec3& pos, const vec3& nor) {
		// transform a tunnel slice at world-coors <pos>
		// and with world-coors normal <nor> to XY-plane
		// centered at (0, 0, 0), note that we do -YVec
		// so local x-vector points along negative world
		// x-axis
		vec3 xv;
		vec3 yv;
		vec3 zv;

		const float a = RTOD(acos(nor.dot3D(YVec)));
		const bool b = (nor.y >= 0.0f);

		// both <nor> and <YVec> have length 1, so angle
		// between them is just equal to acos(nor, YVec)
		//
		// if path segment goes UP, angle will be close
		// to 0 degrees whereas if segment goes DOWN the
		// angle will be close to 180
		//
		// if ((nor.x > -EPSILON && nor.x < EPSILON) && (nor.z > -EPSILON && nor.z < EPSILON)) {
		//	 xv = -XVec;
		//	 yv =  (nor.y >= 0.0f)? -ZVec:  ZVec;
		//	 zv =  (nor.y >= 0.0f)?  YVec: -YVec;
		// } ...
		//
		// if (a < 45.0f || a > 135.0f) {
		//		xv = (b)? nor.cross(-ZVec).norm(): nor.cross(  ZVec).norm();
		//		yv = (b)? nor.cross(-xv  ).norm(): nor.cross(  xv  ).norm();
		//		zv = (b)? nor.cross(-yv  ).norm(): nor.cross(  yv  ).norm();
		// } ...

		if (a < 2.0f || a > 178.0f) {
			 xv = -XVec;
			 yv = (b)? -ZVec:  ZVec;
			 zv = (b)?  YVec: -YVec;
		} else {
			xv = nor.cross(-YVec).norm();
			yv = nor.cross( xv  ).norm();
			zv = nor;
		}

		matrix44 m(pos, xv, yv, zv);
		return m;
	}

	// get the position of a particle within
	// the inverse-transformed tunnel slice
	vec3 GetParticleInvPos(const vec3& pos) const {
		vec3 p = n.Mul(pos); p.z = 0.0f;
		return p;
	}

	int numSegments;					// equal for every tunnel-slice
	float segmentAngle;					// in degrees, dito
	float radius;						// variable per slice
	std::vector<vec3> vertices;			// vertex xy-coors
	std::vector<vec3> segmentNormals;	// inward-facing (radius-invariant)
	std::vector<vec3> segmentDirects;	// counter-clockwise (radius-invariant)
	matrix44 m;
	matrix44 n;
};

#endif
