#ifndef PATHFINDER_DRAWER_HPP
#define PATHFINDER_DRAWER_HPP

#include "../Math/vec3.hpp"
class CPathFinder;
class Node;
struct BoundingCircle;
class Particle;
class CParticleSystem;

class CPathFinderDrawer {
	public:
		CPathFinderDrawer() {
			tunnelList = -1;
			blockList = -1;
			sID = -1;
			gID = -1;
		}
		~CPathFinderDrawer() {}

		void Draw(CPathFinder*, CParticleSystem*);
		vec3& GetCamPos();
		vec3& GetCamDir();

	private:
		void DrawMap(CPathFinder*);
		void DrawCube(const vec3& color, float size, float lineWidth, int mode);
		void DrawHistory(CPathFinder*);
		void DrawParent(Node* n, Node* p);
		void DrawTunnel(CPathFinder*, Particle*);
		void DrawTunnelSegment(BoundingCircle* bc, bool);
		void DrawBall(CPathFinder*);
		void DrawCurve(CPathFinder*);
		void DrawVector(const vec3& v, const vec3& color);

		int sID, gID;
		int tunnelList;
		int blockList;
};

#endif
