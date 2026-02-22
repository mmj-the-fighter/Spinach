#include <chrono>
#include <thread>
#include "RayTracer.h"
#include <spn_profiler.h>

bool hit_sphere(const point3& center, double radius, const ray& r) {
	vec3 oc = center - r.origin();
	auto a = dot(r.direction(), r.direction());
	auto b = -2.0 * dot(r.direction(), oc);
	auto c = dot(oc, oc) - radius*radius;
	auto discriminant = b*b - 4 * a*c;
	return (discriminant >= 0);
}

color ray_color(const ray& r) {
	if (hit_sphere(point3(0, 0, -1), 0.5, r))
		return color(1, 0, 0);

	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5*(unit_direction.y() + 1.0);
	return (1.0 - a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

void RayTracer::Render()
{
	isRenderingCompleted = false;
	double percentRemaining = 100.0;
	spn::Profiler::GetInstance().Begin(123);
	int imageHeight = rtCanvas->GetHeight();
	int imageWidth = rtCanvas->GetWidth();
	for (int j = 0; j < imageHeight; ++j) {
		percentRemaining = static_cast<double>((imageHeight - j) * 100) / static_cast<double>(imageHeight);
		std::cout << percentRemaining << "% remaining \n";
		std::this_thread::sleep_for(std::chrono::milliseconds(50));// <-- REMOVE THIS IN PRODUCTION
		for (int i = 0; i < imageWidth; ++i) {
			vec3 pixel_center = pixelTopLeftLoc + (i * pixelDeltaU) + (j * pixelDeltaV);
			vec3 ray_direction = pixel_center - cameraCenter;
			ray r(cameraCenter, ray_direction);
			color pixel_color = ray_color(r);
			rtCanvas->SetPixel(i, j, 255.0 * pixel_color.e[0], 255.0 * pixel_color.e[1], 255.0 * pixel_color.e[2]);
		}
	}
	std::cout <<"0% remaining \n";
	spn::Profiler::GetInstance().End();
	spn::Profiler::GetInstance().Print();
	isRenderingCompleted = true;
}
