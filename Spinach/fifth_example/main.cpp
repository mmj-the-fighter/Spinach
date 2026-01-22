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
Button* doFilterButton;
Textbox* kernelSizeTextBox;
UiManager* uim;
bool inputEnabled = true;
//allocate temporary buffers
unsigned char* fully_filtered_img;
unsigned char* horiz_filtered_img;

void Filter()
{
	int i, j;
	int x, y;
	const int channels = 4;
	int kernel_size = 3;
	int width = sourceImage.GetCanvas()->GetWidth();
	int height = sourceImage.GetCanvas()->GetHeight();
	kernel_size = std::stoi(kernelSizeTextBox->GetText());
	if (kernel_size % 2 == 0) {
		std::cout << "Can't work on even kernel size\n";
		return;
	}
	if (kernel_size < 0 || kernel_size > 100) {
		std::cout << "Invalid kernel size\n";
		return;
	}
	spn::ProfilerScope prof(kernel_size);
	inputEnabled = false;
	std::cout << "Kernel size: " << kernel_size << std::endl;
	int half_kernel_size = kernel_size / 2;

	//init horizontal and vertical kernels (for box filtering)
	float filter_value = 1.0f / (float)kernel_size;

	unsigned char* src_img = sourceImage.GetCanvas()->GetPixelBuffer();
	unsigned char* dst_img = workingImage->GetCanvas()->GetPixelBuffer();

	//apply filter horizontally
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (i = -half_kernel_size; i <= half_kernel_size; ++i) {
				int sample_location_x = std::clamp(x + i, 0, width - 1);
				unsigned char* buffer = src_img +
					(width * channels * y) +
					(sample_location_x * channels);
				bs += filter_value * *buffer;
				gs += filter_value * *(buffer + 1);
				rs += filter_value * *(buffer + 2);
			}
			unsigned char* outbuffer =
				horiz_filtered_img +
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
			for (i = -half_kernel_size; i <= half_kernel_size; ++i) {
				int sample_location_y = std::clamp(y + i, 0, height - 1);
				unsigned char* buffer =
					horiz_filtered_img +
					(width * channels * sample_location_y) +
					(x * channels);
				bs += filter_value * *buffer;
				gs += filter_value * *(buffer + 1);
				rs += filter_value * *(buffer + 2);
			}

			unsigned char* outbuffer =
				fully_filtered_img +
				(width * channels * y) +
				(x * channels);

			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	//change the source image to filtered
	memcpy(dst_img, fully_filtered_img, width * height * channels);

	inputEnabled = true;
}


void InitUi() {
	uim = &UiManager::GetInstance();
	doFilterButton = uim->CreateWidget<Button>();
	doFilterButton->SetPosition(80, MAXRESY-50);
	doFilterButton->SetSize(128, 32);
	doFilterButton->SetString("Blur");
	doFilterButton->SetCallback([=](int id) {
		Filter();
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
	fully_filtered_img = new unsigned char[width * height * channels];
	horiz_filtered_img = new unsigned char[width * height * channels];
}

void DestroyApp() {
	delete[] fully_filtered_img;
	delete[] horiz_filtered_img;
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

	UiEvent uie;
	TranslateSdlEvent(sdlEvent, uie);
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