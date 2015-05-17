#include "./ParticleSystem.hpp"
#include "./Particle.hpp"
#include "../PathFinder/PathFinder.hpp"
#include "../../Math/RNG.hpp"
#include "../../Math/Constants.hpp"

#include <algorithm>

#define S1						1.1f
#define S2						1.2f
#define S3						1.1f
#define C1						0.5f
#define C2						0.6f
#define C3						0.5f
#define BORDER_STRENGTH			50.0f
#define ATTRACTION_STRENGTH		-1.0f
#define DRAG_COEFF				0.90f
#define MAX_FORCE				5.0f
#define MAX_SLICE_SEPARATION	30

CParticleSystem::CParticleSystem(int _numParticles) {
	numParticles = _numParticles;
	inited = false;

	particles.resize(numParticles, 0);
	sparticles.resize(numParticles, 0);
	temp1.resize(numParticles * 6);
	temp2.resize(numParticles * 6);

	CreateParticles();
}

CParticleSystem::~CParticleSystem() {
	for (int i = 0; i < numParticles; i++) {
		delete particles[i]; particles[i] = 0x0;
	}
}

void CParticleSystem::SetAttractionPoint(float x, float y) {
	attractionPoint.x = x;
	attractionPoint.y = y;
}

void CParticleSystem::CreateParticles() {
	for (int i = 0; i < numParticles; i++) {
		Particle* p = new Particle();
		float r = (rng.RandInt(10) / 400.0f);

		p->radius = r + 0.1f;
		p->mass = (p->radius * 10.0f) + 1.0f;

		particles[i] = p;
		sparticles[i] = p;
	}
}

void CParticleSystem::AddParticle(const vec3&, float) {
	// note: check if inside the bounding box first?
	/*
	Particle* p = new Particle();
	p->pos.x = pos.x;
	p->pos.y = pos.y;
	p->pos.z = pos.z;
	p->radius = radius;
	p->mass = p->radius * 100.0f + 1.0f;

	particles.push_back(p);
	numParticles++;
	temp1.resize(numParticles * 6);
	temp2.resize(numParticles * 6);
	printf("particle %d spawned @ <%+.2f, %+.2f>\n", numParticles, p->pos.x, p->pos.y);
	*/
}

Particle* CParticleSystem::GetParticle(int i) {
	return particles[i];
}

void CParticleSystem::InitParticles(CPathFinder* pf) {
	if (pf->tunnel.empty()) {
		return;
	}

	BoundingCircle& bc = pf->tunnel.front();

	for (int i = 0; i < numParticles; i++) {
		Particle* p = GetParticle(i);
		int layer = i / bc.numSegments;
		float rmod = 0.8f - (layer * 0.2f);

		// const float a1 = rng.RandInt(bc.numSegments) * bc.segmentAngle;
		// const float a2 = rng.RandInt(bc.numSegments) * bc.segmentAngle;
		// const float x = (bc.radius * rng.RandFloat(0.9f)) * cosf(DTOR(a1));
		// const float y = (bc.radius * rng.RandFloat(0.9f)) * sinf(DTOR(a2));
		const float a1 = i * bc.segmentAngle;
		const float a2 = i * bc.segmentAngle;
		const float x = bc.radius * rmod * cosf(DTOR(a1));
		const float y = bc.radius * rmod * sinf(DTOR(a2));
		vec3 pos(x, y, 0.0f);

		p->pos = bc.m.Mul(pos);
		p->velocity = NVec;
		p->force = NVec;
		p->sliceIdx = 0;
	}

	inited = true;
}



inline bool SortParticlesFunc(const Particle* p, const Particle* q) {
	return (p->sliceIdx > q->sliceIdx);
}

void CParticleSystem::Update(float deltaT, CPathFinder* pf) {
	if (!inited) {
		return;
	}

	std::sort(sparticles.begin(), sparticles.end(), &SortParticlesFunc);

	GetDerivative(pf, temp1);
	ScaleVector(temp1, deltaT);
	GetState(temp2);
	AddVectors(temp1, temp2, temp2);
	SetState(temp2);
}



void CParticleSystem::ComputeForces(CPathFinder* pf) {
	for (int x = 0; x < numParticles; x++) {
		Particle* i = GetParticle(x);

		if (i->sliceIdx >= pf->tunnel.size() - 1) {
			i->force = NVec; continue;
		}

		vec3 bfi = ComputeBorderForce(pf, i);

		// calculate the forces between all the particles and i
		for (int y = 0; y < numParticles; y++) {
			if (y == x) continue;

			Particle* j	= GetParticle(y);
			vec3 d		= i->pos - j->pos;
			float r		= d.len3D();
			float z		= r - i->radius - j->radius;

			z = (z < EPSILON)? EPSILON: z;

			float d1	= powf(z, S1);
			float d2	= powf(z, S2);
			float s		= (C1 / d1) + (C2 / d2);
			vec3 f		= (d / r) * s;

			i->force += f;
		}

		if (i->force.len3D() > MAX_FORCE) {
			i->force.norm();
			i->force *= MAX_FORCE;
		}

		i->force += bfi;
		i->borderForce = bfi;
	}
}

vec3 CParticleSystem::ComputeBorderForce(CPathFinder* pf, Particle* p) {
	bool update = true;
	int count = 0;
	vec3 force = NVec;

	const unsigned int maxSlice = sparticles[               0]->sliceIdx;
	const unsigned int minSlice = sparticles[numParticles - 1]->sliceIdx;
	const unsigned int midSlice = (maxSlice + minSlice) >> 1;
	const bool isGroupSeparated = ((maxSlice - minSlice) > MAX_SLICE_SEPARATION);

	while (update) {
		const unsigned int sliceIdx	= p->sliceIdx;
		const bool nextSlice		= (sliceIdx < (pf->tunnel.size() - 1));
		const BoundingCircle& bcm	=            pf->tunnel[sliceIdx    ];
		const BoundingCircle& bcn	= nextSlice? pf->tunnel[sliceIdx + 1]: bcm;
		const vec3 sliceNormalM		= (bcm.m).GetDir(2);
		const vec3 sliceNormalN		= (bcn.m).GetDir(2);
		const vec3 slicePosM		= (bcm.m).GetDir(3);

		const float distM = trig::PointPlaneDistance(p->pos, sliceNormalM, slicePosM);
		const bool passedM = (distM < 0.0f);

		const vec3 inposM = bcm.GetParticleInvPos(p->pos);
		const vec3 forceM = bcm.GetForce(inposM);

		vec3 zForce = (sliceNormalM + sliceNormalN) * 3.0f;

		if (isGroupSeparated) {
			if (sliceIdx > midSlice) { zForce *= 0.5f; } // slow down
			if (sliceIdx < midSlice) { zForce *= 2.5f; } // speed up
		}

		update = (passedM && nextSlice);
		force += (forceM + zForce);
		count += 1;

		if (update) {
			p->sliceIdx++;
		}
	}

	return (force / count);
}

void CParticleSystem::GetDerivative(CPathFinder* pf, std::vector<float> &dst) {
	ComputeForces(pf);

	for (int i = 0, j = 0; i < numParticles; i++) {
		Particle* p = GetParticle(i);
		dst[j++] = (p->velocity.x);
		dst[j++] = (p->velocity.y);
		dst[j++] = (p->velocity.z);
		// F = ma <==> a = F/m
		dst[j++] = (p->force.x / p->mass);
		dst[j++] = (p->force.y / p->mass);
		dst[j++] = (p->force.z / p->mass);
	}
}

void CParticleSystem::ScaleVector(std::vector<float> &v, float s) {
	for (unsigned int i = 0; i < v.size(); i++) {
		v[i] *= s;
	}
}

void CParticleSystem::AddVectors(const std::vector<float> &v1, const std::vector<float> &v2, std::vector<float> &v3) {
	for (unsigned int i = 0; i < v1.size(); i++)
		v3[i] = v1[i] + v2[i];
}

void CParticleSystem::GetState(std::vector<float>& v) {
	for (int i = 0, j = 0; i < numParticles; i++) {
		Particle* p = GetParticle(i);
		v[j++] = p->pos.x;
		v[j++] = p->pos.y;
		v[j++] = p->pos.z;
		v[j++] = p->velocity.x * DRAG_COEFF;
		v[j++] = p->velocity.y * DRAG_COEFF;
		v[j++] = p->velocity.z * DRAG_COEFF;
	}
}

void CParticleSystem::SetState(std::vector<float>& v) {
	for (int i = 0, j = 0; i < numParticles; i++) {
		Particle* p = GetParticle(i);
		p->pos.x = v[j++];
		p->pos.y = v[j++];
		p->pos.z = v[j++];
		p->velocity.x = v[j++];
		p->velocity.y = v[j++];
		p->velocity.z = v[j++];
	}
}
