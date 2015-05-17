#ifndef INPUTTHREAD_HPP
#define INPUTTHREAD_HPP

// #include <boost/thread/mutex.hpp>
#include "../Common/CommonTypes.hpp"

class CInputThread {
	public:
		CInputThread();
		~CInputThread();

		void Update();
		void Run();
		void NotifyQuit();

		bool IsKeyPressed(int);
		void SetLastMouseButton(int b) { lastMouseButton = b; }
		int GetLastMouseButton() { return lastMouseButton; }

	private:
		void UpdateKeys();

		ubyte* keys;
		int lastMouseButton;

		volatile bool quit;
		// boost::mutex inputMutex;
};

#endif
