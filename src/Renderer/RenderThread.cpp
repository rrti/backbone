#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>

#include "../Common/CommonTypes.hpp"
#include "./RenderThread.hpp"
#include "./Camera.hpp"
#include "../Math/vec3.hpp"
#include "../Math/Trig.hpp"
#include "../Sim/SimThread.hpp"
#include "../System/GEngine.hpp"

#include "./PathFinderDrawer.hpp"
#include "./ParticleSystemDrawer.hpp"

CRenderThread::CRenderThread(const vec3& worldSize) {
	lightEnabled = false;
	wantLight = true;
	wantTrack = false;

	const vec3 camPos = (worldSize * 0.5f);
	const vec3 camTar = (worldSize * 0.5f) - vec3(0.0f, 0.0f, worldSize.y * 0.5f);
	const vec3 azData = vec3(1.0f, 90.0f, 20.0f);

	cam = new Camera(camPos, camTar, azData, CAM_AZIMUTH);
	psDrawer = new CParticleSystemDrawer();
	pfDrawer = new CPathFinderDrawer();
}

CRenderThread::~CRenderThread() {
	delete cam; cam = 0x0;
	delete psDrawer; psDrawer = 0x0;
	delete pfDrawer; pfDrawer = 0x0;
}



void CRenderThread::ApplyCameraTransform() {
	if (wantTrack) {
		const vec3& bp = pfDrawer->GetCamPos();
		const vec3& bd = pfDrawer->GetCamDir();

		gluLookAt(
			bp.x,        bp.y,        bp.z,
			bp.x + bd.x, bp.y + bd.y, bp.z + bd.z,
			0.0f,        1.0f,        0.0f
		);
	} else {
		gluLookAt(
			cam->pos.x, cam->pos.y, cam->pos.z,
			cam->tar.x, cam->tar.y, cam->tar.z,
			cam->yDir.x, cam->yDir.y, cam->yDir.z
		);
	}
}

void CRenderThread::DrawCameraAxisSystem() {
	if (cam->GetMode() == CAM_AZIMUTH || true) {
		const vec3& tget = cam->azData.target;
		const vec3& xdir = cam->xDir;
		const vec3& ydir = cam->yDir;
		const vec3& zdir = cam->zDir;

		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
			glTranslatef(tget.x, tget.y, tget.z);
			glutSolidSphere(0.1f, 10, 10);
		glPopMatrix();

		glPushMatrix();
			glLineWidth(2.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_STRIP);
				glVertex3f(tget.x, tget.y, tget.z);
				glVertex3f(tget.x + xdir.x, tget.y + xdir.y, tget.z + xdir.z);
			glEnd();
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINE_STRIP);
				glVertex3f(tget.x, tget.y, tget.z);
				glVertex3f(tget.x + ydir.x, tget.y + ydir.y, tget.z + ydir.z);
			glEnd();
			glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_LINE_STRIP);
				glVertex3f(tget.x, tget.y, tget.z);
				glVertex3f(tget.x + zdir.x, tget.y + zdir.y, tget.z + zdir.z);
			glEnd();
		glPopMatrix();
	}
}

void CRenderThread::RenderFrame(GameState*) {
	// boost::mutex::scoped_lock lock(drawMutex);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// setting the clear-color is not a one-time operation
	glClearColor(0.01f, 0.01f, 0.01f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (wantLight && !lightEnabled) { EnableLight(); }
	if (!wantLight && lightEnabled) { DisableLight(); }

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ApplyCameraTransform();
	DrawCameraAxisSystem();

	psDrawer->Draw(simThread->GetParticleSystem());
	pfDrawer->Draw(simThread->GetPathFinder(), simThread->GetParticleSystem());

	SDL_GL_SwapBuffers();
}

void CRenderThread::Update(GameState* state) {
	if (gEngineData->useFSAA) { glEnable(GL_MULTISAMPLE_ARB); }
	if (gEngineData->wantDraw) { RenderFrame(state); }
	if (gEngineData->useFSAA) { glDisable(GL_MULTISAMPLE_ARB); }

	gEngineData->renderFPS += 1;
}






void CRenderThread::OnMouseMove(int button, int mx, int my) {
	button = mx = my;
}

void CRenderThread::OnMouseClick(bool press, int button, int mx, int my) {
	press = (button = mx = my);
}



vec3 CRenderThread::MouseToWorldCoors(int mx, int my) {
	// boost::mutex::scoped_lock lock(drawMutex);

	// need to be doubles
	double wcoors[3] = {0.0, 0.0, 0.0};
	double mviewMat[16] = {0.0};
	double mprojMat[16] = {0.0};
	int viewport[4] = {0};

	glGetDoublev(GL_PROJECTION_MATRIX, mprojMat);
	glGetDoublev(GL_MODELVIEW_MATRIX, mviewMat);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// 0 ==> zNear, 1 ==> zFar
	float mz = 1.0f;
	// mouse origin is at top-left, OGL window origin is at bottom-left
	int myy = viewport[3] - my;

	glReadPixels(mx, myy,  1, 1,  GL_DEPTH_COMPONENT, GL_FLOAT, &mz);
	gluUnProject(mx, myy, mz,  mviewMat, mprojMat, viewport,  &wcoors[0], &wcoors[1], &wcoors[2]);

	return vec3(float(wcoors[0]), float(wcoors[1]), float(wcoors[2]));
}



void CRenderThread::EnableLight(void) {
	GLfloat ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f};
	GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat specularLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat lightPos[] = {-100000.0f, 100000.0f, -100000.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// GLfloat specularReflection[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// GLfloat materialEmission[] = {0.2f, 0.1f, 0.2f, 1.0f};

	// disabling is required for the glMaterial() calls to work
	// glDisable(GL_COLOR_MATERIAL);
	// glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	// glMaterialfv(GL_FRONT, GL_SPECULAR, specularReflection);
	// glMateriali(GL_FRONT, GL_SHININESS, 128);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	// make the ambient front-material properties track glColor()
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	lightEnabled = true;
}

void CRenderThread::DisableLight(void) {
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	lightEnabled = false;
}
