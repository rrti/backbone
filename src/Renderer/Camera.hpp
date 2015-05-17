#ifndef CAMERA_HDR
#define CAMERA_HDR

#include "../Math/vec3.hpp"
#include "../Math/Trig.hpp"

enum CAMERA_MOTIONS {CAM_YAW, CAM_PITCH, CAM_ROLL, CAM_MOVE, CAM_HSTRAFE, CAM_VSTRAFE};
enum CAMERA_MODES {CAM_FPS, CAM_AZIMUTH};

struct Camera {
	Camera(const vec3& poss = NVec, const vec3& tarr = NVec, const vec3& azDataa = NVec, int m = CAM_AZIMUTH) {
		xDir = vec3(1.0f, 0.0f, 0.0f);
		yDir = vec3(0.0f, 1.0f, 0.0f);
		zDir = vec3(0.0f, 0.0f, 1.0f);

		// treat azDataa.x as distance multiplier
		azData.distance  = azData.cDistance = azDataa.x * poss.len3D();
		azData.rotation  = azData.cRotation = azDataa.y;
		azData.elevation = azData.cElevation = azDataa.z;
		azData.target    = tarr;

		UpdateAxisSystem(true);

		mode = m;
	}

	void Adjust(int motionType, int sign, float scalar) {
		switch (motionType) {
			case CAM_YAW:		Yaw(    sign, scalar); break;
			case CAM_PITCH:		Pitch(  sign, scalar); break;
			case CAM_ROLL:		Roll(   sign, scalar); break;
			case CAM_MOVE:		Move(   sign, scalar); break;
			case CAM_HSTRAFE:	HStrafe(sign, scalar); break;
			case CAM_VSTRAFE:	VStrafe(sign, scalar); break;
			default: break;
		}

		if (mode == CAM_AZIMUTH) {
			// strafing (panning) in azimuth-mode requires that this is updated
			azData.target = pos + (zDir * azData.distance);
		} else {
			// update the EZR paramaters so we return to azimuth-mode smoothly
			UpdateAZIData();
		}
	}

	int GetMode() const { return mode; }
	void SetMode(int m) { mode = m; }


	void Yaw(int sign, float scalar) {
		tar += (xDir * sign * scalar * 0.5f);
		zDir = (tar - pos).norm();
		tar = pos + zDir;

		// when enabled, world "rolls" but we can spin around 360
		xDir = zDir.cross(yDir);
		xDir.norm();
	}
	void Pitch(int sign, float scalar) {
		tar += (yDir * sign * scalar * 0.5f);
		zDir = (tar - pos).norm();
		tar = pos + zDir;

		// when enabled, world "rolls" but we can spin around 360
		yDir = xDir.cross(zDir);
		yDir.norm();
	}
	void Roll(int sign, float scalar) {
		yDir += (xDir * sign * scalar);
		xDir = zDir.cross(yDir);

		xDir.norm();
		yDir.norm();
	}

	// forward and backward (along zDir)
	void Move(int sign, float scalar) {
		pos += (zDir * sign * scalar);
		tar += (zDir * sign * scalar);
	}
	// left and right (along xDir)
	void HStrafe(int sign, float scalar) {
		pos += (xDir * sign * scalar);
		tar += (xDir * sign * scalar);
	}
	// up and down (along yDir)
	void VStrafe(int sign, float scalar) {
		pos += (yDir * sign * scalar);
		tar += (yDir * sign * scalar);
	}


	void SetAxes(const vec3& poss, const vec3& tarr) {
		pos = poss;
		tar = tarr;

		yDir = YVec;

		zDir = (tar - pos).norm();
		xDir = zDir.cross(yDir).norm();
		yDir = xDir.cross(zDir).norm();

		tar = pos + zDir;
	}

	void UpdateAxisSystem(bool notZooming) {
		if (notZooming) {
			const vec3 npos = azData.GetSpherePos() + azData.target;
			const vec3 ntar = (azData.target - pos).norm() + azData.target;

			// when elevation or rotation changes, camera
			// axis-system does too so update it from new
			// pos
			SetAxes(npos, ntar);
		} else {
			// zooming leaves axis-system unaffected
			pos = azData.GetSpherePos() + azData.target;
			tar = pos + zDir;
		}
	}

	void UpdateAZIData() {
		azData.target = pos + (zDir * azData.distance);

		/*
		const vec3 v = pos - azData.target;
		const vec3 w = vec3(pos.x, tar.y, pos.z);

		const float d = v.len2D() + 0.001f;			// 2D distance between pos and target
		const float s = v.len3D() + 0.001f;			// 3D distance between pos and target
		const float h = pos.y - azData.target.y;	// height diff between pos and target

		const float e = RTOD(asinf(h / s));			// elevation
		const float g = XVec.dot3D(w);
		const float r = RTOD(acosf(g / w.len2D()));

		azData.target = pos + (zDir * azData.distance);
		azData.elevation = e;
		azData.rotation = r;
		azData.distance = s;
		*/

		/*
		// camera position with y flattened
		const vec3 cpos(pos.x, 0.0f, pos.z);

		// for the elevation angle
		const float dotProd1 = pos.dot3D(cpos);
		const float lenProd1 = cpos.len2D() * pos.len3D() + 0.001f;

		// for the rotation angle
		const float dotProd2 = XVec.dot3D(cpos);
		const float lenProd2 = 1.0f * cpos.len2D();

		// angle between flattened and actual camera position vectors
		// (a dot b) = |a||b| cos theta <==> theta = arccos((a dot b) / |a||b|)
		azData.elevation = RTOD(acosf(dotProd1 / lenProd1));
		azData.rotation = RTOD(acosf(dotProd2 / lenProd2));
		// azData.distance = pos.len3D(); // relative to (0, 0, 0)
		azData.distance = (pos - azData.target).len3D(); // relative to target
		azData.target = pos + (zDir * azData.distance);

		printf("azData.elevation: %f\n", azData.elevation);
		printf("azData.rotation: %f\n", azData.rotation);
		printf("azData.distance: %f\n", azData.distance);
		printf("azData.target: %f %f %f\n", azData.target.x, azData.target.y, azData.target.z);
		*/
	}


	struct AzimuthData {
		AzimuthData(float d = 0.0f, float zr = 0.0f, float az = 0.0f) {
			cDistance = distance = d;
			cRotation = rotation = zr;
			cElevation = elevation = az;
		}

		vec3 GetSpherePos() {
			// returns position on sphere centered at
			// (0, 0, 0) and with radius <distance>
			// from current elevation and rotation
			const float beta = DTOR(elevation);
			const float gamma = DTOR(rotation);

			// derived from beta and gamma
			float cx = distance;
			float cy = 0.0f;
			float cz = 0.0f;
			float tx = cx;

			// rotate around Z (elevation)
			tx = cx;
			cx = cx * cosf(beta) + cy * sinf(beta);
			cy = tx * sinf(beta) + cy * cosf(beta);

			// rotate around Y (rotation)
			tx = cx;
			cx = cx * cosf(gamma) - cz * sinf(gamma);
			cz = tx * sinf(gamma) + cz * cosf(gamma);

			return vec3(cx, cy, cz);
		}

		void SaveERZ(bool notZooming) {
			if (notZooming) {
				cRotation = rotation;
				cElevation = elevation;
			} else {
				cDistance = distance;
			}
		}

		void UpdateER(int dx, int dy) {
			rotation = cRotation - dx * 0.25f;
			elevation = cElevation - dy * 0.25f;

			// cap the elevation (vertical angle)
			if (elevation >  89.99f) elevation =  89.99f;
			if (elevation < -89.99f) elevation = -89.99f;
		}
		void UpdateZ(int dy) {
			distance = cDistance - dy * 0.5f;

			// cap the distance
			if (distance <    0.1f) distance =    0.1f;
			if (distance > 1000.0f) distance = 1000.0f;
		}

		// note: horizontal rotation is with respect to x-axis
		float distance, cDistance;
		float rotation, cRotation;		// in degrees
		float elevation, cElevation;	// in degrees
		vec3 target;					// center of sphere we move around
	};

	AzimuthData azData;

	vec3 pos;		// camera ("eye") location
	vec3 tar;		// pos camera is looking at
	vec3 xDir;		// "right"   dir (vRi)
	vec3 yDir;		// "up"      dir (vUp)
	vec3 zDir;		// "forward" dir (vFo)

	int mode;
};

#endif
