#include <iostream>
#include <thread>
#include <memory>
#include <spn_canvas.h>
#include <spn_core.h>

#include "RayTracer.h"


std::unique_ptr<RayTracer> rt;
spn::SpinachCore* pSc = nullptr;
bool userWantsToQuit = false;


void CopyPixelsToCanvas(spn::Canvas* canvas, int scanline) {
	int height = canvas->GetHeight();
	int width = canvas->GetWidth();
	unsigned char* pixBuffer = canvas->GetPixelBuffer();
	int pitch = canvas->GetPitch();
	int channels = 4;
	for (int y = 0; y < scanline; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			unsigned char* aLoc = pixBuffer + pitch * y + x * channels;
			*aLoc = 128;
			*(aLoc + 1) = 0;
			*(aLoc + 2) = 0;
			*(aLoc + 3) = 255;
		}
	}
}

void RayTracerThreadFn() {
	rt->Render();
	std::cout << "Rendering completed";
}


void InputHandler(const SDL_Event* event) {
	/*if (event->type == SDL_EVENT_KEY_UP ||
		event->type == SDL_EVENT_QUIT ||
		event->type == SDL_EVENT_MOUSE_BUTTON_UP
		) {
		rt->SetUserWantsToQuit(true);
	}*/
}


int main(int argc, char* argv[])
{
	int imageWt = 640;
	int imageHt = RayTracer::CalculateImageHeight(16.0 / 9.0, 640);
	spn::SpinachCore sc(imageWt, imageHt, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetWindowTitle("RT in one weekend: First example");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.GetCanvas()->Clear();
	sc.SetInputHandler(InputHandler);
	pSc = &sc;
	rt = std::make_unique<RayTracer>();
	rt->Init(sc.GetCanvas(), 1.0, 2.0, point3(0,0,0));
	std::thread t1{ RayTracerThreadFn };
	t1.detach();
	sc.MainLoop();
	return 0;
}