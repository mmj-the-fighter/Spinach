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
int buttonState = spn::imgui::BTN_RELEASE;
bool running = false;
bool canrun = true;
char* buttonText = nullptr;
char* runText = "Run";
char* pauseText = "Pause";
spn::rmgui::UiEvent uie;

void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->SetPrimaryColor(128, 255, 4);
	float w = canvas->GetWidth();
	float h = canvas->GetHeight();
	float hw = w * 0.5;
	float hh = h * 0.5;
	int graphInterval = 35;
	canvas->DrawLine(hw, 0, hw, h - 1);
	canvas->DrawLine(0, hh, w - 1, hh);
	//+X
	for (int i = hw; i < w; i+= graphInterval) {
		char num[256];
		int x = i - hw;
		sprintf(num, "%d", x);
		canvas->DrawCString(num, i, hh);
	}
	//-X
	for (int i = hw- graphInterval; i >= 0; i -= graphInterval) {
		char num[256];
		int x = i - hw;
		sprintf(num, "%d", x);
		canvas->DrawCString(num, i, hh);
	}

	//-Y
	for (int i = hh; i < h; i += graphInterval) {
		char num[256];
		int y = hh - i;
		sprintf(num, "%d", y);
		canvas->DrawCString(num, hw, i);
	}

	//+Y
	for (int i = hh - graphInterval; i >= 0; i -= graphInterval) {
		char num[256];
		int y = hh - i;
		sprintf(num, "%d", y);
		canvas->DrawCString(num, hw, i);
	}

	canvas->SaveColors();
	canvas->SetPrimaryColorUint(0xb0b0b0);
	canvas->DrawRectangle(0, 0, w-1, h-1);
	for (int i = 0; i < w; i += graphInterval) {
		canvas->DrawLine(i, 0, i, h-1);
	}

	for (int i = 0; i < h; i += graphInterval) {
		canvas->DrawLine(0, i, w - 1, i);
	}
	canvas->RestoreColors();
	if (running) {
		float radius = a * exp(b * theta);
		float angularSpeed = 4.0f;
		theta += angularSpeed * canvas->GetLastFrameTime();
		Vector2 v;
		v.x = hw + radius * cos(theta);
		v.y = hh + radius * sin(theta);
		if (canvas->IsOutsideBounds(v.x, v.y)) {
			running = false;
			canrun = false;
		}
		else if (spiralPoints.size() < maxPoints) {
			spiralPoints.push_back(v);
		}
	}
	
	for (int i = 1; i < spiralPoints.size(); i++) {
		canvas->DrawLine(spiralPoints[i - 1].x, spiralPoints[i - 1].y, 
			spiralPoints[i].x, spiralPoints[i].y);
	}
	//spn::imgui::Checkbox(canvas, uie, "running",90,100, running);
	if (spn::imgui::Button(canvas, uie, buttonText, 90, 100, 90, 30, buttonState)) {
		if (canrun) {
			running = !running;
			if (running) {
				buttonText = pauseText;
			}
			else {
				buttonText = runText;
			}

		}
	}
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
	sc.LockFps(true);
	spiralPoints.reserve(maxPoints+2);
	if (running) {
		buttonText = pauseText;
	}
	else {
		buttonText = runText;
	}
	sc.MainLoop();
	return 0;
}