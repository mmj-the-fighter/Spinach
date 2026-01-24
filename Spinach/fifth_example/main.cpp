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


#define MAXRESX 800
#define MAXRESY 600

spn::Image sourceImage;
spn::Image* workingImage;
spn::rmgui::Button* doFilterButton;
spn::rmgui::Button* showOriginalButton;
spn::rmgui::Textbox* kernelSizeTextBox;
spn::rmgui::UiManager* uim;
bool inputEnabled = true;
//allocate temporary buffers
unsigned char* fullyFilteredImagePixels;
unsigned char* horizontallyFilteredImagePixels;

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

	kernelSizeTextBox = uim->CreateWidget<Textbox>();
	kernelSizeTextBox->SetPosition(10, MAXRESY-50);
	kernelSizeTextBox->SetSize(64, 30);
}


void InitApp() {
	sourceImage.CreateFromPng("../res/road.png");
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

// note
// res folder and build_root folder are inside the project folder
// build_root is where solution is built using cmake
// there for the correct resources folder for me is "../res/"
// note that i have included build_root in .gitignore


int main(int argc, char* argv[])
{
	spn::SpinachCore sc(MAXRESX, MAXRESY, "../res/");
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	InitUi();
	InitApp();
	
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Seperable Kernel Box Blur Example");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.MainLoop();
	spn::Profiler::GetInstance().Print();
	DestroyApp();
	return 0;
}