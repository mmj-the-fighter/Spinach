#include <iostream>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"
#include "../spinach/common/spn_profiler.h"

void UpdateAndRender(spn::Canvas* canvas) {
	static int frameNum = 0;
	++frameNum;
	spn::ProfilerLimitedScope scope(frameNum, 10, 40);
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
	canvas->DrawCString("the quick brown fox 1234567890", 245, 40);
}

void HandleInput(const SDL_Event* sdlEvent) {
	//std::cout << "*";
}

// note
// res folder and build_root folder are inside the project folder
// build_root is where solution is built using cmake
// there for the correct resources folder for me is "../res/"
// note that i have included build_root in .gitignore


int main(int argc, char* argv[])
{
	spn::SpinachCore sc(640, 480, "../res/", UpdateAndRender, HandleInput);
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	//sc.SetUpdateAndRenderHandler(UpdateAndRender);
	//sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Spinach Demo");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.MainLoop();
	spn::Profiler::GetInstance().Print();
	return 0;
}