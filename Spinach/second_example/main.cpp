//image processing example
#include <iostream>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"
#include "../spinach/core/spn_image.h"
#include "imageproc/imageproc.h"

int main(int argc, char* argv[])
{
	spn::Image img;
	img.CreateFromPng("res/road.png");
	spn::Image* sepiaImage = img.Clone();
	ApplySepiaFilter(sepiaImage);

	spn::SpinachCore sc(640, 480);

	if (sc.IsInitSucceded()){
		spn::Canvas* canvas = sc.GetCanvas();
		canvas->SetPrimaryColor(0, 255, 0);
		canvas->DrawImage(&img,0,0);
		canvas->DrawText("base image", 100, 400);
		sc.RenderCanvas();

		sc.WaitForEvents();

		canvas->DrawImage(sepiaImage,0,0);
		canvas->DrawText("after applying sepia filter", 100, 400);
		sc.RenderCanvas();
		sc.WaitForEvents();

	}
	delete sepiaImage;
	
	return 0;
}