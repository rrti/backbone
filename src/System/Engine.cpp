#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include <cmath>
#include <ctime>
#include <cassert>

#include "./Engine.hpp"
#include "./GEngine.hpp"
#include "./Client.hpp"
#include "../Math/Trig.hpp"

CEngine::CEngine(int argc, char** argv) {
	// TODO: parse the command-line, etc
	// needed for glutSolidSphere() only
	glutInit(&argc, argv);

	gEngineData = new EngineData();
	gEngineData->wantQuit = false;
	gEngineData->wantDraw = true;
	gEngineData->useFSAA = true;

	fullScreen = false;
	dualScreen = false;
	// dualScreenMapLeft = true;
	// dualScreenMapLeft = dualScreen? GetInt("DualScreenMMapLeft", 0): false;
	lineSmoothing = true;
	pointSmoothing = true;

	desktopSizeX = 0;
	desktopSizeY = 0;
	// default window resolution when not read from config
	windowSizeX = 1024;
	windowSizeY =  768;
	windowPositionX = 0;
	windowPositionY = 0;
	viewPortSizeX = 0;
	viewPortSizeY = 0;
	viewPortPositionX = 0;
	viewPortPositionY = 0;

	bitsPerPixel = 32;
	depthBufferBits = 24;
	FSAALevel = 4;

	// this needs to be called here, since CClient's constructor
	// immediately proceeeds with the CClient part (ie. including
	// CInputThread which depends on SDL having been initialized)
	Initialize();
}

CEngine::~CEngine() {
	Terminate();
}



void CEngine::Initialize() {
	InitSDL("LHC Engine");
	InitOGL();

	SetSDLWindowVideoMode();
	SetOGLViewPortGeometry();
}

void CEngine::Terminate() {
	delete gEngineData; gEngineData = 0x0;

	SDL_Quit();
}



void CEngine::Run() {}
void CEngine::Update() {}

void CEngine::MouseMoved(SDL_Event*) {}
void CEngine::MousePressed(SDL_Event*) {}
void CEngine::MouseReleased(SDL_Event*) {}
void CEngine::KeyPressed(SDL_Event*, bool) {}
void CEngine::KeyReleased(SDL_Event*) {}



void CEngine::SetWindowSize(int w, int h) {
	windowSizeX = w;
	windowSizeY = h;
}

void CEngine::InitStencilBuffer() {
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT); SDL_GL_SwapBuffers();
	glClear(GL_STENCIL_BUFFER_BIT); SDL_GL_SwapBuffers();
}



void CEngine::InitSDL(const char* caption) {
	// initialize the window (one-time operation)
	// SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
	SDL_WM_SetCaption(caption, NULL);
}

// called on resize
void CEngine::SetSDLWindowVideoMode() {
	// set the video mode (32 bits per pixel, etc)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   depthBufferBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	// this needs to be done prior to calling SetVideoMode()
	gEngineData->useFSAA = EnableMultiSampling(); FSAALevel = gEngineData->useFSAA? FSAALevel: 0;
	gEngineData->useFSAA = VerifyMultiSampling(); FSAALevel = gEngineData->useFSAA? FSAALevel: 0;

	screen = SDL_SetVideoMode(windowSizeX, windowSizeY, bitsPerPixel,
		SDL_OPENGL | SDL_RESIZABLE | SDL_HWSURFACE | SDL_DOUBLEBUF | (fullScreen? SDL_FULLSCREEN: 0));

	assert(screen != 0x0);

	gEngineData->verFOV = 45.0f;
//	gEngineData->verFOV = 2 * RTOD(atan(screen->h * tan(gEngineData->horFOV * 0.5f) / screen->w));
	gEngineData->horFOV = 2 * RTOD(atan(screen->w * tan(gEngineData->verFOV * 0.5f) / screen->h));

	// this should not have to be done when resizing
	// InitStencilBuffer();

	// these should be equal to our bitsPerPixel member
	// SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &bits);
	// SDL_PixelFormat* f = screen->format;

	printf("SDL video-mode set to %ux%ux%u\n", windowSizeX, windowSizeY, screen->format->BitsPerPixel);
}



void CEngine::InitOGL() {
	// initialize the color, depth, and stencil buffers
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// enable depth-testing
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0f, 1.0f);
	glDepthMask(GL_TRUE);

	// enable alpha-blending and auto-normalization
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);

	// enable backface-culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// default winding is already CCW
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT, GL_FILL);
	glShadeModel(GL_FLAT);

	if (lineSmoothing) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	if (pointSmoothing) {
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}

	// glEnable(GL_POLYGON_SMOOTH);
}

// called on resize
void CEngine::SetOGLViewPortGeometry() {
	UpdateViewPortDimensions();

	printf("viewPortPositionX, viewPortPositionY, viewPortSizeX, viewPortSizeY: %u, %u, %u, %u\n",
		viewPortPositionX, viewPortPositionY, viewPortSizeX, viewPortSizeY);

	glViewport(viewPortPositionX, viewPortPositionY, viewPortSizeX, viewPortSizeY);

	const float yFieldOfView = 45.0f;
	const float aspectRatio = viewPortSizeX / float(viewPortSizeY);
	const float zNearDist = 0.125f;
	const float zFarDist = 512.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(yFieldOfView, aspectRatio, zNearDist, zFarDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// Linux-only
bool CEngine::UpdateWindowInfo() {
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (!SDL_GetWMInfo(&info)) {
		// we can't get information, so assume desktop is as
		// wide as "screenWidth" and as high as "screenHeight"
		// which are the current window dimensions
		desktopSizeX = windowSizeX;
		desktopSizeY = windowSizeY;

		windowPositionX = 0;
		windowPositionY = 0;
		return false;
	}

	info.info.x11.lock_func();

	Display* display = info.info.x11.display;
	Window window = info.info.x11.window;

	XWindowAttributes attrs;
	XGetWindowAttributes(display, window, &attrs);

	desktopSizeX = WidthOfScreen(attrs.screen);
	desktopSizeY = HeightOfScreen(attrs.screen);
	// NOTE: why do we need this when SDL tells us
	// the new windowSizeX and windowSizeY already?
	windowSizeX = attrs.width;
	windowSizeY = attrs.height;

	Window tmp;
	int xp, yp;
	XTranslateCoordinates(display, window, attrs.root, 0, 0, &xp, &yp, &tmp);

	windowPositionX = xp;
	windowPositionY = desktopSizeY - windowSizeY - yp;

	printf("desktop resolution: %ux%u, window resolution: %ux%u, window position: %ux%u\n",
		desktopSizeX, desktopSizeY, windowSizeX, windowSizeY, windowPositionX, windowPositionY);

	return true;
}

void CEngine::UpdateViewPortDimensions() {
	UpdateWindowInfo();

	if (!dualScreen) {
		viewPortSizeX = windowSizeX;
		viewPortSizeY = windowSizeY;
		viewPortPositionX = 0;
		viewPortPositionY = 0;
	} else {
		viewPortSizeX = windowSizeX >> 1;
		viewPortSizeY = windowSizeY;

		if (dualScreenMapLeft) {
			viewPortPositionX = windowSizeX >> 1;
			viewPortPositionY = 0;
		} else {
			viewPortPositionX = 0;
			viewPortPositionY = 0;
		}
	}

	pixelSizeX = 1.0f / float(viewPortSizeX);
	pixelSizeY = 1.0f / float(viewPortSizeY);
}






bool CEngine::EnableMultiSampling(void) {
	if (!GL_ARB_multisample)
		return false;

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, FSAALevel);
	glEnable(GL_MULTISAMPLE);
	printf("EnableMultiSampling(): true\n");
	return true;
}

bool CEngine::VerifyMultiSampling(void) {
	GLint buffers, samples;
	glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, &buffers);
	glGetIntegerv(GL_SAMPLES_ARB, &samples);
	printf("VerifyMultiSampling(): %d\n", (buffers > 0 && samples > 0));
	return (buffers > 0 && samples > 0);
}
