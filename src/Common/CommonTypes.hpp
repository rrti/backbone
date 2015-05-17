#ifndef COMMONTYPES_HDR
#define COMMONTYPES_HDR

typedef unsigned char ubyte;
typedef unsigned int uint;

struct GameState {
};

struct EngineData {
	EngineData(): frame(0), simFPS(0), renderFPS(0), runTime(0) {
		wantQuit = false; wantDraw = true;
		useFSAA = true; mouseLook = false;
		horFOV = 90.0f; verFOV = 45.0f;
	}

	uint frame;			// current game-time (expressed in simframes)
	uint simFPS;		// number of sim-frames that were processed last real-time second
	uint renderFPS;		// number of rendered frames last real-time second
	uint runTime;
	bool wantQuit;
	bool wantDraw;
	bool useFSAA;
	bool mouseLook;
	float horFOV;
	float verFOV;
};

#endif
