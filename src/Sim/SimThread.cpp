#include <SDL/SDL.h>

#include "../Common/CommonTypes.hpp"
#include "../System/GEngine.hpp"
#include "./SimThread.hpp"
#include "./ParticleSystem/ParticleSystem.hpp"
#include "./PathFinder/PathFinder.hpp"

CSimThread::CSimThread(uint frameRate, uint frameMult) {
	paused = false;

	frame = 0;
	frameTime = 0;
	frameDelta = 0;
	totalFrameTime = 0;
	avgFrameTime = 0;
	realTime = 0;
	prevRealTime = 0;
	startTime = SDL_GetTicks();
	gameTime = 0;
	lastTick = 0;
	missedFrames = 0;

	FRAMERATE = frameRate;
	FRAMETIME = 1000 / FRAMERATE;
	FRAMEMULT = frameMult;

	pf = new CPathFinder(25, 25, 25);
	pf->Reset();
	ps = new CParticleSystem(32);
}

CSimThread::~CSimThread() {
	delete ps; ps = 0x0;
	delete pf; pf = 0x0;
}


void CSimThread::ChangeSpeed(uint mult) {
	FRAMEMULT = mult;
	FRAMETIME = 1000 / (FRAMERATE * FRAMEMULT);
}


void CSimThread::SimFrame(uint* frame, uint* ftime) {
	(*frame) += 1;
	(*ftime) = SDL_GetTicks();

	pf->update();
	ps->Update(1.0f / (FRAMERATE * FRAMEMULT), pf);

	(*ftime) = (SDL_GetTicks() - (*ftime));
}

bool CSimThread::Update(GameState*) {
	bool updated = false;

	if (frameDelta <= 0 || missedFrames > 0 /*|| gameTime < realTime*/) {
		// previous frame took longer than FRAMETIME (or it is just
		// time for a new one); if we missed any "scheduled" frames
		// due to some unusually long frameTime catch up by creating
		// a new one too
		if (!paused) {
			SimFrame(&frame, &frameTime);

			gEngineData->simFPS += 1;
			gEngineData->frame += 1;
			gameTime = frame / FRAMERATE;					// game-time is based on number of elapsed frames
			missedFrames = std::max(0, missedFrames - 1);	// we made up one frame of our backlog
			missedFrames += (frameTime / FRAMETIME);		// update how many frames we are still "behind"
			updated = true;
		} else {
			frameTime = 0;
		}

		lastTick = SDL_GetTicks();
		realTime = (lastTick - startTime) / 1000;			// update the program's real running time
		frameDelta = FRAMETIME - frameTime;					// how much time we had left to complete this frame
		totalFrameTime += frameTime;						// update the cumulative frame-time

		if ((realTime - prevRealTime) >= 1) {
			// (at least) one real-time second's worth of sim-frames passed
			// which together took <totalFrameTime> milliseconds to calculate,
			// if framerate was not changed then simFPS will be APPROXIMATELY
			// equal to <FRAMERATE> but in general we do not know how many
			// have been calculated between <prevRealTime> and <realTime>
			// (so totalFrameTime cannot be divided by fixed <FRAMERATE>)
			//
			// note: checking if <totalFrameTime> >= 1000 is pointless since
			// bigger framerate means totalFrameTime will increase faster (so
			// correspondence with true running time <realTime> lost, updates
			// will then occur every 1000 / <per-frame time> frames)
			prevRealTime = realTime;
			avgFrameTime = (gEngineData->simFPS > 0)? (totalFrameTime / gEngineData->simFPS): 0;
			totalFrameTime = 0;

			// start a new measure-point
			gEngineData->runTime = realTime;
			gEngineData->simFPS = 0;
			gEngineData->renderFPS = 0;
		}
	} else {
		// last frame took less than FRAMETIME, if it is time for a
		// new one then reset frameDelta to 0 so next Update() call
		// will create one
		if (SDL_GetTicks() >= (lastTick + frameDelta)) {
			frameDelta = 0;
		}
	}

	return updated;
}
