#include <iostream>
#include <deque>
#include <vector>
//#define _USE_MATH_DEFINES
#include <cmath>
#include <ui_event.h>
#include <ui_event_translator.h>
#include <ui_manager.h>
#include <slider.h>
#include <dropdown.h>
#include <label.h>
#include <button.h>
#include <checkbox.h>
#include <textbox.h>


#include <spn_canvas.h>
#include <spn_core.h>

constexpr double M_PI_VALUE = 3.14159265358979323846;

struct Knot {
	float x;
	float y;
	float radius;
	float theta;
	float frequency;
};

struct Vector2 {
	float x;
	float y;
};


std::vector<Knot> knots;
std::deque<Vector2> genPoints;
int maxPoints = 1000;
float speedMultiplier = 1;
bool showSegments = true;
bool spiroRunning = true;
float samplingInterval = 0;
float sampleTime = 0;
bool canSample = true;
bool clearPoints = false;
bool hideUi = false;
Slider* slider;
Slider* slider2;
Slider* slider3;
Slider* slider4;
Slider* slider5;
Dropdown* dropdown;
Label* label;
Button* button;
Checkbox* checkbox;
Textbox* textbox;
Textbox* textbox2;

UiManager* uim;

void OnSliderValueChanged(int id, float value) {
	//std::cout << "id " << id << "\n";
	int i;
	switch (id) {
	case 100:speedMultiplier = value; break;
	case 200:
		knots[1].radius = value;
		for (i = 1; i < knots.size(); i++) {
			knots[i].y = knots[i - 1].y + knots[i].radius * sin(knots[i].theta);
			knots[i].x = knots[i - 1].x + knots[i].radius * cos(knots[i].theta);
		}
		break;
	case 300:
		knots[2].radius = value;
		for (i = 1; i < knots.size(); i++) {
			knots[i].y = knots[i - 1].y + knots[i].radius * sin(knots[i].theta);
			knots[i].x = knots[i - 1].x + knots[i].radius * cos(knots[i].theta);
		}
		break;
	case 400:
		knots[1].frequency = value;
		break;
	case 500:
		knots[2].frequency = value;
		break;
	}
}


void Init(float maxwidth, float maxheight)
{
	sampleTime = samplingInterval;
	maxPoints = 5000;
	speedMultiplier = 20;

	float midx = maxwidth / 2;
	float midy = maxheight / 2;
	knots.push_back(Knot{ midx, midy, 0,  0,  0 });
	knots.push_back(Knot{ midx, midy, 45, 0, 1 });
	knots.push_back(Knot{ midx, midy, 45,  0, 2 });
	//knots.push_back(Knot{ midx, midy, 20,  0, M_PI });

	for (int i = 1; i < knots.size(); i++) {
		knots[i].y = knots[i-1].y + knots[i].radius * sin(knots[i].theta);
		knots[i].x = knots[i-1].x + knots[i].radius * cos(knots[i].theta);
	}

	uim = &UiManager::GetInstance();
	slider = uim->CreateWidget<Slider>();
	slider->SetId(100);
	slider->SetPosition(16, 300);
	slider->SetSize(128, 32);
	slider->SetDecimalPlaces(0);
	slider->SetRangeAndValue(0, 80, speedMultiplier);
	slider->SetCallback(OnSliderValueChanged);
	slider->CalculateKnobPosition();
	slider->SetSensitivity(0.5);
	slider->SetCStringLabel("AnimSpeed");

	slider2 = uim->CreateWidget<Slider>();
	slider2->SetId(200);
	slider2->SetPosition(16, 336);
	slider2->SetSize(128, 32);
	slider2->SetDecimalPlaces(0);
	slider2->SetRangeAndValue(30, 140, 90);
	slider2->SetCallback(OnSliderValueChanged);
	slider2->CalculateKnobPosition();
	slider2->SetSensitivity(0.5);
	slider2->SetCStringLabel("Radius 1");

	slider3 = uim->CreateWidget<Slider>();
	slider3->SetId(300);
	slider3->SetPosition(16, 372);
	slider3->SetSize(128, 32);
	slider3->SetDecimalPlaces(0);
	slider3->SetRangeAndValue(30, 140, 45);
	slider3->SetCallback(OnSliderValueChanged);
	slider3->CalculateKnobPosition();
	slider3->SetSensitivity(0.5);
	slider3->SetCStringLabel("Radius 2");

	slider4 = uim->CreateWidget<Slider>();
	slider4->SetId(400);
	slider4->SetPosition(16, 408);
	slider4->SetSize(128, 32);
	slider4->SetDecimalPlaces(0);
	slider4->SetRangeAndValue(1, 25, 1);
	slider4->SetCallback(OnSliderValueChanged);
	slider4->CalculateKnobPosition();
	slider4->SetSensitivity(0.5);
	slider4->SetCStringLabel("Freq 1");

	slider5 = uim->CreateWidget<Slider>();
	slider5->SetId(500);
	slider5->SetPosition(16, 444);
	slider5->SetSize(128, 32);
	slider5->SetDecimalPlaces(0);
	slider5->SetRangeAndValue(1, 25, 2);
	slider5->SetCallback(OnSliderValueChanged);
	slider5->CalculateKnobPosition();
	slider5->SetSensitivity(0.5);
	slider5->SetCStringLabel("Freq 2");

	dropdown = uim->CreateWidget<Dropdown>();
	dropdown->SetPosition(16, 260);
	dropdown->AddOption("abc");
	dropdown->AddOption("dcd");
	dropdown->AddOption("eds");
	dropdown->SetSize(128, 32);
	dropdown->SetId(100);

	label = uim->CreateWidget<Label>();
	label->SetString("Hello");
	label->SetPosition(16, 200);
	label->SetId(2000);

	button = uim->CreateWidget<Button>();
	button->SetPosition(16, 170);
	button->SetSize(128, 32);
	button->SetString("Clear");
	button->SetCallback([=](int id) {
		clearPoints = true;
		});
	

	checkbox = uim->CreateWidget<Checkbox>();
	checkbox->SetCStringLabel("Show Segments");
	checkbox->SetPosition(50, 100);
	checkbox->SetChecked(true);
	checkbox->SetCallback([=](int id, bool isChecked) {
		showSegments = isChecked;
		});

	textbox = uim->CreateWidget<Textbox>();
	textbox->SetPosition(50, 40);
	textbox->SetSize(128, 50);

	textbox2 = uim->CreateWidget<Textbox>();
	textbox2->SetPosition(250, 40);
	textbox2->SetSize(128, 50);
}

void UpdateAndRendertest(spn::Canvas* canvas) {
	static int i = 0;
	static int j = 400;
	canvas->Clear();
	canvas->SetClippingRectangle(i, 0, 200, 30);
	canvas->DrawCString("My name is Antony Gonzalez",i,0);
	canvas->DrawRectangle(i, 0, i + 200, 0 + 30);
	i++;
	if (i > 200)
		i = 0;

	canvas->SetClippingRectangle(j, 300, 200, 30);
	canvas->DrawRectangle(j, 300, j+200, 300 + 30);
	canvas->DrawCString("My name is Antony Gonzalez", j, 300);
	j++;
	if (j > 550)
		j = 400;
}

void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	
	//Rotate the spirograph one time, for each segment according to its frequency
	int n = knots.size();
	if (spiroRunning) {
		for (int i = 1; i < n; i++) {
			float omega = 2 * M_PI_VALUE * knots[i].frequency * speedMultiplier / 180;
			float theta = knots[i].theta + omega * canvas->GetLastFrameTime();
			float radius = knots[i].radius;
			knots[i].x = knots[i - 1].x + radius * cos(theta);
			knots[i].y = knots[i - 1].y + radius * sin(theta);
			knots[i].theta = theta;
		}

		if (canSample)
		{
			//if maxpoints is reached deallocate the oldest point
			if (genPoints.size() > maxPoints) {
				genPoints.pop_front();
			}

			//store the point generated by the rotation of the outermost segment.
			if (canvas->IsInsideBounds(knots[n - 1].x, knots[n - 1].y)) {
				genPoints.push_back(Vector2{ knots[n - 1].x, knots[n - 1].y });
			}
			
		}
	}

	canvas->SetPrimaryColor(128, 255, 4);

	 for (int i = 1; i < genPoints.size(); i++) {
		canvas->DrawLine(genPoints[i - 1].x, genPoints[i - 1].y, genPoints[i].x, genPoints[i].y);
	}

	//draw line segments only if its enabled
	if (showSegments) {
		canvas->SetPrimaryColor(255, 255, 255);
		for (int i = 1; i < n; i++) {
			int x0 = knots[i - 1].x;
			int y0 = knots[i - 1].y;
			int x1 = knots[i].x;
			int y1 = knots[i].y;
			canvas->ClipLineNaive(x0, y0, x1, y1);
			canvas->DrawLine(x0, y0, x1, y1);
		}
		canvas->DrawFilledRectangle(knots[n - 1].x-6, knots[n - 1].y-6, 
			knots[n - 1].x, knots[n - 1].y);
	}


	//wait untill user presses space key
	if (!spiroRunning) {
		canvas->SetPrimaryColor(255, 255, 255);
		canvas->DrawCString("Press SPACE to start", 100, 100);
	}

	if (canSample) {
		sampleTime -= canvas->GetLastFrameTime();
		if (sampleTime <= 0) {
			sampleTime = 0;
			canSample = false;
		}
	}
	if(!canSample) {
		sampleTime += canvas->GetLastFrameTime();
		if (sampleTime > samplingInterval) {
			sampleTime = samplingInterval;
			canSample = true;
		}
	}
	if (!hideUi) {
		uim->Display(canvas);
		//slider.Display(canvas);
		//slider2.Display(canvas);
		//slider3.Display(canvas);
		//slider4.Display(canvas);
		//slider5.Display(canvas);
	}
	
	if (clearPoints) {
		genPoints.clear();
		clearPoints = false;
	}
}

void HandleInput(const SDL_Event* e)
{
	float x, y, dx, dy;
	static float prvX = 0, prvY = 0;
	int c1 = -1, c2 = -1, n1 = -1, n2 = -1;
	UiEvent uie;
	TranslateSdlEvent(e, uie);
	bool hasUiConsumedEvent = uim->HandleUiEvent(uie);

	if (!hasUiConsumedEvent && uie.eventType == UiEventType::CharInput) {
		switch (uie.character) {
		case 'c':
		case 'C':
			clearPoints = true;
			break;
		case 'r':
		case 'R':
			showSegments = !showSegments;
			break;
		case ' ':
			spiroRunning = !spiroRunning;
			break;
		case 'h':
		case 'H':
			hideUi = !hideUi;
			break;
		}
		
	}
}

// note
// res folder and build_root folder are inside the project folder
// build_root is where solution is built using cmake
// there for the correct resources folder for me is "../res/"
// note that i have included build_root in .gitignore


int main(int argc, char* argv[])
{
	Init(800, 600);
	spn::SpinachCore sc(800, 600, "../res/", UpdateAndRender, HandleInput);
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