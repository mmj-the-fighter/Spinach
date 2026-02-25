#include <iostream>
#include <thread>
#include <memory>
#include <spn_canvas.h>
#include <spn_core.h>
#include "RayTracer.h"

int main(int argc, char* argv[])
{
	constexpr int imageWt = 640;
	constexpr double aspectRatio = 16.0/9.0;
	constexpr int imageHt = RayTracer::CalculateImageHeight(aspectRatio, imageWt);
	spn::SpinachCore sc(imageWt, imageHt, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetWindowTitle("RT in one weekend: First example");
	sc.GetCanvas()->SetClearColorUint(0x0);
	sc.GetCanvas()->Clear();
	std::unique_ptr<RayTracer> rt = std::make_unique<RayTracer>();
	rt->Init(sc.GetCanvas(), 1.0, 2.0, point3(0,0,0));
	std::thread t1 { [&rt]() { rt->Render(); std::cout << "Rendering completed";} };
	t1.detach();
	sc.MainLoop();
	return 0;
}