#include <iostream>
#include <algorithm>
#include <vector>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <spn_geom.h>


inline float easeInCubic(float t)
{
	return t * t * t;
}

inline float linear(float t)
{
	return t;
}

using EasingFn = float (*)(float);
EasingFn easeFn = easeInCubic;
EasingFn easeFnArray[] = {
	linear,
	easeInCubic,
};


char* easeFnNamesArray[] = {
	"linear",
	"easeInCubic",
};

bool running = false;

using ConstPointsRef = const std::vector<spn::Vec2d>&;
using PointsRef = std::vector<spn::Vec2d>&;
using Points = std::vector<spn::Vec2d>;
using Point = spn::Vec2d;



int numOfEasingFns = sizeof(easeFnArray) / sizeof(easeFn);
int easingFnIndex = 0;

Points a = {
	{ 113.397, 250.000 },
	{ 163.397, 163.397 },
	{ 200.000, 300.000 },
	{ 200.000, 300.000 }
};
//Points b = { {100,100}, {200,100}, {100,200}, {100,200} };
Points b = { {100,100}, {200,100}, {200,200}, {100,200} };
Points c = { {100,100}, {200,100}, {100,200}, {100,200} };

float frames = 320;
float frameCount = 0;
int frameNum = 0;


void SetAnimationDelay(float delay, int fps) {
	frames = (delay * fps);
}

void dbgPrintVec(Point v) {
	std::cout << v.x << " " << v.y << "\n";
}

float SnapToUnity(float v) {
	return ((1 - v) < 0.00001) ? 1 : v;
}

void DrawPoly(spn::Canvas* canvas, ConstPointsRef p) {
	int n = (int)p.size();
	for (int i = 0; i < n; i++) {
		const Point& u = p[i];
		const Point& v = p[(i + 1) % n];
		canvas->DrawStroke(u.x, u.y, v.x, v.y);
	}
}

void MorphPoints(PointsRef result, ConstPointsRef first, ConstPointsRef second, float t) {
	for (int i = 0; i < result.size(); i++) {
		
		result[i] = spn::Lerp(first[i], second[i], easeFn(t));
	}
}

void UpdateAndRender(spn::Canvas* canvas) {
	if (!running) return;
	canvas->Clear();
	MorphPoints(c, a, b, std::min(1.0f, (frameCount /frames)));
	DrawPoly(canvas, c);
	++frameCount;
}

void HandleInput(const SDL_Event* sdlEvent) {
	switch (sdlEvent->type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent->key.key) {
		case SDLK_F1:
			std::cout << easingFnIndex << "  " << easeFnNamesArray[easingFnIndex] << std::endl;
			running = true;
			break;
		case SDLK_SPACE:
			frameCount = 0;
			++easingFnIndex;
			easingFnIndex = easingFnIndex % numOfEasingFns;
			easeFn = easeFnArray[easingFnIndex];
			std::cout << easingFnIndex << "  "<< easeFnNamesArray[easingFnIndex] << std::endl;
			std::copy(a.begin(), a.end(), c.begin());
			break;
		}
		break;
	}
	
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
	SetAnimationDelay(2.5, 30);
	sc.LockFps(true);
	
	sc.MainLoop();
	//spn::Profiler::GetInstance().Print();
	PROFILE_PRINT_PROFILER_OUTPUT
	return 0;
}