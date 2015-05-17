#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <vector>

#include "../../Math/matrix44.hpp"
#include "../../Math/vec3.hpp"
class Particle;
class CPathFinder;

class CParticleSystem {
	public:
		CParticleSystem(int numParticles);
		~CParticleSystem();

		Particle* GetParticle(int i);
		int numParticles;

		void Reset() { inited = false; }
		bool IsInited() const { return inited; }

		void Update(float deltaT, CPathFinder* pf);
		void CreateParticles();
		void InitParticles(CPathFinder* pf);
		void AddParticle(const vec3& pos, float radius);
		void SetAttractionPoint(float x, float y);

	private:
		std::vector<Particle*> particles;
		std::vector<Particle*> sparticles;
		std::vector<float> temp1, temp2;
		vec3 attractionPoint;
		bool inited;

		void ComputeForces(CPathFinder* pf);
		vec3 ComputeBorderForce(CPathFinder* pf, Particle* p);
		void GetDerivative(CPathFinder* pf, std::vector<float>& dst);
		void ScaleVector(std::vector<float>& v, float s);
		void AddVectors(const std::vector<float>& v1, const std::vector<float>& v2, std::vector<float>& v3);
		void GetState(std::vector<float>& v);
		void SetState(std::vector<float>& v);
};

#endif
