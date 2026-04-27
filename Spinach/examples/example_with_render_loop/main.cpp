#include <iostream>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>

void UpdateAndRender(spn::Canvas* canvas) {
	static int frameNum = 0;
	++frameNum;
	PROFILE_REST_OF_THE_CURRENT_BLOCK_IF_TAG_WITHIN_BOUNDS(frameNum,30,40)
	static unsigned char k = 0;
	unsigned char *pixBuffer = canvas->GetPixelBuffer();
	int bytes = canvas->GetPixelDataLength();
	int half = bytes / 2;
	for (int i = 0; i < half; i += 4) {
		pixBuffer[i] = 32 + k;
		pixBuffer[i + 1] = 0;
		pixBuffer[i + 2] = 0;
		pixBuffer[i + 3] = 255;
	}

	for (int i = half; i < bytes; i += 4) {
		pixBuffer[i] = 0;
		pixBuffer[i + 1] = 32 + k;
		pixBuffer[i + 2] = 0;
		pixBuffer[i + 3] = 255;
	}
	++k;
	k %= 220;
	
	canvas->DisplayFps(100, 100);
}

void HandleInput(const SDL_Event* sdlEvent) {
	//std::cout << "*";
}


int main(int argc, char* argv[])
{
	spn::SpinachCore sc;
	if (!sc.Init(640, 480, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Spinach Demo");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	sc.MainLoop();
	//spn::Profiler::GetInstance().Print();
	PROFILE_PRINT_PROFILER_OUTPUT
	return 0;
}