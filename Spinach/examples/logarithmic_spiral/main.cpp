#include <iostream>
#include <vector>
#include <cmath>
#include <spn_canvas.h>
#include <spn_core.h>
#include <ui_event.h>
#include <ui_event_translator.h>
#include <imgui/imgui.h>

struct Vector2 {
	float x;
	float y;
};
std::vector<Vector2> spiralPoints;

int maxPoints = 5000;
float a = 1.0;
float b = 0.16;
float theta=0.0;
bool running = false;
spn::rmgui::UiEvent uie;

void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->SetPrimaryColor(128, 255, 4);
	if (running) {
		float radius = a * exp(b * theta);
		float angularSpeed = 4.0f;
		theta += angularSpeed * canvas->GetLastFrameTime();
		Vector2 v;
		v.x = canvas->GetWidth() / 2 + radius * cos(theta);
		v.y = canvas->GetHeight() / 2 + radius * sin(theta);
		if (canvas->IsOutsideBounds(v.x, v.y)) {
			running = false;
		}
		else if (spiralPoints.size() < maxPoints) {
			spiralPoints.push_back(v);
		}
	}
	
	for (int i = 1; i < spiralPoints.size(); i++) {
		canvas->DrawLine(spiralPoints[i - 1].x, spiralPoints[i - 1].y, 
			spiralPoints[i].x, spiralPoints[i].y);
	}
	spn::imgui::Checkbox(canvas, uie, "running",100,100, running);
}

void HandleInput(const SDL_Event* e) {
	spn::rmgui::TranslateSdlEvent(e, uie);
}

int main(int argc, char* argv[])
{

	spn::SpinachCore sc(800, 600, "../res/", UpdateAndRender, HandleInput); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetWindowTitle("Spinach Demo");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	spiralPoints.reserve(maxPoints+2);
	sc.MainLoop();
	return 0;
}