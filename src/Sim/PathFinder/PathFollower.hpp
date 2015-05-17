#ifndef PATHFOLLOWER_HPP
#define PATHFOLLOWER_HPP

#include <vector>

struct PathFollower {
	void Init() {
		pos = NVec;
		curveIdx = 0;
	}
	void Init(const vec4& p, int idx) {
		pos = p;
		curveIdx = idx;
	}

	void Update(const std::vector<vec4>& curve) {
		if (curveIdx < curve.size()) {
			const vec4& p1 = curve[curveIdx - 1];
			const vec4& p2 = curve[curveIdx    ];

			dir = (p2 - p1).norm();
			speed = 0.04f;
			pos += (dir * speed);
			wantedDir = (p2 - pos).norm();

			// determine if we have passed <p2>: if the distance
			// from p2 to the plane defined by the normal vector
			// <dir> and plane-point <pos> is less than 0, then
			// p2 is behind the plane
			if (trig::PointPlaneDistance(pos, dir, p2) < 0.0f) {
				curveIdx += 1;
			}

			// alternative method (works well if
			// curve consists of enough points)
			//
			// pos = curve[curveIdx - 1];
			// curveIdx += 1;
		}
	}

	vec3 pos;
	vec3 dir;
	vec3 wantedDir;
	float speed;
	unsigned int curveIdx;
};

#endif
