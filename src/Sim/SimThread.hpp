#ifndef SIMTHREAD_HPP
#define SIMTHREAD_HPP

#include "../Common/CommonTypes.hpp"

class CRenderThread;
class CParticleSystem;
class CPathFinder;

class CSimThread {
	public:
		CSimThread(uint, uint);
		~CSimThread();

		void ChangeSpeed(uint);
		void SimFrame(uint*, uint*);
		bool Update(GameState*);

		bool IsPaused() const { return paused; }
		void TogglePause() { paused = !paused; }

		void SetRendererInstance(CRenderThread* p) { renderThread = p; }

		// these should not be here
		CParticleSystem* GetParticleSystem() const { return ps; }
		CPathFinder* GetPathFinder() const { return pf; }
	private:
		bool paused;

		uint frame;						// current frame
		uint frameTime;					// time taken to calculate sim-frame <f> (millisecs)
		int frameDelta;					// time left over after calculating <f> (millisecs, should be > 0)
		uint totalFrameTime;			// total time taken by all frames between realTime and prevRealTime
		uint avgFrameTime;				// average time per frame, set every real-time second
		uint realTime;					// number of real-time seconds elapsed so far
		uint prevRealTime;				// previous real-time snapshot
		uint startTime;					// program start-time
		uint gameTime;					// number of game-time seconds elapsed so far
		uint lastTick;
		int missedFrames;				// num. of frames delayed (due to high frameTime) by the last frame

		uint FRAMERATE;					// current simulation speed (number of simframes processed per real-time second)
		uint FRAMETIME;					// ideal maximum amount of time a single simframe may take at current speed
		uint FRAMEMULT;					// simulation speed multiplier

		CRenderThread* renderThread;
		CParticleSystem* ps;
		CPathFinder* pf;
};

#endif
