#define FIRSTEXAMPLE

#ifdef FIRSTEXAMPLE
#include <iostream>
#include "spinach/core/spn_canvas.h"
#include "spinach/core/spn_core.h"

void UpdateAndRender(spn::Canvas* canvas) {
	static int k = 1;
	std::string str = std::to_string(canvas->GetLastFrameTime() * 1000);
	unsigned char *pixBuffer = canvas->GetPixelBuffer();
	int bytes = canvas->GetPixelDataLength();
	int half = bytes / 2;
	for (int i = 0; i < bytes; i += 4) {
		if (i < half){
			pixBuffer[i] = 128 + k;
			pixBuffer[i + 1] = 0;
			pixBuffer[i + 2] = 0;
			pixBuffer[i + 3] = 255;
		}
		else{
			pixBuffer[i] = 0;
			pixBuffer[i + 1] = 128 + k;
			pixBuffer[i + 2] = 0;
			pixBuffer[i + 3] = 255;
		}
	}
	++k;
	k = k % 254;
	canvas->DrawText("the quick brown fox 1234567890", 245, 40);
}

void HandleInput(const SDL_Event* sdlEvent) {
	//std::cout << "*";
}

int main(int argc, char* argv[])
{
	spn::SpinachCore sc(640, 480, UpdateAndRender, HandleInput);
	if (sc.IsInitSucceded()){
		//sc.SetUpdateAndRenderHandler(UpdateAndRender);
		//sc.SetInputHandler(HandleInput);
		sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
		sc.MainLoop();
	}
	else{
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
	}
	return 0;
}

#else 
//image processing example
#include <iostream>
#include "spinach/core/spn_canvas.h"
#include "spinach/core/spn_core.h"
#include "spinach/core/spn_image.h"
#include "imageproc/imageproc.h"

int main(int argc, char* argv[])
{
	spn::Image img;
	img.CreateFromPng("res/road.png");
	spn::Image* sepiaImage = img.Clone();
	ApplySepiaFilter(sepiaImage);

	spn::SpinachCore sc(640, 480);

	if (sc.IsInitSucceded()){
		spn::Canvas* canvas = sc.GetCanvas();
		canvas->SetPrimaryColor(0, 255, 0);
		canvas->DrawImage(&img,0,0);
		canvas->DrawText("base image", 100, 400);
		sc.RenderCanvas();

		sc.WaitForEvents();

		canvas->DrawImage(sepiaImage,0,0);
		canvas->DrawText("after applying sepia filter", 100, 400);
		sc.RenderCanvas();
		sc.WaitForEvents();

	}
	delete sepiaImage;
	
	return 0;
}
#endif