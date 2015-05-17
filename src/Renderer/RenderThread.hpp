#ifndef RENDERTHREAD_HPP
#define RENDERTHREAD_HPP

struct GameState;
class CSimThread;
class CParticleSystemDrawer;
class CPathFinderDrawer;
struct Camera;
struct vec3;

class CRenderThread {
	public:
		CRenderThread(const vec3&);
		~CRenderThread();

		void ApplyCameraTransform();
		void DrawCameraAxisSystem();
		void RenderFrame(GameState*);
		void Update(GameState*);

		void SetSimulationInstance(CSimThread* p) { simThread = p; }

		vec3 MouseToWorldCoors(int, int);

		void OnMouseMove(int, int, int);
		void OnMouseClick(bool, int, int, int);

		void ToggleLighting() { wantLight = !wantLight; }
		void ToggleTracking() { wantTrack = !wantTrack; }
		Camera* GetCamera() const { return cam; }

	private:
		void EnableLight();
		void DisableLight();

		CSimThread* simThread;
		Camera* cam;
		CParticleSystemDrawer* psDrawer;
		CPathFinderDrawer* pfDrawer;

		bool wantLight;
		bool wantTrack;
		bool lightEnabled;
		// boost::mutex drawMutex;
};

#endif
