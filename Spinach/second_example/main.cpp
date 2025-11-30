//image processing example
#include <iostream>
#include <cstdlib>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"
#include "../spinach/core/spn_image.h"
#include "imageproc/imageproc.h"


// note
// res folder and build_root folder are inside the project folder
// build_root is where solution is built using cmake
// there for the correct resources folder for me is "../res/"
// note that i have included build_root in .gitignore

struct vec2
{
	float x;
	float y;
};

void Sierpinski(spn::Canvas* canvas, vec2 points[3], int iterations) {
	vec2 p;
	vec2 q;
	vec2 u, v;
	u.x = points[1].x - points[0].x;
	u.y = points[1].y - points[0].y;
	v.x = points[2].x - points[0].x;
	v.y = points[2].y - points[0].y;
	p.x = points[0].x + 0.5 * (u.x + v.x);
	p.y = points[0].y + 0.5 * (u.y + v.y);

	int count = iterations;
	while (--count > 0) {
		int r = rand() % 3;
		q.x = p.x + (points[r].x - p.x) * 0.5;
		q.y = p.y + (points[r].y - p.y) * 0.5;
		canvas->SetPixel(q.x, q.y, 255, 0 , 0);
		p = q;
	}
}

int main(int argc, char* argv[])
{
	spn::Image img;


	img.CreateFromPng("../res/road.png");
	spn::Image* sepiaImage = img.Clone();
	ApplySepiaFilter(sepiaImage);

	spn::Image svgImg;
	svgImg.CreateFromSvg("../res/NAND_ANSI.svg",100);

	spn::SpinachCore sc(640, 480,"../res/");

	if (sc.IsInitFailed()) {
		return 1;
	}
	
	spn::Canvas* canvas = sc.GetCanvas();

	float minx = 30;
	float miny = 30;
	float maxx = canvas->GetWidth() / 2 - 30;
	float maxy = maxx;
	vec2 points[3];
	points[0].x = minx;
	points[0].y = maxy;
	points[1].x = maxx;
	points[1].y = maxy;
	points[2].x = (minx + maxy) / 2;
	points[2].y = miny;
	canvas->Clear();
	canvas->EnableAlphaBlending(false);
	canvas->SetClearColor(0, 0, 0);
	Sierpinski(canvas, points, 4000);
	sc.RenderCanvas();
	sc.WaitForEvents();

	canvas->SetClearColor(255, 255, 255);
	canvas->Clear();
	canvas->SetPrimaryColor(0, 0, 128);
	canvas->EnableAlphaBlending(true);//needed for rendering images with transparency whether loaded from png or svg.
	canvas->DrawImage(&svgImg, 10, 10);
	canvas->DrawText("NAN gate rendered from svg", 100, 150);
	sc.RenderCanvas();
	sc.WaitForEvents();
	
	canvas->EnableAlphaBlending(false);
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

	delete sepiaImage;
	
	return 0;
}