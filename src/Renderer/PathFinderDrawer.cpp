#include <GL/gl.h>
#include <GL/glut.h>

#include "./PathFinderDrawer.hpp"
#include "../Common/CommonTypes.hpp"
#include "../Sim/PathFinder/PathFinder.hpp"
#include "../Sim/PathFinder/PathFollower.hpp"
#include "../Sim/ParticleSystem/BoundingCircle.hpp"
#include "../Sim/ParticleSystem/Particle.hpp"
#include "../Sim/ParticleSystem/ParticleSystem.hpp"
#include "../System/GEngine.hpp"
extern PathFollower pathFollower;

void CPathFinderDrawer::Draw(CPathFinder* pf, CParticleSystem* ps) {
	glPushMatrix();
		// translate the world's boundary box
		// rather than everything inside it
		glTranslatef((pf->X * 0.5f), (pf->Y * 0.5f), (pf->Z * 0.5f));
		DrawCube(ZVec, pf->X, 1.0f, GL_LINE);
	glPopMatrix();

	glPushMatrix();
		DrawMap(pf);
		DrawCurve(pf);
		DrawHistory(pf);
		DrawTunnel(pf, ps->GetParticle(0));
		// DrawBall(pf);
	glPopMatrix();


	/*
	// SphereBlockOffset sanity check
	vec3 color2 = vec3(1.0f, 1.0f, 1.0f);
	for (unsigned int i = 0; i < pf->step; i++) {
		CPathFinder::SphereBlockOffset* sbo = &(pf->sphereBlockOffsets[i]);
		glPushMatrix();
		glTranslatef(sbo->x, sbo->y, sbo->z);
		DrawCube(color2, 0.6f, 1.0f, GL_FILL);
		glPopMatrix();
	}
	*/
}

void CPathFinderDrawer::DrawCurve(CPathFinder* pf) {
	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);

	for (unsigned int i = 1; i < pf->curve.size(); i++) {
		vec3* p0 = &pf->curve[i - 1];
		vec3* p1 = &pf->curve[i    ];

		glVertex3f(p0->x, p0->y, p0->z);
		glVertex3f(p1->x, p1->y, p1->z);
	}

	glEnd();
}

void CPathFinderDrawer::DrawMap(CPathFinder* pf) {
	if (pf->showBlockedNodes) {
		if (sID != pf->sId || gID != pf->gId) {
			// world changed, force a list rebuild
			if (blockList >= 0) {
				glDeleteLists(blockList, 1);
				blockList = -1;
			}

			sID = pf->sId;
			gID = pf->gId;
		}

		if (blockList == -1) {
			blockList = glGenLists(1);
			glNewList(blockList, GL_COMPILE);

			for (unsigned int i = 0; i < pf->blocked.size(); i++) {
				Node* n = pf->blocked[i];

				glPushMatrix();
					glTranslatef(n->x, n->y, n->z);
					DrawCube(n->GetColor(), 0.6f, 1.0f, GL_FILL);
				glPopMatrix();
			}

			glEndList();
		} else {
			glCallList(blockList);
		}
	}

	Node* s = &(pf->map[pf->sId]);
	Node* g = &(pf->map[pf->gId]);

	glPushMatrix();
		glTranslatef(s->x, s->y, s->z);
		DrawCube(s->GetColor(), 1.0f, 3.0f, GL_LINE);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(g->x, g->y, g->z);
		DrawCube(g->GetColor(), 1.0f, 3.0f, GL_LINE);
	glPopMatrix();
}



/*
void DrawSolidTunnelSegment(BoundingCircle* bcm, BoundingCircle* bcn) {
	const int numVerts = bcm->vertices.size();

	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPushMatrix();
		glColor3f(1.0f, 0.60f, 0.0f);
		glBegin(GL_QUADS);

		for (int i = 0; i < numVerts; i++) {
			const int j = (i + 1) % numVerts;
			const vec3& n1 = bcm->segmentNormals[i], v1 = bcm->m.Mul(bcm->vertices[i]);
			const vec3& n2 = bcm->segmentNormals[j], v2 = bcm->m.Mul(bcm->vertices[j]);
			const vec3& n3 = bcn->segmentNormals[j], v3 = bcn->m.Mul(bcn->vertices[j]);
			const vec3& n4 = bcn->segmentNormals[i], v4 = bcn->m.Mul(bcn->vertices[i]);

			glNormal3f(n1.x, n1.y, n1.z); glVertex3f(v1.x, v1.y, v1.z);
			glNormal3f(n2.x, n2.y, n2.z); glVertex3f(v2.x, v2.y, v2.z);
			glNormal3f(n3.x, n3.y, n3.z); glVertex3f(v3.x, v3.y, v3.z);
			glNormal3f(n4.x, n4.y, n4.z); glVertex3f(v4.x, v4.y, v4.z);
		}

		glEnd();

	glPopMatrix();
	glShadeModel(GL_SMOOTH);
}
*/

void CPathFinderDrawer::DrawTunnel(CPathFinder* pf, Particle* part) {
	if (!pf->showBackBonePath) {
		return;
	}

	if (pf->tunnel.empty()) {
		if (tunnelList >= 0) {
			glDeleteLists(tunnelList, 1);
			tunnelList = -1;
		}
		return;
	}

	if (tunnelList == -1) {
		tunnelList = glGenLists(1);
		glNewList(tunnelList, GL_COMPILE);

		/*
		for (unsigned int i = 0; i < pf->tunnel.size() - 1; i++) {
			BoundingCircle* bcm = &pf->tunnel[i    ];
			BoundingCircle* bcn = &pf->tunnel[i + 1];
			DrawSolidTunnelSegment(bcm, bcn);
		*/

		for (unsigned int i = 0; i < pf->tunnel.size(); i++) {
			DrawTunnelSegment(&pf->tunnel[i], false);
		}

		glEndList();
	} else {
		glCallList(tunnelList);
	}

	DrawTunnelSegment(&pf->tunnel[part->sliceIdx], true);
}

void CPathFinderDrawer::DrawTunnelSegment(BoundingCircle* bcp, bool hilite) {
	const int numVerts = bcp->vertices.size();

	glPushMatrix();
		glMultMatrixf(&bcp->m[0]);
		if (hilite) {
			glColor3f(1.0f, 0.0f, 0.0f);
			glLineWidth(12.0f);
		} else {
			glColor3f(1.0f, 0.60f, 0.0f);
			glLineWidth(3.0f);
		}
		glBegin(GL_LINE_STRIP);

		for (int i = 0; i < numVerts; i++) {
			const int j = (i + 1) % numVerts;
			const vec3& v1 = bcp->vertices[i];
			const vec3& v2 = bcp->vertices[j];

			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
		}

		glEnd();

	glPopMatrix();
}



void CPathFinderDrawer::DrawBall(CPathFinder* pf) {
	if (pf->curve.size() > 0) {
		glPushMatrix();
			glTranslatef(pathFollower.pos.x, pathFollower.pos.y, pathFollower.pos.z);
			glColor3f(0.5f, 0.0f, 0.0f);
			glutSolidSphere(0.5f, 15, 15);
		glPopMatrix();
	}
}


void CPathFinderDrawer::DrawHistory(CPathFinder* pf) {
	glColor3f(1.0f, 1.0f, 0.0f);
	vec3 color = vec3(1.0f, 1.0f, 1.0f);

	const int hsize = pf->history.size();
	const int psize = pf->path.size() - 2;

	for (int i = 0; i < hsize; i++) {
		int j = (i < hsize - 1)? i + 1: i;
		Node* n = dynamic_cast<Node*>(pf->history[i]);
		Node* o = dynamic_cast<Node*>(pf->history[j]);

		float x = (float) n->x;
		float y = (float) n->y;
		float z = (float) n->z;

		glPushMatrix();
		glTranslatef(x, y, z);

		// path nodes and links
		if ((hsize - psize) < i) {
			vec3 color = vec3(1.0f, 1.0f, 0.0f);
			vec3 link = o->GetVector() - n->GetVector();

			glutSolidSphere(0.1f, 15, 15);
		}
		// visited cq. parent nodes
		else if (pf->showVisitedNodes) {
			// parent
			if ((i & 1) == 1) {
				DrawParent(n, dynamic_cast<Node*>(pf->history[i - 1]));
			}
		}

		glPopMatrix();
	}
}

void CPathFinderDrawer::DrawVector(const vec3& v, const vec3& color) {
	glColor3f(color.x, color.y, color.z);

	glPushMatrix();
		glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(v.x, v.y, v.z);
		glEnd();
	glPopMatrix();
}

void CPathFinderDrawer::DrawParent(Node* n, Node* p) {
	vec3 color = vec3(1.0f, 0.0f, 1.0f);
	glColor3f(color.x, color.y, color.z);
	glLineWidth(1.0f);

	float x = (p->x - n->x);
	float y = (p->y - n->y);
	float z = (p->z - n->z);

	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(x,y,z);
	glEnd();

	glPushMatrix();
		glTranslatef(x,y,z);
		DrawCube(color, 0.1f, 1.0f, GL_LINE);
	glPopMatrix();
}

void CPathFinderDrawer::DrawCube(const vec3& color, float size, float lineWidth, int mode) {
	glLineWidth(lineWidth);
	glColor3f(color.x, color.y, color.z);

	if (mode == GL_FILL) { glutSolidCube(size); }
	if (mode == GL_LINE) { glutWireCube(size); }
}

vec3& CPathFinderDrawer::GetCamPos() { return pathFollower.pos; }
vec3& CPathFinderDrawer::GetCamDir() { return pathFollower.dir; }
