#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <spn_rng.h>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_textbox.h>
#include <rmgui/spn_rmgui_button.h>
#include <rmgui/spn_rmgui_slider.h>

#define MAXRESX 800
#define MAXRESY 600
#define NOISEBUFSIZE 256

spn::Image sourceImage;
spn::Image* workingImage;
spn::rmgui::Button* doFilterButton;
spn::rmgui::Button* showOriginalButton;
spn::rmgui::Button* autoThreshButton;
spn::rmgui::Textbox* kernelSizeTextBox;
spn::rmgui::UiManager* uim;
bool inputEnabled = true;
//allocate temporary buffers
unsigned char* fullyFilteredImagePixels;
unsigned char* horizontallyFilteredImagePixels;


spn::rmgui::Slider* threshSlider;
struct RoiRect{
	int x0;
	int y0;
	int x1;
	int y1;
};

RoiRect curRoi;
bool buildingRoi = false;
spn::RandomGen& rng = spn::RandomGen::GetInstance();

/*

Thresholding partitions the the image into black and white

Pixels having intensity less than the threshold intensity
will become black

And pixels having intensity greater than or equal to threshold
will become white

The threshold intensity has a minimum value of 0
and a maximum value of 255

Automatic threshold demonstrated here is the
average intensity of the image

Note that the intensity calculation used
here is an approximation

*/

void Threshold(float thresh) {
	unsigned char* srcImg = sourceImage.GetCanvas()->GetPixelBuffer();
	unsigned char* dstImg = workingImage->GetCanvas()->GetPixelBuffer();
	
	int n = sourceImage.GetCanvas()->GetNumOfPixels();
	for (int i = 0; i < n; ++i)
	{
		int b = *srcImg;
		int g = *(srcImg + 1);
		int r = *(srcImg + 2);
		float intensity = (float)((b + g + r)) / 3.0f;

		if (intensity < thresh)
		{
			*dstImg++ = 0;
			*dstImg++ = 0;
			*dstImg++ = 0;
			*dstImg++ = 255;
		}
		else
		{
			*dstImg++ = b;
			*dstImg++ = g;
			*dstImg++ = r;
			*dstImg++ = 255;
		}
		srcImg += 4;
	}
}

void AutoThreshold() {
	unsigned char* srcImg = sourceImage.GetCanvas()->GetPixelBuffer();
	float sum = 0;
	int count = 0;
	int n = sourceImage.GetCanvas()->GetNumOfPixels();
	for (int i = 0; i < n; ++i)
	{
		int b = *srcImg;
		int g = *(srcImg + 1);
		int r = *(srcImg + 2);
		float intensity = (float)((b + g + r)) / 3.0f;
		sum += intensity;
		++count;
		srcImg += 4;
	}
	float averageIntensity = sum / (float)count;
	//Threshold(averageIntensity);
	threshSlider->SetRangeAndValue(0, 255, averageIntensity);
	threshSlider->CalculateKnobPosition();
}

void OnSliderValueChanged(int id, float value) {
	int i;
	switch (id) {
	case 100:
		Threshold(value);
		break;
	}
}

void Filter(bool noFiltering)
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
	float noiseAmount = 20 + rng.GenerateFloat()*40;
	std::cout << "noise amount " << noiseAmount << "\n";
	RoiRect& roi = curRoi;



	//apply filter horizontally
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) continue;
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			
			for (i = -halfKernelSize; i <= halfKernelSize; ++i) {
				int sampleLocationX = std::clamp(x + i, 0, width - 1);
				unsigned char* buffer = srcImg +
					(width * channels * y) +
					(sampleLocationX * channels);
				bs += filterValue * *buffer;
				gs += filterValue * *(buffer + 1);
				rs += filterValue * *(buffer + 2);
			}
			unsigned char* outbuffer =
				horizontallyFilteredImagePixels +
				(width * channels * y) +
				(x * channels);
			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	bool nop = false;

	//apply filter vertically
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) nop=true;
			if (nop) {
				unsigned char* buffer = srcImg +
					(width * channels * y) +
					(x * channels);
				unsigned char* outbuffer =
					fullyFilteredImagePixels +
					(width * channels * y) +
					(x * channels);
				*outbuffer = *buffer;
				*(outbuffer + 1) = *(buffer + 1);
				*(outbuffer + 2) = *(buffer + 2);
				*(outbuffer + 3) = 255; //alpha
				nop = false;
				continue;
			}
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (i = -halfKernelSize; i <= halfKernelSize; ++i) {
				int sampleLocationY = std::clamp(y + i, 0, height - 1);
				unsigned char* buffer =
					horizontallyFilteredImagePixels +
					(width * channels * sampleLocationY) +
					(x * channels);
				bs += filterValue * *buffer;
				gs += filterValue * *(buffer + 1);
				rs += filterValue * *(buffer + 2);
			}

			unsigned char* outbuffer =
				fullyFilteredImagePixels +
				(width * channels * y) +
				(x * channels);

			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}


	//add noise
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) continue;
			unsigned char* buffer = fullyFilteredImagePixels +
				(width * channels * y) +
				(x * channels);
			float displ = (2.0 * rng.GenerateFloat() - 1.0) * noiseAmount;
			float brs = *buffer + displ;
			float grs = *(buffer + 1) + displ;
			float rrs = *(buffer + 2) + displ;
			brs = std::clamp(brs, 0.0f, 255.0f);
			grs = std::clamp(grs, 0.0f, 255.0f);
			rrs = std::clamp(rrs, 0.0f, 255.0f);

			unsigned char* outbuffer =
				fullyFilteredImagePixels +
				(width * channels * y) +
				(x * channels);
			*outbuffer = brs;
			*(outbuffer + 1) = grs;
			*(outbuffer + 2) = rrs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	

	//change the dest image to be filtered
	memcpy(dstImg, fullyFilteredImagePixels, width * height * channels);
	inputEnabled = true;
}


void InitUi() {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();
	doFilterButton = uim->CreateWidget<Button>();
	doFilterButton->SetPosition(80, MAXRESY-50);
	doFilterButton->SetSize(128, 32);
	doFilterButton->SetString("Blur");
	doFilterButton->SetCallback([=](int id) {
		Filter(false);
		});

	showOriginalButton = uim->CreateWidget<Button>();
	showOriginalButton->SetPosition(80+140, MAXRESY - 50);
	showOriginalButton->SetSize(128, 32);
	showOriginalButton->SetString("Show Original");
	showOriginalButton->SetCallback([=](int id) {
		Filter(true);
		});

	autoThreshButton = uim->CreateWidget<Button>();
	autoThreshButton->SetPosition(80 + 140+140, MAXRESY - 50);
	autoThreshButton->SetSize(128, 32);
	autoThreshButton->SetString("Auto Thresh");
	autoThreshButton->SetCallback([=](int id) {
		AutoThreshold();
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
	sourceImage.CreateFromPng("../examples/res_for_examples/road.png");
	workingImage = sourceImage.Clone();
	int channels = 4;
	int width = sourceImage.GetCanvas()->GetWidth();
	int height = sourceImage.GetCanvas()->GetHeight();

	//allocate temporary buffers
	fullyFilteredImagePixels = new unsigned char[width * height * channels];
	horizontallyFilteredImagePixels = new unsigned char[width * height * channels];
	curRoi.x0 = 0;
	curRoi.y0 = 0;
	curRoi.x1 = width;
	curRoi.y1 = height;
	
}

void DestroyApp() {
	delete[] fullyFilteredImagePixels;
	delete[] horizontallyFilteredImagePixels;
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