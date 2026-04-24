#include <iostream>
#include <algorithm>
#include "../common/spn_utils.h"
#define MSF_GIF_IMPL
#include "spn_core.h"


//#define SHOWFRAMESTATS
#define SINGLEMEMCOPY
#define DEFAULTFPS 64



namespace spn
{
	SpinachCore::~SpinachCore()
	{
		Destroy();
	}

	void SpinachCore::SetTargetFramesPerSecond(unsigned int aFps)
	{
		int fps = (aFps == 0) ? 1 : aFps;
		targetFramesPerSecond = fps;
		targetMillisPerFrame = 1000.0f / static_cast<float>(fps);
	}


	bool SpinachCore::Init(unsigned int width, unsigned int height, const char* fontDir)
	{
		SetTargetFramesPerSecond(DEFAULTFPS);
		strcpy(appName, " ");

		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			std::cout << "Couldn't initialize SDL:" << SDL_GetError() << '\n';
			initializationResult = 1;
			return false;
		}

		if (!SDL_CreateWindowAndRenderer(
			"Spinach App",
			width,
			height,
			0,
			&window,
			&renderer))
		{
			std::cout << "Couldn't create window or renderer:" << SDL_GetError() << '\n';
			initializationResult = 2;
			return false;
		}

		if (!SDL_TextInputActive(window))
		{
			SDL_StartTextInput(window);
		}

		texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			width,
			height);
		if (!texture)
		{
			std::cout << "Couldn't create streaming texture: " << SDL_GetError() << '\n';
			initializationResult = 3;
			return false;
		}
		std::string atlasName = fontDir;
		atlasName.append("TrueNoFontAtlas.ppm");
		std::string csvName = fontDir;
		csvName.append("TrueNoFontData.csv");
		font = new RFont(atlasName.c_str(), csvName.c_str());
		if (!font->IsInitSucceded()){
			initializationResult = 4;
			std::cout << "Couldn't create font from " << fontDir << '\n';
			return false;
		}
		canvas = new Canvas(width, height);
		canvas->SetFont(font);
		initializationResult = 0;
		return true;
	}

	void SpinachCore::SetUpdateAndRenderHandler(std::function<void(Canvas*)> aUpdateAndRenderHandler)
	{
		updateAndRenderHandler = aUpdateAndRenderHandler;
	}

	void SpinachCore::SetInputHandler(std::function<void(const SDL_Event* sdlEvent)> aInputHandler){
		inputHandler = aInputHandler;
	}

	void SpinachCore::MainLoop()
	{
		SDL_Event event;
		SDL_zero(event);
		Uint32 frameStartTime, frameProcTime = 0, waitTime = 0;
#ifdef SHOWFRAMESTATS
		Uint32 minFrameProcTime = 1000000;
		Uint32 maxFrameProcTime = 0;
#endif
		int height = canvas->GetHeight();
		int pitch = canvas->GetPitch();
		int bufferBytesLength = height * pitch;
		unsigned char* pixels = canvas->GetPixelBuffer();
		userWantsToQuit = false;
		
		while (!userWantsToQuit)
		{
			frameStartTime = SDL_GetTicks();
			canvas->SetLastFrameTime(static_cast<float>(frameProcTime + waitTime) / 1000.0f);
			while (SDL_PollEvent(&event))
			{
				switch (event.type){
				case SDL_EVENT_QUIT:
					userWantsToQuit = true;
#ifdef SHOWFRAMESTATS
					std::cout << "min: " << minFrameProcTime << "\tcur:" << frameProcTime << "\tmax:" << maxFrameProcTime << std::endl;
#endif
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.key)
					{
					case SDLK_ESCAPE:
						SetTargetFramesPerSecond(1000);
						LockFps(false);
						userWantsToQuit = true;
						break;
					case SDLK_F12:
						{
							char fileName[256];
							GetFilenameFromCurrentTime(fileName, "PIC_SECONDS", ".png");
							SaveScreenShot(fileName);
						}
						break;
#ifdef MSF_GIF_DEFINED
					case SDLK_F8:
						if (!isRecording) {
							StartRecording();
						}
						break;
					case SDLK_F10:
						if (isRecording) {
							StopRecording(true);
						}
						break;
					case SDLK_F6:
						if (isRecording) {
							StopRecording(false);
						}
						break;
#endif
					}
				default:
					if (nullptr != inputHandler){
						inputHandler(&event);
					}
					break;
				}
			}

			if (nullptr != updateAndRenderHandler) {
				updateAndRenderHandler(canvas);
			}

#ifdef MSF_GIF_DEFINED
			if (isRecording) {
				if (userWantsToQuit) {
					StopRecording(false);
				}
				else {
					ProcessRecording();
				}
			}
#endif
			unsigned char* destPixels;
			int destPitch;
			if (SDL_LockTexture(texture, NULL, (void**)&destPixels, &destPitch))
			{
#ifdef SINGLEMEMCOPY
				memcpy(destPixels, pixels, bufferBytesLength);
#else
				for (int y = 0; y < height; ++y) {
					memcpy(destPixels + y * destPitch, pixels + y * pitch, pitch);
				}
#endif
				SDL_UnlockTexture(texture);
			}
			SDL_RenderTexture(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			frameProcTime = (SDL_GetTicks() - frameStartTime);
#ifdef SHOWFRAMESTATS
			if (frameProcTime < minFrameProcTime){
				minFrameProcTime = frameProcTime;
			}
			else if (frameProcTime > maxFrameProcTime){
				maxFrameProcTime = frameProcTime;
			}
			std::cout << frameProcTime << "\n";
#endif
			
			if (lockFps && frameProcTime < targetMillisPerFrame) {
				waitTime = targetMillisPerFrame - frameProcTime;
				SDL_Delay(waitTime);
			}
			else {
				waitTime = 0;
			}
			
		}
	}

#ifdef MSF_GIF_DEFINED
	void SpinachCore::StartRecording() {
		msfGifState = {};
		msf_gif_bgra_flag = true;
		//msf_gif_alpha_threshold = 128;
		msf_gif_begin(&msfGifState, canvas->GetWidth(), canvas->GetHeight());
		std::cout << "Recording started...\n";
		isRecording = true;
	}

	void SpinachCore::ProcessRecording() {
		//////////////////////////////////
		//This portion of the code does a ping pong of the value DEFAULTFPS/2
		//between -DEFAULTFPS/2 to DEFAULTFPS/2
		//for giving a flickering notification of recording
		static int frames = DEFAULTFPS / 2;
		static int maxFrames = DEFAULTFPS / 2;
		static const int framesCap = 25;
		if (frames == maxFrames) {
			SDL_SetWindowTitle(window, "Recording...");
		}
		else if (frames == -1) {
			SDL_SetWindowTitle(window, " ");
		}
		--frames;
		if (frames < -maxFrames) {
			frames = maxFrames;
		}
		//////////////////////////////////////////////
		//This portion of the code set the gif rendering speed and 
		//records the frame
		float frameTime = canvas->GetLastFrameTime();
		if (frameTime < 0.0001) {
				frames = maxFrames;
		}
		else {
			maxFrames = std::min(
				static_cast<int>(1.0 / frameTime),
				framesCap
			);
		}
		msfGifCentiSecondsPerFrame = canvas->GetLastFrameTime() * 100;
		msfGifCentiSecondsPerFrame = std::max(2, msfGifCentiSecondsPerFrame);
		msf_gif_frame(&msfGifState, canvas->GetPixelBuffer(),
			msfGifCentiSecondsPerFrame, msfGifQuality,
			canvas->GetWidth() * 4);
	}

	void SpinachCore::StopRecording(bool saveData) {
		
		MsfGifResult msfGifResult = msf_gif_end(&msfGifState);
		if (msfGifResult.data != NULL && saveData==true) {
			char fileName[256];
			GetFilenameFromCurrentTime(fileName, "REC_SECONDS", ".gif");
			FILE* fp = fopen(fileName, "wb");
			fwrite(msfGifResult.data, msfGifResult.dataSize, 1, fp);
			fclose(fp);
			std::cout << "Recording ended: session saved to " <<fileName << '\n';
		}
		else if (!saveData) {
			std::cout << "Recording aborted." << '\n';
		}
		msf_gif_free(msfGifResult);
		isRecording = false;
		SDL_SetWindowTitle(window, appName);
	}
#endif



	void SpinachCore::SaveScreenShot(const char* fileName)
	{
		image.SetCanvas(canvas);
		image.SaveAsPng(fileName);
		image.SetCanvas(nullptr);
	}

	void SpinachCore::WaitForEvents()
	{
		SDL_Event event;
		int loop = 1;
		while (loop) {
			//SDL_zero(event);
			//SDL_PollEvent(&event);
			if (SDL_WaitEvent(&event) == 0) {
				continue;
			}
			if (event.type == SDL_EVENT_KEY_UP ||
				event.type == SDL_EVENT_QUIT ||
				event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				loop = 0;
			}

		}
	}

	void SpinachCore::RenderCanvas()
	{
		int height = canvas->GetHeight();
		int pitch = canvas->GetPitch();
		int bufferBytesLength = height * pitch;
		unsigned char* pixels = canvas->GetPixelBuffer();
		unsigned char* destPixels;
		int destPitch;
		if (SDL_LockTexture(texture, NULL, (void**)&destPixels, &destPitch))
		{
#ifdef SINGLEMEMCOPY
			memcpy(destPixels, pixels, bufferBytesLength);
#else
			for (int y = 0; y < height; ++y) {
				memcpy(destPixels + y * destPitch, pixels + y * pitch, pitch);
			}
#endif
			SDL_UnlockTexture(texture);
		}
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	void SpinachCore::Destroy()
	{

		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}

		if (canvas != nullptr) {
			delete canvas;
			canvas = nullptr;
		}

		if (renderer != nullptr) {
			SDL_DestroyRenderer(renderer);
			renderer = nullptr;
		}

		if (window != nullptr) {
			SDL_DestroyWindow(window);
			window = nullptr;
		}
	}
}
