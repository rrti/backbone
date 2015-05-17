#include <SDL/SDL.h>
#include <math.h>

#include "../Sim/SimThread.hpp"
#include "../Sim/ParticleSystem/ParticleSystem.hpp"
#include "../Sim/PathFinder/PathFinder.hpp"
#include "../Renderer/RenderThread.hpp"
#include "../Renderer/Camera.hpp"
#include "../Input/InputThread.hpp"
#include "../System/GEngine.hpp"
#include "../Math/vec3.hpp"
#include "./Client.hpp"

CClient::CClient(int argc, char** argv): CEngine(argc, argv) {
	// "pseudo-threads" which are updated sequentially
	simThread = new CSimThread(25, 1);
	renderThread = new CRenderThread(simThread->GetPathFinder()->GetWorldSize());

	simThread->SetRendererInstance(renderThread);
	renderThread->SetSimulationInstance(simThread);

	InitInput();

	SetLastMouseButton(-1);
	SetLastMouseCoors(-1, -1);
}
CClient::~CClient() {
	delete simThread; simThread = 0x0;
	delete renderThread; renderThread = 0x0;

	KillInput();
}


void CClient::Update() {
	UpdateInput();
	PollInput();
	simThread->Update(&state);
	renderThread->Update(&state);
}

void CClient::Run() {
	while (!gEngineData->wantQuit) {
		Update();
	}
}



void CClient::InitInput() {
	keys = new ubyte[SDLK_LAST];
	memset(keys, 0, SDLK_LAST);

	lastMouseButton = -1;

	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY >> 2, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_SetModState(KMOD_NONE);
	SDL_ShowCursor(gEngineData->mouseLook? SDL_DISABLE: SDL_ENABLE);
	SDL_WM_GrabInput(gEngineData->mouseLook? SDL_GRAB_ON: SDL_GRAB_OFF);
}

void CClient::KillInput() {
	delete keys; keys = 0x0;

	SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void CClient::UpdateInput() {
	// needed only in the multi-threaded world
	// boost::mutex::scoped_lock lock(inputMutex);
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_VIDEORESIZE: {
				printf("received SDL_VIDEORESIZE event\n");
				SetWindowSize(event.resize.w, event.resize.h);
				SetSDLWindowVideoMode();
				SetOGLViewPortGeometry();
			} break;

			case SDL_VIDEOEXPOSE: {
				printf("received SDL_VIDEOEXPOSE event\n");
				// re-initialize the stencil
				InitStencilBuffer();
				SetOGLViewPortGeometry();
			} break;

			case SDL_QUIT: {
				gEngineData->wantQuit = true;
			} break;

			case SDL_ACTIVEEVENT: {
				if (event.active.state & SDL_APPACTIVE) {
					gEngineData->wantDraw = !!event.active.gain;
				}
				break;
			}


			case SDL_MOUSEMOTION: { MouseMoved(&event); } break;
			case SDL_MOUSEBUTTONDOWN: { MousePressed(&event); } break;
			case SDL_MOUSEBUTTONUP: { MouseReleased(&event); } break;
			case SDL_SYSWMEVENT: {
				// unhandled event received from WM
				break;
			}

			case SDL_KEYDOWN: {
				// check if key <i> was already pressed
				int i = event.key.keysym.sym;
				bool repeat = !!keys[i];

				if (i == SDLK_ESCAPE) {
					event.type = SDL_QUIT;
					event.quit.type = SDL_QUIT;
					SDL_PushEvent(&event);

					gEngineData->wantQuit = true;
				}

				if (i == SDLK_g) {
					int grabMode = SDL_WM_GrabInput(SDL_GRAB_QUERY);

					if (grabMode == SDL_GRAB_ON ) { SDL_WM_GrabInput(SDL_GRAB_OFF); printf("input-grabbing off\n"); }
					if (grabMode == SDL_GRAB_OFF) { SDL_WM_GrabInput(SDL_GRAB_ON ); printf("input-grabbing on\n"); }
				}

				if (i == SDLK_m) {
					gEngineData->mouseLook = !gEngineData->mouseLook;
					renderThread->GetCamera()->SetMode(gEngineData->mouseLook? CAM_FPS: CAM_AZIMUTH);
					SDL_ShowCursor(gEngineData->mouseLook? SDL_DISABLE: SDL_ENABLE);

					printf("mouse-look %s\n", gEngineData->mouseLook? "enabled": "disabled");
				}

				UpdateKeys();
				KeyPressed(&event, repeat);
			} break;
			case SDL_KEYUP: {
				UpdateKeys();
				KeyReleased(&event);
			} break;
		}
	}
}

// refresh the keyboard array
void CClient::UpdateKeys() {
	int numKeys = 0;
	Uint8* state = SDL_GetKeyState(&numKeys);
	const SDLMod mods = SDL_GetModState();

	memcpy(keys, state, sizeof(Uint8) * numKeys);

	keys[SDLK_LALT]   = (mods & KMOD_ALT)  ? 1: 0;
	keys[SDLK_LCTRL]  = (mods & KMOD_CTRL) ? 1: 0;
	keys[SDLK_LMETA]  = (mods & KMOD_META) ? 1: 0;
	keys[SDLK_LSHIFT] = (mods & KMOD_SHIFT)? 1: 0;
}

bool CClient::IsKeyPressed(int k) {
	// needed only in the multi-threaded world
	// boost::mutex::scoped_lock lock(inputMutex);
	return keys[k];
}

void CClient::PollInput() {
	if (gEngineData->mouseLook && gEngineData->frame >= 2) {
		// HACK: filter out bogus input on startup
		if (GetLastMouseButton() == SDL_BUTTON_LEFT) { renderThread->GetCamera()->Adjust(CAM_MOVE, 1, 0.05f); }
		if (GetLastMouseButton() == SDL_BUTTON_RIGHT) { renderThread->GetCamera()->Adjust(CAM_MOVE, -1, 0.05f); }
	}

	if (gEngineData->frame >= 2) {
		// TODO: make the scalars dependent on (avg) renderer framerate
		// const float s = 1.0f / gEngineData->renderFPS;
		if (renderThread->GetCamera()->GetMode() == CAM_FPS) {
			if (IsKeyPressed(SDLK_LALT))  { renderThread->GetCamera()->Adjust(CAM_VSTRAFE,  1, 0.01f); }
			if (IsKeyPressed(SDLK_LCTRL)) { renderThread->GetCamera()->Adjust(CAM_VSTRAFE, -1, 0.01f); }

			if (IsKeyPressed(SDLK_w)) { renderThread->GetCamera()->Adjust(CAM_MOVE,  1, 0.0125f); }
			if (IsKeyPressed(SDLK_s)) { renderThread->GetCamera()->Adjust(CAM_MOVE, -1, 0.0125f); }
			if (IsKeyPressed(SDLK_a)) { renderThread->GetCamera()->Adjust(CAM_HSTRAFE, -1, 0.0125f); }
			if (IsKeyPressed(SDLK_d)) { renderThread->GetCamera()->Adjust(CAM_HSTRAFE,  1, 0.0125f); }
			if (IsKeyPressed(SDLK_q)) { renderThread->GetCamera()->Adjust(CAM_ROLL, -1, 0.00125f); }
			if (IsKeyPressed(SDLK_e)) { renderThread->GetCamera()->Adjust(CAM_ROLL,  1, 0.00125f); }
		}

	}
}



void CClient::MouseMoved(SDL_Event* e) {
	if (gEngineData->frame <= 2) {
		// HACK: filter out bogus input on startup
		return;
	}

	Camera* cam = renderThread->GetCamera();

	if (gEngineData->mouseLook) {
		const int xmotion = e->motion.xrel;
		const int ymotion = e->motion.yrel;
		const float xfrac = fabsf(xmotion / gEngineData->horFOV);	// wider horFOV means horizontal motion is less wild
		const float yfrac = fabsf(ymotion / gEngineData->verFOV);	// wider verFOV means vertical motion is less wild
		const int xdir = (xmotion > 0)? 1: -1;
		const int ydir = (ymotion < 0)? 1: -1;

		if (xmotion != 0) { cam->Adjust(CAM_YAW,   xdir, xfrac); }
		if (ymotion != 0) { cam->Adjust(CAM_PITCH, ydir, yfrac); }

		// re-center the mouse and eat the event it generates
		SDL_WarpMouse(screen->w >> 1, screen->h >> 1);
		SDL_PollEvent(e);
	} else {
		renderThread->OnMouseMove((e->button).button, e->motion.x, e->motion.y);
		// vec3 wcoors = renderThread->MouseToWorldCoors(e->motion.x, e->motion.y);
		// simThread->GetParticleSystem()->UpdateDragging(wcoors);

		if (lastMouseButton == SDL_BUTTON_LEFT) {
			cam->azData.UpdateER(lastMouseX - e->motion.x, lastMouseY - e->motion.y);
			cam->UpdateAxisSystem(true);
		}
		if (lastMouseButton == SDL_BUTTON_RIGHT) {
			cam->azData.UpdateZ(lastMouseY - e->motion.y);
			cam->UpdateAxisSystem(false);
		}

		if (lastMouseButton == SDL_BUTTON_MIDDLE || lastMouseButton == 8) {
			const int xmotion = e->motion.xrel;
			const int ymotion = e->motion.yrel;
			const float xfrac = fabsf(xmotion / gEngineData->horFOV);
			const float yfrac = fabsf(ymotion / gEngineData->verFOV);
			const int xdir = (xmotion > 0)? 1: -1;
			const int ydir = (ymotion < 0)? 1: -1;

			cam->Adjust(CAM_HSTRAFE,  xdir, xfrac * 4.0f);
			cam->Adjust(CAM_VSTRAFE,  ydir, yfrac * 4.0f);
		}
	}
}

void CClient::MousePressed(SDL_Event* e) {
	SetLastMouseButton((e->button).button);
	SetLastMouseCoors(e->motion.x, e->motion.y);

	if (!gEngineData->mouseLook) {
		vec3 wcoors = renderThread->MouseToWorldCoors(e->motion.x, e->motion.y);
		renderThread->OnMouseClick(true, (e->button).button, e->motion.x, e->motion.y);

		switch ((e->button).button) {
			case SDL_BUTTON_LEFT: {
				renderThread->GetCamera()->azData.SaveERZ(true);
			} break;

			case SDL_BUTTON_RIGHT: {
				// TODO: queue up this request for the next simframe?
				// (when system is next updated, let it process them)
				renderThread->GetCamera()->azData.SaveERZ(false);
			} break;
		}
	}
}

void CClient::MouseReleased(SDL_Event* e) {
	SetLastMouseButton(-1);

	if (!gEngineData->mouseLook) {
		renderThread->OnMouseClick(false, (e->button).button, e->motion.x, e->motion.y);

		// vec3 wcoors = renderThread->MouseToWorldCoors(e->motion.x, e->motion.y);
	}
}



void CClient::KeyPressed(SDL_Event* e, bool) {
	if (e->key.keysym.sym == SDLK_PAUSE) {
		simThread->TogglePause();
	}
	if (e->key.keysym.sym == SDLK_r) {
		simThread->GetParticleSystem()->Reset();
		simThread->GetPathFinder()->Reset();
	}
	if (e->key.keysym.sym == SDLK_s) {
		simThread->GetPathFinder()->search(1.5f, 3.0f);
		simThread->GetParticleSystem()->InitParticles(simThread->GetPathFinder());
	}
	if (e->key.keysym.sym == SDLK_l) { renderThread->ToggleLighting(); }
	if (e->key.keysym.sym == SDLK_t) { renderThread->ToggleTracking(); }
	if (e->key.keysym.sym == SDLK_v) { simThread->GetPathFinder()->toggleShowVisitedNodes(); }
	if (e->key.keysym.sym == SDLK_b) { simThread->GetPathFinder()->toggleShowBlockedNodes(); }
	if (e->key.keysym.sym == SDLK_p) { simThread->GetPathFinder()->toggleShowBackBonePath(); }
}

void CClient::KeyReleased(SDL_Event*) {
}
