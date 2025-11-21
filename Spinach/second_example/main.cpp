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

	spn::Image svgImg;
	svgImg.CreateFromSvg("res/NAND_ANSI.svg",100);

	spn::SpinachCore sc(640, 480);

	if (sc.IsInitSucceded()){
		spn::Canvas* canvas = sc.GetCanvas();

		canvas->SetClearColor(255, 255, 255);
		canvas->Clear();
		canvas->SetPrimaryColor(0, 0, 128);
		canvas->EnableAlphaBlending(true);//needed for rendering images with transparency whether loaded from png or svg.
		canvas->DrawImage(&svgImg, 10, 10);
		canvas->DrawText("NAN gate rendered from svg", 100, 150);
		sc.RenderCanvas();

		sc.WaitForEvents();
		canvas->EnableAlphaBlending(false);//we are not dealing with images with transparency anymore, this is important because bitblt calls will be fast.
		canvas->SetClearColor(0, 0, 0);

		canvas->Clear();
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