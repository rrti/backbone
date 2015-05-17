#include "./PathFinder.hpp"
#include "./PathFollower.hpp"
#include "../../Math/RNG.hpp"
#include "../../Math/Trig.hpp"
#include "../../Math/Interpolators.hpp"
#include "../../System/ScopedTimer.hpp"
#include <math.h>

#define NODE(n) dynamic_cast<Node*>(n)
PathFollower pathFollower;

CPathFinder::CPathFinder(int X, int Y, int Z) {
	this->X = X;
	this->Y = Y;
	this->Z = Z;

	for (int x = 0; x < X; x++) {
		for (int y = 0; y < Y; y++) {
			for (int z = 0; z < Z; z++) {
				Node n(x, y, z, id(x, y, z), 0.0f);
				map.push_back(n);
			}
		}
	}

	canSearch = true;
	showBlockedNodes = true;
	showVisitedNodes = false;
	showBackBonePath = true;
	step = 0;
	// how badly do we want to explore (find the largest tunnel)?
	radialScalar = 0.9f;
}

void CPathFinder::toggleBlocked(int x, int y, int z) {
	map[id(x, y, z)].toggleBlocked();
}

void CPathFinder::GenerateSphereBlockOffsets() {
	sphereBlockOffsets.clear();

	// generate a list of XYZ-offsets of blocks (nodes)
	// in a virtual block-sphere that we need to check
	// while pathfinding to see if our path can pass at
	// minimum radius
	// note: this does not generate the block offsets in
	// a strictly "outwards" pattern, might be faster?
	for (float r = RADIALSTEP; r <= maxRad; r += RADIALSTEP) {
		int M = int(ceilf(r));

		for (int x = -M; x <= M; x++) {
			for (int y = -M; y <= M; y++) {
				for (int z = -M; z <= M; z++) {
					float l = sqrt(x*x + y*y + z*z);
					if (l >= r - EPSILON)
						continue;

					SphereBlockOffset offset(r, x, y, z);

					if (std::find_if(sphereBlockOffsets.begin(), sphereBlockOffsets.end(), offset) != sphereBlockOffsets.end()) {
						continue;
					}

					sphereBlockOffsets.push_back(offset);
				}
			}
		}
	}
}


void CPathFinder::successors(ANode* an, std::queue<ANode*>& succ) {
	Node *s = 0x0, *n = NODE(an);
	int x, y, z;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				// don't add the parent node
				if (k == 0 && j == 0 && i == 0)
					continue;

				x = n->x + i; y = n->y + j; z = n->z + k;

				// check if we are within boundaries
				if (x < X && x >= 0 && y < Y && y >= 0 && z < Z && z >= 0) {
					s = &map[id(x, y, z)];

					// can our corridor pass this successor
					// without shrinking to less than minRad?
					if (PathCanPass(s)) {
						s->w = radialScalar * ((1.0f - s->radius / maxRad) + 1.0f);
						succ.push(s);
					}
				}
			}
		}
	}
}

bool CPathFinder::PathCanPass(Node* sn) {
	for (unsigned int i = 0; i < sphereBlockOffsets.size(); i++) {
		// "blow up the balloon" around this successor node:
		// for each offset block around <sn>, check if that
		// block is blocking (impassable) and if so, if our
		// corridor could still pass it at minimal radius
		const SphereBlockOffset& sbo = sphereBlockOffsets[i];
		float r = sbo.r;
		int B = (int) floorf(r);
		int x = sbo.x + sn->x; bool b1 = (x <= X - B && x >= B);
		int y = sbo.y + sn->y; bool b2 = (y <= Y - B && y >= B);
		int z = sbo.z + sn->z; bool b3 = (z <= Z - B && z >= B);

		if (b1 && b2 && b3) {
			Node* o = &map[id(x, y, z)];
			sn->radius = r;

			if (o->blocked()) {
				r -= RADIALSTEP;
				sn->radius = r;

				if (r < minRad - EPSILON)
					return false;
				return true;
			}
		} else {
			r -= RADIALSTEP;
			sn->radius = r;

			if (r < minRad - EPSILON)
				return false;

			return true;
		}
	}
	return true;
}

float CPathFinder::heuristic(ANode* an1, ANode* an2) {
	const Node* n1 = NODE(an1);
	const Node* n2 = NODE(an2);
	const int dx = abs(n1->x - n2->x);
	const int dy = abs(n1->y - n2->y);
	const int dz = abs(n1->z - n2->z);
	return sqrt(dx*dx + dy*dy + dz*dz);
}


void CPathFinder::setStart(int x, int y, int z) {
	sId = id(x, y, z);
	Node* s = &map[sId];
	s->setStart();
	start = s;
}

void CPathFinder::setGoal(int x, int y, int z) {
	gId = id(x, y, z);
	Node* g = &map[gId];
	g->setGoal();
	goal = g;
}


void CPathFinder::Reset() {
	canSearch = true;
	path.clear();
	blocked.clear();
	curve.clear();
	tunnel.clear();
	init();

	step = 0;

	for (unsigned int i = 0; i < map.size(); i++) {
		map[i].bType = NORMAL;
	}

	for (int g = 6; g < X - 6; g++) {
		for (int h = 0; h < 3; h++) {
			int ry = rng.RandInt(Y - 1);
			int rz = rng.RandInt(Z - 1);

			for (int i = -2; i < 2; i++) {
				for (int j = -2; j < 2; j++) {
					for (int k = -2; k < 2; k++) {
						int x = g + i;
						int y = ry + j;
						int z = rz + k;

						if ((y < Y && y >= 0) && (z < Z && z >= 0) && (x < X && x >= 0)) {
							map[id(x, y, z)].toggleBlocked();
						}
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < map.size(); i++) {
		if (map[i].blocked()) {
			blocked.push_back(&map[i]);
		}
	}

	int sx = 3;
	int sy = rng.RandInt(Y - 4) + 2;
	int sz = rng.RandInt(Z - 4) + 2;
	setStart(sx, sy, sz);

	int gx = 22;
	int gy = rng.RandInt(Y - 4) + 2;
	int gz = rng.RandInt(Z - 4) + 2;
	/// DEBUG: CREATE VERTICAL PATH SEGMENTS
	/// setGoal(sx, sy - 10, sz);
	setGoal(gx, gy, gz);
}

void CPathFinder::search(float minRad, float maxRad) {
	if (!canSearch) {
		return;
	}

	this->minRad = minRad;
	this->maxRad = maxRad;
	canSearch = false;
	GenerateSphereBlockOffsets();

	// push goal since interpolation occurs between point b and c (goal = a)
	path.push_back(goal);

	{
		ScopedTimer t("CPathFinder::findPath()");
		findPath(path);
	}

	// push start since it's never in the path
	path.push_back(start);
	// push start again since interpolation occurs between point b and c (start = d)
	path.push_back(start);

	if (path.size() > 3) {
		BuildPathCurve(0.05f);
		BuildTunnel();
	}
}

void CPathFinder::BuildPathCurve(float muStep) {
	LinearInterpolator lip;
	HermiteInterpolator hip;

	pathFollower.Init();

	for (unsigned int i = 3; i < path.size(); i++) {
		Node* a = NODE(path[i - 3]);
		Node* b = NODE(path[i - 2]);
		Node* c = NODE(path[i - 1]);
		Node* d = NODE(path[i    ]);


		if (i == 4) {
			// first curve segment is now filled
			pathFollower.Init(curve[0], 1);
		}


		// interpolation occurs between point b and c with mu in [0, 1]
		for (float mu = 0.0f; mu <= 1.0f; mu += muStep) {
			vec4 p;
			p.x = hip.Interpolate(a->x, b->x, c->x, d->x, mu, 0.0f, 0.0f);
			p.y = hip.Interpolate(a->y, b->y, c->y, d->y, mu, 0.0f, 0.0f);
			p.z = hip.Interpolate(a->z, b->z, c->z, d->z, mu, 0.0f, 0.0f);
			p.w = lip.Interpolate(b->radius, c->radius, mu);
			curve.push_back(p);
		}
	}
}

void CPathFinder::BuildTunnel() {
	for (unsigned int i = 1; i < curve.size(); i++) {
		const vec4& p0 = curve[i - 1];
		const vec4& p1 = curve[i    ];
		const vec3 n = (p1 - p0).norm();
		const float r = p0.w;

		BoundingCircle bc(p0, n, 16, r);
		tunnel.push_back(bc);
	}
}


void CPathFinder::update() {
	if (!canSearch) {
		step++;
		step = (step > sphereBlockOffsets.size())? sphereBlockOffsets.size(): step;

		pathFollower.Update(curve);
	}
}
