#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <spn_utils.h>
#include <spn_collision_utils.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_textbox.h>
#include <rmgui/spn_rmgui_button.h>
#include <rmgui/spn_rmgui_slider.h>
#include <rmgui/spn_rmgui_dropdown.h>
#include "imgprc.h"

#define MAXRESX 800
#define MAXRESY 600


spn::Image sourceImage;
spn::Image* workingImage;
spn::rmgui::Button* doFilterButton;
spn::rmgui::Button* showOriginalButton;
spn::rmgui::Button* autoThreshButton;
spn::rmgui::Button* saveButton;
spn::rmgui::Textbox* kernelSizeTextBox;
spn::rmgui::Dropdown* algoDropdown;
spn::rmgui::UiManager* uim;
bool inputEnabled = true;

spn::rmgui::Slider* threshSlider;
RoiRect curRoi;
bool buildingRoi = false;

void SaveWorkingImage(char* buffer) {
	workingImage->SaveAsPng(buffer);
}

bool IsRoiDiscardable(const RoiRect r, int x, int y, int w, int h) {
	return
		std::abs((r.x0 - r.x1) * (r.y0 - r.y1)) < 4L
		||
		!spn::CheckCollision(r.x0, r.y0, r.x1, r.y1, x, y, x + w, y + h);
}

void ThresholdSeperation(float threshold) {
	spn::Canvas* srcCanvas = sourceImage.GetCanvas();
	int w = srcCanvas->GetWidth();
	int h = srcCanvas->GetHeight();
	int chn = srcCanvas->GetChannels();
	if (IsRoiDiscardable(curRoi, 0, 0, w, h)) {
		std::cout << "Roi invalid. right click for full window. left click and drag for region\n";
		return;
	}
	Threshold(workingImage->GetCanvas()->GetPixelBuffer(),
		sourceImage.GetCanvas()->GetPixelBuffer(),
		w,h,chn,
		curRoi,
		threshold
	);
}
void AutoThreshold() {
	float t = GetAvgIntensity(sourceImage.GetCanvas()->GetPixelBuffer(),
		sourceImage.GetCanvas()->GetNumOfPixels());
	threshSlider->SetRangeAndValue(0, 255, t);
	threshSlider->CalculateKnobPosition();
}


void OnSliderValueChanged(int id, float value) {
	int i;
	switch (id) {
	case 100:
		ThresholdSeperation(value);
		break;
	}
}


void NoisedBlur(bool noFiltering)
{	
	int i, j;
	int x, y;
	const int channels = 4;
	int kernelSize = 3;
	unsigned char* srcImg = sourceImage.GetCanvas()->GetPixelBuffer();
	unsigned char* dstImg = workingImage->GetCanvas()->GetPixelBuffer();

	int width = sourceImage.GetCanvas()->GetWidth();
	int height = sourceImage.GetCanvas()->GetHeight();
	if (noFiltering) {
		std::cout << "Restoring the original image\n";
		memcpy(dstImg, srcImg, width * height * channels);
		return;
	}

	if (IsRoiDiscardable(curRoi, 0, 0, width, height)) {
		std::cout << "Roi invalid. right click for full window. left click and drag for region\n";
		return;
	}
	

	try {
		kernelSize = std::stoi(kernelSizeTextBox->GetText());
	}
	catch (...) {
		std::cout << "Parsing int failed...\n";
		std::cout << "Blurring with kernel size 3\n";
		kernelSize = 3;
	}
	
	if (kernelSize % 2 == 0) {
		std::cout << "Can't work on even kernel size\n";
		return;
	}
	if (kernelSize < 0 || kernelSize > 100) {
		std::cout << "Invalid kernel size\n";
		return;
	}
	spn::ProfilerScope prof(kernelSize);
	inputEnabled = false;
	
	std::cout << "Kernel size: " << kernelSize << std::endl;
	int halfKernelSize = kernelSize / 2;

	//init horizontal and vertical kernels (for box filtering)
	float filterValue = 1.0f / (float)kernelSize;
	float noiseAmount = 20 + rng.GenerateFloat() * 15;
	std::cout << "noise amount " << noiseAmount << "\n";
	RoiRect& roi = curRoi;
	
	Blur(smoothedImagePixels, srcImg, 
		width, height, channels, 
		curRoi, kernelSize);
	int choice = algoDropdown->GetOption();

	if (choice == 0) {
		std::cout << "Blue Noise\n";
		BlueNoise(fullyFilteredImagePixels, smoothedImagePixels,
			width, height, channels,
			curRoi, noiseAmount);
	}
	else {
		std::cout << "White Noise\n";
		WhiteNoise(fullyFilteredImagePixels, smoothedImagePixels,
			width, height, channels,
			curRoi, noiseAmount);
	}
	
	//change the dest image to be filtered
	memcpy(dstImg, fullyFilteredImagePixels, width * height * channels);
	inputEnabled = true;
}


void InitUi() {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();

	algoDropdown = uim->CreateWidget<Dropdown>();
	algoDropdown->SetPosition(MAXRESX - 250 - 2, 100);
	algoDropdown->AddOption("Blue Noise");
	algoDropdown->AddOption("White Noise");
	algoDropdown->SetSize(128, 32);
	algoDropdown->SetId(100);
	algoDropdown->SetCallback([=](int id, int selected) {
		if (id != 100) return;
		if (inputEnabled) {
			NoisedBlur(false);
		}

		});


	
	doFilterButton = uim->CreateWidget<Button>();
	doFilterButton->SetPosition(80, MAXRESY-50);
	doFilterButton->SetSize(128, 32);
	doFilterButton->SetString("Blur + Noise");
	doFilterButton->SetCallback([=](int id) {
		NoisedBlur(false);
		//EdgeDetectFilter(false);
		});

	showOriginalButton = uim->CreateWidget<Button>();
	showOriginalButton->SetPosition(80+140, MAXRESY - 50);
	showOriginalButton->SetSize(128, 32);
	showOriginalButton->SetString("Show Original");
	showOriginalButton->SetCallback([=](int id) {
		NoisedBlur(true);
		//EdgeDetectFilter(true);
		});

	autoThreshButton = uim->CreateWidget<Button>();
	autoThreshButton->SetPosition(80 + 140+140, MAXRESY - 50);
	autoThreshButton->SetSize(128, 32);
	autoThreshButton->SetString("Auto Thresh");
	autoThreshButton->SetCallback([=](int id) {
		AutoThreshold();
		});

	saveButton = uim->CreateWidget<Button>();
	saveButton->SetPosition(80 + 140 + 140 + 140, MAXRESY - 50);
	saveButton->SetSize(128, 32);
	saveButton->SetString("Save");
	saveButton->SetCallback([=](int id) {
		char buffer[256];
		spn::GetFilenameFromCurrentTime(buffer, "ImgProcExample", ".png");
		SaveWorkingImage(buffer);
		std::cout << "Image saved: " << buffer << '\n';
		});

	kernelSizeTextBox = uim->CreateWidget<Textbox>();
	kernelSizeTextBox->SetPosition(10, MAXRESY-50);
	kernelSizeTextBox->SetSize(64, 30);

	threshSlider = uim->CreateWidget<Slider>();
	threshSlider->SetId(100);
	threshSlider->SetPosition(MAXRESX-250-2, MAXRESY/2);
	threshSlider->SetSize(120, 32);
	threshSlider->SetDecimalPlaces(2);
	threshSlider->SetRangeAndValue(0, 255, 0);
	threshSlider->CalculateKnobPosition();
	threshSlider->SetCallback(OnSliderValueChanged);
	threshSlider->SetSensitivity(0.1);
	threshSlider->SetCStringLabel("Threshold");
	
}



void InitApp() {
	spn::Profiler::GetInstance().Begin(954);
	spn::Profiler::GetInstance().End();

	sourceImage.CreateFromPng("../examples/res_for_examples/road.png");
	workingImage = sourceImage.Clone();
	int channels = 4;
	int width = sourceImage.GetCanvas()->GetWidth();
	int height = sourceImage.GetCanvas()->GetHeight();

	//allocate temporary buffers
	curRoi.x0 = 0;
	curRoi.y0 = 0;
	curRoi.x1 = width;
	curRoi.y1 = height;
	ImgPrcInit(width, height, channels);
}

void DestroyApp() {
	ImgPrcDeInit();
}


void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->DrawImage(workingImage, 0, 0);
	uim->Display(canvas);
		RoiRect& r = curRoi;
		int left = r.x0;
		int right = r.x1;
		int t;
		if (right < left) {
			t = right;
			right = left;
			left = t;
		}
		int top = r.y0;
		int bottom = r.y1;
		if (top > bottom) {
			t = top;
			top = bottom;
			bottom = t;
		}
		canvas->SetPrimaryColorUint(0xc0c0c0);
		canvas->DrawRectangle(left, top, right, bottom);
}

void HandleInput(const SDL_Event* sdlEvent) {

	if (!inputEnabled) {
		return;
	}

	spn::ui::UiEvent uie;
	spn::ui::TranslateSdlEvent(sdlEvent, uie);
	switch (uie.mouseButton) {
	case spn::ui::MouseButton::Right:
			buildingRoi = false;
			curRoi.x0 = 0;
			curRoi.y0 = 0;
			curRoi.x1 = sourceImage.GetCanvas()->GetWidth();
			curRoi.y1 = sourceImage.GetCanvas()->GetHeight();
			break;
	case spn::ui::MouseButton::Left:
		if (uie.eventType == spn::ui::UiEventType::MouseDown) {
			buildingRoi = true;
			if (uie.mouseX > sourceImage.GetCanvas()->GetWidth() || uie.mouseY > sourceImage.GetCanvas()->GetHeight()) {
				break;
			}
			curRoi.x0 = uie.mouseX;
			curRoi.x1 = uie.mouseX;
			curRoi.y0 = uie.mouseY;
			curRoi.y1 = uie.mouseY;
		}
		else if (uie.eventType == spn::ui::UiEventType::MouseDrag) {
			if (buildingRoi) {
				if (uie.mouseX > sourceImage.GetCanvas()->GetWidth() || uie.mouseY > sourceImage.GetCanvas()->GetHeight()) {
					buildingRoi = false;
					break;
				}
				curRoi.x1 = uie.mouseX;
				curRoi.y1 = uie.mouseY;
			}
		}
		else if (uie.eventType == spn::ui::UiEventType::MouseUp) {
			if (buildingRoi) {
				buildingRoi = false;
			}
		}
	}
	uim->HandleUiEvent(uie);
}




int main(int argc, char* argv[])
{
	spn::SpinachCore sc;
	if (!sc.Init(MAXRESX, MAXRESY, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	InitApp();
	InitUi();
	
	
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Image Processing Example");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	sc.MainLoop();
	spn::Profiler::GetInstance().Print();
	DestroyApp();
	return 0;
}