#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "../Common/CommonTypes.hpp"

class CSimThread;
class CRenderThread;
class CClient;
struct SDL_Surface;
union SDL_Event;

class CEngine {
	public:
		CEngine(int, char**);
		virtual ~CEngine();

		void Initialize();
		void Terminate();

		virtual void Run();
		virtual void Update();

		virtual void MouseMoved(SDL_Event*);
		virtual void MousePressed(SDL_Event*);
		virtual void MouseReleased(SDL_Event*);
		virtual void KeyPressed(SDL_Event*, bool);
		virtual void KeyReleased(SDL_Event*);

		bool EnableMultiSampling();
		bool VerifyMultiSampling();

		void InitStencilBuffer();
		void SetWindowSize(int, int);

		void SetSDLWindowVideoMode();
		void SetOGLViewPortGeometry();

		// CClient inherits this
		SDL_Surface* screen;
	private:
		void InitSDL(const char*);
		void InitOGL();

		// helper functions for SetOGLViewPortGeometry()
		void UpdateViewPortDimensions();
		bool UpdateWindowInfo();

		// run-time and startup options
		bool fullScreen;
		bool dualScreen;
		bool dualScreenMapLeft;
		bool lineSmoothing;
		bool pointSmoothing;

		uint desktopSizeX;			// width of entire desktop in pixels (screen resolution, gu->screenSizeX)
		uint desktopSizeY;			// height of entire desktop in pixels (screen resolution, gu->screenSizeY)
		uint windowSizeX;			// width of SDL/OGL window in pixels (resolution, gu->winSizeX)
		uint windowSizeY;			// height of SDL/OGL window in pixels (resolution, gu->winSizeY)
		uint windowPositionX;		// x-pos relative to bottom-left corner of desktop (gu->winPosX)
		uint windowPositionY;		// y-pos relative to bottom-left corner of desktop (gu->winPosY)
		uint viewPortSizeX;			// OGL viewport width in pixels (gu->viewSizeX)
		uint viewPortSizeY;			// OGL viewport height in pixels (gu->viewSizeY)
		int viewPortPositionX;		// x-pos relative to bottom-left corner of SDL/OGL window (gu->viewPosX)
		int viewPortPositionY;		// y-pos relative to bottom-left corner of SDL/OGL window (gu->viewPosY)

		float pixelSizeX;			// width of one pixel in viewport coordinates, 1 / viewPortSizeX
		float pixelSizeY;			// height of one pixel in viewport coordinates, 1 / viewPortSizeY

		// int SpringApp::screenW;	// game screen width (REDUNDANT, winSizeX)
		// int SpringApp::screenH;	// game screen height (REDUNDANT, winSizeY)

		uint bitsPerPixel;
		uint depthBufferBits;
		uint FSAALevel;				// should be even number between 0 and 8
};

#endif
