#include <iostream>
#include <thread>
#include <memory>
#include <spn_canvas.h>
#include <spn_core.h>

#include "RayTracer.h"


std::unique_ptr<RayTracer> rt;
spn::SpinachCore* pSc = nullptr;
bool userWantsToQuit = false;


void RayTracerThreadFn() {
	rt->Render();
	std::cout << "Rendering completed";
}

int main(int argc, char* argv[])
{
	int imageWt = 640;
	double aspectRatio = 16.0/9.0;
	int imageHt = RayTracer::CalculateImageHeight(aspectRatio, imageWt);
	spn::SpinachCore sc(imageWt, imageHt, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetWindowTitle("RT in one weekend: First example");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.GetCanvas()->Clear();
	pSc = &sc;
	rt = std::make_unique<RayTracer>();
	rt->Init(sc.GetCanvas(), 1.0, 2.0, point3(0,0,0));
	std::thread t1{ RayTracerThreadFn };
	t1.detach();
	sc.MainLoop();
	return 0;
}