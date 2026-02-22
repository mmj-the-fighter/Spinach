#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include <iostream>

#include <spn_canvas.h>
#include "rt_in_one_weekend/rtweekend.h"
#include "rt_in_one_weekend/ray.h"
#include "rt_in_one_weekend/color.h"
#include "rt_in_one_weekend/vec3.h"

bool hit_sphere(const point3& center, double radius, const ray& r);
color ray_color(const ray& r);

class RayTracer
{
private:
	double aspectRatio;
	int imageWidth;
	int imageHeight;
	double focalLength;
	double viewportHeight;
	double viewportWidth;
	vec3 cameraCenter;

	//vectors across the horizontal and down the vertical viewport edges.
	vec3 viewportU;
	vec3 viewportV;

	//horizontal and vertical delta vectors from pixel to pixel.
	vec3 pixelDeltaU;
	vec3 pixelDeltaV;

	//location of the upper left pixel.
	vec3 viewportUpperLeft;
	vec3 pixelTopLeftLoc;

	//
	unsigned char* pixels;
	bool isRenderingCompleted;
	spn::Canvas* rtCanvas;

public:
	static int CalculateImageHeight(double aspectRatio, int imageWidth) {
		int imageHeight;
		imageHeight = int(imageWidth / aspectRatio);
		imageHeight = (imageHeight < 1) ? 1 : imageHeight;
		return imageHeight;
	}

	void Init(
		spn::Canvas* canvas,
		double focalLength,
		double viewPortHeight,
		vec3 cameraCenter) 
	{
		imageWidth = canvas->GetWidth();
		imageHeight = canvas->GetHeight();
		rtCanvas = canvas;

		// Camera
		focalLength = 1.0;
		viewportHeight = 2.0;
		viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
		cameraCenter = cameraCenter;

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		viewportU = vec3(viewportWidth, 0, 0);
		viewportV = vec3(0, -viewportHeight, 0);

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// Calculate the location of the upper left pixel.
		viewportUpperLeft =
			cameraCenter - vec3(0, 0, focalLength)
			- viewportU / 2
			- viewportV / 2;
		pixelTopLeftLoc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
		//extra
		isRenderingCompleted = false;
	}
	inline bool IsRenderingCompleted() { return isRenderingCompleted; }
	void Render();
};
#endif

