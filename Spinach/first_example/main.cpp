#include <iostream>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"

void CubicBezierTests(spn::Canvas* canvas) 
{
	//Cubic Bezier tests
	canvas->DrawCubicBezierUniform(100, 100, 200, 100, 300, 100, 400, 100);
	canvas->DrawCubicBezierUniform(100, 100, 150, 300, 250, 300, 300, 500);
	canvas->DrawCubicBezierUniform(100, 500, 250, 100, 550, 700, 700, 200);
	canvas->DrawCubicBezierUniform(100, 300, 200, 100, 600, 100, 700, 300);
	canvas->DrawCubicBezierUniform(400, 100, 600, 200, 600, 400, 400, 500);
	canvas->DrawCubicBezierUniform(300, 300, 320, 280, 340, 320, 360, 300);
	canvas->DrawCubicBezierUniform(700, 500, 600, 200, 500, 200, 400, 500);
	canvas->DrawCubicBezierUniform(50, 550, 200, 50, 600, 550, 750, 100);
	canvas->DrawCubicBezierUniform(100, 100, 250, 105, 550, 95, 700, 100);
	canvas->DrawCubicBezierUniform(150, 300, 350, 100, 450, 500, 650, 300);
}

void ArcTests(spn::Canvas* canvas)
{
	canvas->DrawArc(100, 300, 200, 200, 0, false, true, 300, 100);
	canvas->DrawArc(100, 300, 200, 200, 0, false, false, 300, 100);
	canvas->DrawArc(100, 300, 200, 200, 0, true, true, 300, 100);
	canvas->DrawArc(200, 300, 100, 50, 45.0f, false, true, 400, 300);
	canvas->DrawArc(100, 300, 150, 50, 0, false, true, 400, 300);
}

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
	//CubicBezierTests(canvas);
	//ArcTests(canvas);
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
	return 0;
}