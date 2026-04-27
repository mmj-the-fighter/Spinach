#ifndef SPN_CORE_H
#define SPN_CORE_H

#define MSF_GIF_DEFINED
#define MAX_APP_NAME_LEN 255
#define DEFAULTFPS 64

#include <functional>
#include <SDL3/SDL.h>

#ifdef MSF_GIF_DEFINED
#include "../external/msf_gif/msf_gif.h"
#endif


namespace spn
{
	class Canvas;
	class RFont;
	class Image;

	class SpinachCore
	{
	public:
		SpinachCore() = default;
		~SpinachCore();
		bool Init(unsigned int width, unsigned int height, const char* fontDir);
		void SetUpdateAndRenderHandler(std::function<void(Canvas* canvas)>);
		void SetInputHandler(std::function<void(const SDL_Event* sdlEvent)>);
		void MainLoop();
		void RenderCanvas();
		void SaveScreenShot(const char* fileName);
		void WaitForEvents();
		inline int GetInitializationResult() {
			return initializationResult;
		}
		inline Canvas* GetCanvas() {
			return canvas;
		}
		inline void SetUserWantsToQuit(bool flag) {
			userWantsToQuit = flag;
		}
		inline void SetWindowTitle(const char* text) {
			if (nullptr == window) {
				return;
			}
			strcpy(appName, text);
			SDL_SetWindowTitle(window, text);
		}
		void SetTargetFramesPerSecond(int aFps);
		inline void LockFps(bool flag) {
			lockFps = flag;
		};
		void Destroy();
	private:
		char appName[MAX_APP_NAME_LEN + 1];
		bool lockFps = false;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
		Image* image;
		int initializationResult=0;
		RFont* font=nullptr;
		Canvas *canvas=nullptr;
		unsigned int targetFramesPerSecond=0;
		unsigned int targetMillisPerFrame=0;
		bool userWantsToQuit=false;
		std::function<void(Canvas* canvas)> updateAndRenderHandler=nullptr;
		std::function<void(const SDL_Event* sdlEvent)> inputHandler=nullptr;
		

#ifdef MSF_GIF_DEFINED
		bool isRecording = false;
		int recordingPingPongFrame = DEFAULTFPS / 2;
		int recordingPingPongMaxFrameCount = DEFAULTFPS / 2;
		MsfGifState msfGifState;
		int msfGifCentiSecondsPerFrame=0;
		int msfGifQuality=16;
		void StartRecording();
		void ProcessRecording();
		void StopRecording(bool saveData);
#endif
	};
}

#endif
