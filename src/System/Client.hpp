#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Common/CommonTypes.hpp"
#include "./Engine.hpp"

class CSimThread;
class CRenderThread;
union SDL_Event;

class CClient: public CEngine {
	public:
		CClient(int, char**);
		~CClient();

		void Update();
		void Run();

		void MouseMoved(SDL_Event*);
		void MousePressed(SDL_Event*);
		void MouseReleased(SDL_Event*);
		void KeyPressed(SDL_Event*, bool);
		void KeyReleased(SDL_Event*);

		bool IsKeyPressed(int);
		void SetLastMouseButton(int b) { lastMouseButton = b; }
		void SetLastMouseCoors(int x, int y) { lastMouseX = x; lastMouseY = y; }
		int GetLastMouseButton() { return lastMouseButton; }

	private:
		void InitInput();
		void KillInput();
		void UpdateInput();
		void PollInput();
		void UpdateKeys();

		CSimThread* simThread;
		CRenderThread* renderThread;

		ubyte* keys;
		int lastMouseButton;
		int lastMouseX;
		int lastMouseY;

		GameState state;
};

#endif
