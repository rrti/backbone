#ifndef PARTICLESYSTEM_DRAWER_HPP
#define PARTICLESYSTEM_DRAWER_HPP

#include "../Math/vec3.hpp"
class CParticleSystem;

class CParticleSystemDrawer {
	public:
		CParticleSystemDrawer() {}
		~CParticleSystemDrawer() {}

		void Draw(CParticleSystem*);

	private:
		void DrawGroundPlane(CParticleSystem*, float);
		void DrawSystem(CParticleSystem*);
		void DrawSystemBoundaries(CParticleSystem*);
};

#endif
