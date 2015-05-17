#include <SDL/SDL.h>

#include "../System/Engine.hpp"
#include "../System/GEngine.hpp"
#include "./InputThread.hpp"

CInputThread::CInputThread() {
	keys = new ubyte[SDLK_LAST];
	memset(keys, 0, SDLK_LAST);

	lastMouseButton = -1;

	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY >> 2, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_SetModState(KMOD_NONE);
	SDL_ShowCursor(gEngineData->mouseLook? SDL_DISABLE: SDL_ENABLE);
	SDL_WM_GrabInput(gEngineData->mouseLook? SDL_GRAB_ON: SDL_GRAB_OFF);
}
CInputThread::~CInputThread() {
	delete keys; keys = 0x0;

	SDL_WM_GrabInput(SDL_GRAB_OFF);
}






// refresh the keyboard array
void CInputThread::UpdateKeys() {
	int numKeys = 0;
	Uint8* state = SDL_GetKeyState(&numKeys);
	const SDLMod mods = SDL_GetModState();

	memcpy(keys, state, sizeof(Uint8) * numKeys);

	keys[SDLK_LALT]   = (mods & KMOD_ALT)  ? 1: 0;
	keys[SDLK_LCTRL]  = (mods & KMOD_CTRL) ? 1: 0;
	keys[SDLK_LMETA]  = (mods & KMOD_META) ? 1: 0;
	keys[SDLK_LSHIFT] = (mods & KMOD_SHIFT)? 1: 0;
}

void CInputThread::Update() {
	// needed only in the multi-threaded world
	// boost::mutex::scoped_lock lock(inputMutex);
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_VIDEORESIZE: {
				printf("received SDL_VIDEORESIZE event\n");
				gEngine->SetWindowSize(event.resize.w, event.resize.h);
				gEngine->SetSDLWindowVideoMode();
				gEngine->SetOGLViewPortGeometry();
			} break;

			case SDL_VIDEOEXPOSE: {
				printf("received SDL_VIDEOEXPOSE event\n");
				// re-initialize the stencil
				gEngine->InitStencilBuffer();
				gEngine->SetOGLViewPortGeometry();
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


			case SDL_MOUSEMOTION: { gEngine->MouseMoved(&event); } break;
			case SDL_MOUSEBUTTONDOWN: { gEngine->MousePressed(&event); } break;
			case SDL_MOUSEBUTTONUP: { gEngine->MouseReleased(&event); } break;
			case SDL_SYSWMEVENT: {
				// Spring: mouseInputHandler->HandleSDLMouseEvent(event);
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
					SDL_ShowCursor(gEngineData->mouseLook? SDL_DISABLE: SDL_ENABLE);

					printf("mouse-look %s\n", gEngineData->mouseLook? "enabled": "disabled");
				}

				UpdateKeys();
				gEngine->KeyPressed(&event, repeat);
			} break;
			case SDL_KEYUP: {
				UpdateKeys();
				gEngine->KeyReleased(&event);
			} break;
		}
	}
}

// needed only in the multi-threaded world
void CInputThread::Run() {
	while (!quit) {
		Update();
	}
}

// needed in the multi-threaded world (to stop the thread)
void CInputThread::NotifyQuit() {
	quit = true;
}


bool CInputThread::IsKeyPressed(int k) {
	// needed only in the multi-threaded world
	// boost::mutex::scoped_lock lock(inputMutex);
	return keys[k];
}
