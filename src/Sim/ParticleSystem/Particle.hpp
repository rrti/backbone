#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "../../Math/vec3.hpp"

class Particle {
	public:
		Particle();
		~Particle() {}

		float radius;
		float mass;
		vec3 pos;
		vec3 velocity;
		vec3 force;
		vec3 borderForce;
		unsigned int sliceIdx;

		bool operator < (const Particle* p) const {
			return (sliceIdx < p->sliceIdx);
		}

		bool operator () (const Particle* p, const Particle* q) const {
			return (p->sliceIdx > q->sliceIdx);
		}
};

#endif
