#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <vector>
#include <algorithm>

#include "./AAStar.hpp"
#include "./Node.hpp"
#include "../ParticleSystem/BoundingCircle.hpp"

#define RADIALSTEP 0.5f


class CPathFinder: public AAStar {
	private:
		void successors(ANode* an, std::queue<ANode*>& succ);
		float heuristic(ANode* an1, ANode* an2);
		bool PathCanPass(Node* n);
		void GenerateSphereBlockOffsets();
		void BuildPathCurve(float);
		void BuildTunnel();
		inline int id(int x, int y, int z) const { return ((x * Y * Z) + (y * Z) + z); }

		float minRad, maxRad, radialScalar;

	public:
		CPathFinder(int X, int Y, int Z);
		void setStart(int x, int y, int z);
		void setGoal(int x, int y, int z);
		void toggleBlocked(int x, int y, int z);
		void toggleShowBlockedNodes() { showBlockedNodes = !showBlockedNodes; }
		void toggleShowVisitedNodes() { showVisitedNodes = !showVisitedNodes; }
		void toggleShowBackBonePath() { showBackBonePath = !showBackBonePath; }
		void Reset();
		void search(float minRad, float maxRad);
		void update();
		vec3 GetWorldSize() const { return vec3(X, Y, Z); }

		struct SphereBlockOffset {
			SphereBlockOffset(float _r = 0.0f, int _x = 0, int _y = 0, int _z = 0) {
				r = _r; x = _x; y = _y; z = _z;
			}

			bool operator () (const SphereBlockOffset& obs) const {
				return (x == obs.x && y == obs.y && z == obs.z);
			}

			float r;
			int x;
			int y;
			int z;
		};

		std::vector<SphereBlockOffset> sphereBlockOffsets;
		std::vector<Node> map;
		std::vector<Node*> blocked;
		std::vector<ANode*> path;
		std::vector<vec4> curve;
		std::vector<BoundingCircle> tunnel;

		int X, Y, Z;
		int sId, gId;
		unsigned int step;
		bool canSearch;
		bool showBlockedNodes, showVisitedNodes, showBackBonePath;
};

#endif
