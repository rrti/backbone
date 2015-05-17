#ifndef TRIG_HDR
#define TRIG_HDR

#include "./vec3.hpp"

// convert degrees to radians and vice versa
#define DTOR(a) ((float) (a) * (3.141592653 / 180.0f))
#define RTOD(a) ((float) (a) * (180.0f / 3.141592653))

// calculate the signed distance from point <Q> to the
// plane defined by 1) the normal <pnv> and 2) a point
// in the plane <ppt>
namespace trig {
	static float PointPlaneDistance(const vec3& ppt, const vec3& pnv, const vec3& Q) {
		return (pnv.dot3D(Q) - pnv.dot3D(ppt));
	}
}

#endif
