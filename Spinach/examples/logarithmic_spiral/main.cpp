#include <iostream>
#include <vector>
#include <cmath>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <imgui/spn_imgui_imgui.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_dropdown.h>

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
int rstbuttonState = spn::imgui::BTN_RELEASE;
int okState = spn::imgui::BTN_RELEASE;
int cancelState = spn::imgui::BTN_RELEASE;
bool alertStatus = false;
bool running = false;
bool canrun = true;
char* buttonText = nullptr;
char* runText = "Run";
char* pauseText = "Pause";
spn::ui::UiEvent uie;
spn::SpinachCore* pCore=nullptr;
spn::rmgui::Dropdown* drawStyleDropdown;
spn::rmgui::UiManager* uim;

void Restart() {
	spiralPoints.clear();
	a = 1.0;
	b = 0.16;
	theta = 0.0;
}

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
			alertStatus = true;
		}
		else if (spiralPoints.size() < maxPoints) {
			spiralPoints.push_back(v);
		}
	}
	canvas->SetPrimaryColorUint(0x0000c0);
	if (drawStyleDropdown->GetOption() == 0) {
		for (int i = 1; i < spiralPoints.size(); i++) {
			canvas->DrawStroke(spiralPoints[i - 1].x, spiralPoints[i - 1].y,
				spiralPoints[i].x, spiralPoints[i].y);
		}
	}
	else {
		for (int i = 0; i < spiralPoints.size(); i++) {
			canvas->DrawDot(spiralPoints[i].x, spiralPoints[i].y);
		}
	}
	
	//spn::imgui::Checkbox(canvas, uie, "running",90,100, running);
	if (spn::imgui::Button(canvas, uie, "Restart", 90, 60, 90, 30, rstbuttonState)) {
		Restart();
		running = true;
		buttonText = pauseText;
	}
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
	if (alertStatus) {
		spn::imgui::AlertResult res = spn::imgui::Alert(
			canvas,
			uie,
			"Want to quit?",
			100, 100,
			250, 200,
			alertStatus,
			okState, cancelState
		);
		if (res == spn::imgui::Ok)
		{
			std::cout << "Ok pressed\n";
			if (pCore != nullptr) {
				pCore->SetUserWantsToQuit(true);
			}
		}
		else if (res == spn::imgui::Cancel) {
			std::cout << "Cancel pressed\n";
		}
		
	}
	spn::imgui::ProgressBar(canvas, 10, 14, 320, 12, 0x0000ff, 0xc0c000, (float)spiralPoints.size()/272.0f);
	uim->Display(canvas);
}

void HandleInput(const SDL_Event* e) {
	
	spn::ui::TranslateSdlEvent(e, uie);
	uim->HandleUiEvent(uie);
}

void InitUI(spn::SpinachCore* pCore) {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();

	drawStyleDropdown = uim->CreateWidget<Dropdown>();
	drawStyleDropdown->SetPosition(pCore->GetCanvas()->GetWidth() - 250 - 2, 100);
	drawStyleDropdown->AddOption("Line");
	drawStyleDropdown->AddOption("Dot");
	drawStyleDropdown->SetSize(128, 32);
	drawStyleDropdown->SetId(100);
	//drawStyleDropdown->SetCallback([&](int id, int selected) {});
}

int main(int argc, char* argv[])
{
	spn::SpinachCore sc;
	if (!sc.Init(800, 600, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);
	pCore = &sc;
	InitUI(pCore);
	sc.GetCanvas()->SetStrokeRadius(3);
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
	
	pCore = nullptr;
	return 0;
}