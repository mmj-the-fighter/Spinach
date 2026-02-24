#include <iostream>
#include <functional>
#include <spn_canvas.h>
#include <spn_core.h>
#include "n3d/rasterizer.h"
#include "wireframe_renderer.h"

int main(int argc, char* argv[])
{
	WireframeRenderer wr;
	spn::SpinachCore sc(640, 480, "../res/");
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	wr.Init(&sc);
	sc.SetUpdateAndRenderHandler(
		std::bind(&WireframeRenderer::UpdateAndRender,
		&wr, std::placeholders::_1));
	sc.SetInputHandler(
		std::bind(&WireframeRenderer::HandleInput,
		&wr, std::placeholders::_1));
	sc.SetTargetFramesPerSecond(60);
	sc.MainLoop();
	return 0;
}
