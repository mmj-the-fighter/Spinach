#include <iostream>
#include <algorithm>
#include <string>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <ui_event.h>
#include <ui_event_translator.h>
#include <ui_manager.h>
#include <textbox.h>
#include <button.h>
#include <slider.h>


#define MAXRESX 800
#define MAXRESY 600

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
			*dstImg++ = 255;
			*dstImg++ = 255;
			*dstImg++ = 255;
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


	//apply filter horizontally
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
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

	//apply filter vertically
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
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
}

void DestroyApp() {
	delete[] fullyFilteredImagePixels;
	delete[] horizontallyFilteredImagePixels;
}


void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->DrawImage(workingImage, 0, 0);
	uim->Display(canvas);
}

void HandleInput(const SDL_Event* sdlEvent) {

	if (!inputEnabled) {
		return;
	}

	spn::rmgui::UiEvent uie;
	spn::rmgui::TranslateSdlEvent(sdlEvent, uie);
	uim->HandleUiEvent(uie);
}



int main(int argc, char* argv[])
{
	spn::SpinachCore sc(MAXRESX, MAXRESY, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
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
	sc.MainLoop();
	spn::Profiler::GetInstance().Print();
	DestroyApp();
	return 0;
}