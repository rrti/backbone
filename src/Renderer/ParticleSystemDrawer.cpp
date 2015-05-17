#include <GL/gl.h>
#include <GL/glut.h>

#include "./ParticleSystemDrawer.hpp"
#include "../Sim/ParticleSystem/Particle.hpp"
#include "../Sim/ParticleSystem/ParticleSystem.hpp"

// auxiliary functions
inline static void DrawCircle(float r, const vec3&, int, const vec3&, int);
inline static void DrawVector(const vec3&, const vec3&, const vec3&);
inline static void gluMyCube(float);

void CParticleSystemDrawer::Draw(CParticleSystem* ps) {
	DrawGroundPlane(ps, 2.0f);
	DrawSystemBoundaries(ps);
	DrawSystem(ps);
}

void CParticleSystemDrawer::DrawGroundPlane(CParticleSystem* ps, float scale) {
	ps = 0x0;
	scale = 0.0f;
}

void CParticleSystemDrawer::DrawSystem(CParticleSystem* ps) {
	/*
	if (ps->DrawParticleForces()) {
		for (int i = 0; i < ps->numParticles; i++) {
			Particle* p = ps->getParticle(i);

			vec3 c1(0.0f, 0.0f, 1.0f);
			DrawVector(p->force, p->pos, c1);
			vec3 c2(0.0f, 1.0f, 0.0f);
			DrawVector(p->velocity, p->pos, c2);
			vec3 c3(1.0f, 0.0f, 0.0f);
			DrawVector(p->borderForce, p->pos, c3);
		}
	}
	*/
	if (!ps->IsInited()) {
		return;
	}

	for (int i = 0; i < ps->numParticles; i++) {
		Particle* p = ps->GetParticle(i);

		const vec3 vAbs = p->velocity.abs() + vec3(0.01f, 0.01f, 0.01f);

		const float r = (vAbs.x > 1.0f)? 1.0f - (1.0f / vAbs.x): 1.0f - vAbs.x;
		const float g = (vAbs.y > 1.0f)? 1.0f - (1.0f / vAbs.y): 1.0f - vAbs.y;
		const float b = (vAbs.z > 1.0f)? 1.0f - (1.0f / vAbs.z): 1.0f - vAbs.z;

		vec3 color(r, g, b);

		glColor4f(color.x, color.y, color.z, 0.8f);
		glPushMatrix();
			glTranslatef(p->pos.x, p->pos.y, p->pos.z);
			glutSolidSphere(p->radius, 10, 10);
		glPopMatrix();
	}
}

void CParticleSystemDrawer::DrawSystemBoundaries(CParticleSystem* ps) {
	ps = 0x0;
}



inline static void DrawCircle(float r, const vec3& pos, int s, const vec3& color, int mode) {
	glColor4f(color.x, color.y, color.z, 0.9f);
	glBegin(mode);

	for (int i = s; i >= 1; i--) {
		float a = ((i * 360) / s) * PIDIV180;
		float y = cosf(a) * r;
		float x = sinf(a) * r;
		glVertex3f(x + pos.x, y + pos.y, 0.0f);
	}

	glEnd();
}

inline static void DrawVector(const vec3& v, const vec3& pos, const vec3& color) {
	float x = v.x * 0.33f;
	float y = v.y * 0.33f;
	float z = v.z * 0.33f;

	glLineWidth(2.0f);
	glBegin(GL_LINE_STRIP);
	glColor4f(color.x, color.y, color.z, 0.9f);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + x, pos.y + y, pos.z + z);
	glEnd();
}

inline static void gluMyCube(float lineWidth) {
	// unit cube (before scaling) centered at
	// (0, 0, 0) extends 0.5 units along each
	// axis
	const float mid = 0.5f;

	glLineWidth(lineWidth);
	glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
		// top
		glVertex3f( mid, mid,  mid);
		glVertex3f( mid, mid, -mid);
		glVertex3f(-mid, mid, -mid);
		glVertex3f(-mid, mid,  mid);
		// front
		glVertex3f( mid,  mid, mid);
		glVertex3f(-mid,  mid, mid);
		glVertex3f(-mid, -mid, mid);
		glVertex3f( mid, -mid, mid);
		// right
		glVertex3f(mid,  mid,  mid);
		glVertex3f(mid, -mid,  mid);
		glVertex3f(mid, -mid, -mid);
		glVertex3f(mid,  mid, -mid);
		// left
		glVertex3f(-mid,  mid,  mid);
		glVertex3f(-mid,  mid, -mid);
		glVertex3f(-mid, -mid, -mid);
		glVertex3f(-mid, -mid,  mid);
		// bottom
		glVertex3f( mid, -mid,  mid);
		glVertex3f( mid, -mid, -mid);
		glVertex3f(-mid, -mid, -mid);
		glVertex3f(-mid, -mid,  mid);
		// back
		glVertex3f( mid,  mid, -mid);
		glVertex3f(-mid,  mid, -mid);
		glVertex3f(-mid, -mid, -mid);
		glVertex3f( mid, -mid, -mid);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
