#ifndef SPN_CORE_H
#define SPN_CORE_H

#define MSF_GIF_DEFINED
#define MAX_APP_NAME_LEN 255
#include <functional>
#include <string>
#include <SDL3/SDL.h>

#ifdef MSF_GIF_DEFINED
#include "../external/msf_gif/msf_gif.h"
#endif

#include "spn_canvas.h"
#include "spn_image.h"
#include "spn_rfont.h"

namespace spn
{
	class SpinachCore
	{
	public:
		SpinachCore(unsigned int width, unsigned int height,
			std::string resourcesDir = "res/",
			std::function<void(Canvas* canvas)> updateAndRenderFn = nullptr,
			std::function<void(const SDL_Event* sdlEvent)> inputFn = nullptr
			);
		~SpinachCore();
		void SetUpdateAndRenderHandler(std::function<void(Canvas* canvas)>);
		void SetInputHandler(std::function<void(const SDL_Event* sdlEvent)>);
		void MainLoop();
		void RenderCanvas();
		void SaveScreenShot(const std::string& fileName);
		void WaitForEvents();
		inline bool IsInitSucceded(){
			return initializationResult == 0;
		}
		inline bool IsInitFailed(){
			return initializationResult != 0;
		}
		inline int GetInitializationResult() {
			return initializationResult;
		}
		inline Canvas* GetCanvas() {
			return canvas;
		}
		inline void SetUserWantsToQuit(int flag) {
			userWantsToQuit = flag;
		}
		inline void SetWindowTitle(const char* text) {
			if (nullptr == window) {
				return;
			}
			strcpy(appName, text);
			SDL_SetWindowTitle(window, text);
		}
		void SetTargetFramesPerSecond(unsigned int aFps);
		inline void LockFps(bool flag) {
			lockFps = flag;
		};
	private:
		char appName[MAX_APP_NAME_LEN + 1];
		bool lockFps;
		std::string resourcesDirectory;
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
		Image image;
		int initializationResult;
		RFont* font;
		Canvas *canvas;
		unsigned int targetFramesPerSecond;
		unsigned int targetMillisPerFrame;
		int userWantsToQuit;
		std::function<void(Canvas* canvas)> updateAndRenderHandler;
		std::function<void(const SDL_Event* sdlEvent)> inputHandler;
		int Init(unsigned int width, unsigned int height);

		void Destroy();
#ifdef MSF_GIF_DEFINED
		bool isRecording;
		MsfGifState msfGifState;
		int msfGifCentiSecondsPerFrame;
		int msfGifQuality;
		void StartRecording();
		void ProcessRecording();
		void StopRecording(bool saveData);
#endif
	};
}

#endif
