#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "spn_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"

#define NANOSVG_IMPLEMENTATION
#include "../external/nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "../external/nanosvg/nanosvgrast.h"

#include "spn_canvas.h"

namespace spn
{
	Image::Image(unsigned int aWidth, unsigned int aHeight)
	{
		canvas = new Canvas(aWidth, aHeight);
	}

	Image::Image(Canvas* aCanvas)
	{
		canvas = aCanvas;
	}

	Image::Image()
	{
		canvas = nullptr;
	}

	Image::~Image()
	{
		DestroyCanvas();
	}


	void Image::DestroyCanvas()
	{
		if (nullptr != canvas){
			delete canvas;
			canvas = nullptr;
		}
	}

	Image* Image::Clone()
	{
		Image * newImage = new Image();
		if (nullptr != canvas)
		{
			Canvas * newCanvas = canvas->Clone();
			newImage->SetCanvas(newCanvas);
		}
		return newImage;
	}

	bool Image::CreateSolidColorBlockImage(int width, int height,
		int colorR, int colorG, int colorB, float alpha)
	{
		if (nullptr != canvas) {
			return false;
		}
		canvas = new Canvas(width, height);
		unsigned char* pixels = canvas->GetPixelBuffer();
		int pitch = canvas->GetPitch();
		int colorA = alpha * 255.0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				unsigned char* loc = pixels + pitch * y + x * 4;
				*loc++ = colorB;
				*loc++ = colorG;
				*loc++ = colorR;
				*loc = colorA;

			}
		}
		return true;
	}

	//AI is used for making this function
	bool Image::CreateCheckerImage(int width, int height, 
		int tileSize, 
		int color1R, int color1G, int color1B, 
		int color2R, int color2G, int color2B) 
	{
		if (nullptr != canvas) {
			return false;
		}
		canvas = new Canvas(width, height);
		unsigned char* pixels = canvas->GetPixelBuffer();
		int pitch = canvas->GetPitch();

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				unsigned char* loc = pixels + pitch * y + x * 4;
				bool isColor1 = (((x / tileSize) & 1) == ((y / tileSize) & 1));
				if (isColor1) {
					*loc++ = color1B;
					*loc++ = color1G;
					*loc++ = color1R;
					*loc = 255;
				}
				else {
					*loc++ = color2B;
					*loc++ = color2G;
					*loc++ = color2R;
					*loc = 255;
				}
			}
		}
		return true;
	}

	bool Image::CreateFromSvg(const char* fileName, float dpi)
	{
		if (nullptr != canvas) {
			return false;
		}
		unsigned char* srcloc, * dstloc;
		NSVGimage* image = NULL;
		NSVGrasterizer* rast = NULL;
		unsigned char* img = NULL;
		int w, h, bpp;
		int num_of_pixels;
		int i;
		image = nsvgParseFromFile(fileName, "px", dpi);
		if (image == NULL) {
			std::cout<<"nanosvg: Cannot open: "<<fileName<<"\n";
			return false;
		}
		w = (int)image->width;
		h = (int)image->height;

		rast = nsvgCreateRasterizer();
		if (rast == NULL) {
			std::cout<<"nanosvg: Could not init rasterizer\n";
			nsvgDelete(image);
			return false;
		}

		img = (unsigned char*)malloc(w * h * 4);
		if (img == NULL) {
			std::cout<<"nanosvg: Could not alloc image buffer.\n";
			nsvgDeleteRasterizer(rast);
			nsvgDelete(image);
			return false;
		}
		nsvgRasterize(rast, image, 0, 0, 1, img, w, h, w * 4);
		canvas = new Canvas(w, h);
		dstloc = canvas->GetPixelBuffer();
		srcloc = img;
		num_of_pixels = canvas->GetNumOfPixels();
		for (i = 0; i < num_of_pixels; ++i) {
			unsigned char r = *srcloc++;
			unsigned char g = *srcloc++;
			unsigned char b = *srcloc++;
			unsigned char a = *srcloc++;
			*dstloc++ = b;
			*dstloc++ = g;
			*dstloc++ = r;
			*dstloc++ = a;
		}
		free(img);
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);
		return true;
	}

	bool Image::CreateFromPng(const char* fileName)
	{
		if (nullptr != canvas) {
			return false;
		}
		int w, h, bpp;
		int num_of_pixels;
		int i;
		unsigned char *srcloc, *dstloc;
		unsigned char* imgdata_stb = stbi_load(fileName, &w, &h, &bpp, STBI_rgb_alpha);
		if (nullptr == imgdata_stb) {
			return false;
		}
		canvas = new Canvas(w, h);
		dstloc = canvas->GetPixelBuffer();
		srcloc = imgdata_stb;
		num_of_pixels = canvas->GetNumOfPixels();

		unsigned char rgba[4];
		for (i = 0; i < num_of_pixels; ++i) {
			rgba[0] = *srcloc++;
			rgba[1] = *srcloc++;
			rgba[2] = *srcloc++;
			rgba[3] = *srcloc++;
			*dstloc++ = rgba[2];
			*dstloc++ = rgba[1];
			*dstloc++ = rgba[0];
			*dstloc++ = rgba[3];
		}
		stbi_image_free(imgdata_stb);
		return true;
	}

	bool Image::CreateFromPpmRaw(const char* fileName)
	{
		unsigned char buffer[3];
		int width = 0, height = 0, colorDepth = 0;
		
		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}

		char c;

		file.read((char*)buffer, 3);
		if (!file) {
			return false;
		}
		if (buffer[0] != 'P' || buffer[1] != '6') {
			return false;
		}

		c = file.peek();
		if (c == '#') {
			do{
				file.get(c);
			} while (c != '\n');
		}
		file >> width;
		file >> height;
		file >> colorDepth;

		file.get();


		int numPixels = width * height;
		canvas = new Canvas(width, height);
		unsigned char *dstloc = canvas->GetPixelBuffer();
		unsigned char red, green, blue;
		for (int i = 0; i < numPixels; ++i) {
			file.read((char*)buffer, 3);
			*dstloc = buffer[2];
			++dstloc;
			*dstloc = buffer[1];
			++dstloc;
			*dstloc = buffer[0];
			++dstloc;
			*dstloc = 255;
			++dstloc;
		}
		return true;
	}

	bool Image::CreateFromPpmAscii(const char* fileName)
	{
		return false;
	}

	bool Image::SaveAsPng(const char* fileName)
	{
		Canvas* canvas = GetCanvas();
		int pitch = canvas->GetPitch();
		unsigned char* loc;

		unsigned char* png_data = new unsigned char[canvas->GetPixelDataLength()];
		int  height = canvas->GetHeight();
		int  width = canvas->GetWidth();
		unsigned char* pixels = canvas->GetPixelBuffer();

		int i = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				loc = pixels + pitch * y + x * 4;
				int b = *loc;
				int g = *(loc + 1);
				int r = *(loc + 2);
				int a = *(loc + 3);
				png_data[i] = r;
				png_data[i + 1] = g;
				png_data[i + 2] = b;
				png_data[i + 3] = a;
				i += 4;
			}
		}
		stbi_write_png(fileName, width, height, 4, png_data, pitch);
		delete[] png_data;
		return true;
	}

	void Image::Scale(ScaleMethod sm, Image* dstImage) {
		switch (sm) {
		case NEAREST:
			return ScaleNearest(dstImage);
		case BLERP:
			return ScaleBlerp(dstImage);
		}
	}

	// Copyright and License for this function 
	// SPDX-License-Identifier: MIT
	// SPDX-FileCopyrightText: 2026 Meitar Basson
	// Gist:https://gist.github.com/meitarBass/ff224ae087e1ffd3a1b50b74b6721e1a
	void Image::ScaleBlerp(Image* destination) {
		int target_width = destination->GetCanvas()->GetWidth();
		int target_height = destination->GetCanvas()->GetHeight();
		if (target_width <= 0 || target_height <= 0) {
			return;
		}
		unsigned char* srcPix = canvas->GetPixelBuffer();
		unsigned char* dstPix = destination->GetCanvas()->GetPixelBuffer();
		int width = canvas->GetWidth();
		int height = canvas->GetHeight();
		float scale_x = width / static_cast<float>(target_width);
		float scale_y = height / static_cast<float>(target_height);
		for (int y_d = 0; y_d < target_height; y_d++) {
			for (int x_d = 0; x_d < target_width; x_d++) {
				float scaled_x_pos = (x_d + 0.5f) * scale_x - 0.5f;
				float scaled_y_pos = (y_d + 0.5f) * scale_y - 0.5f;
				int x0 = (int)floorf(scaled_x_pos);
				int y0 = (int)floorf(scaled_y_pos);
				int x1 = x0 + 1;
				int y1 = y0 + 1;

				float tx = scaled_x_pos - x0;
				float ty = scaled_y_pos - y0;
				int ix0 = (int)std::clamp(x0, 0, width - 1);
				int iy0 = (int)std::clamp(y0, 0, height - 1);
				int ix1 = (int)std::clamp(x1, 0, width - 1);
				int iy1 = (int)std::clamp(y1, 0, height - 1);
				int p00i = iy0 * width + ix0;
				float p00b = srcPix[p00i*4];
				float p00g = srcPix[p00i*4+1];
				float p00r = srcPix[p00i*4+2];
				
				int p10i = iy0 * width + ix1;
				float p10b = srcPix[p10i*4];
				float p10g = srcPix[p10i*4+1];
				float p10r = srcPix[p10i*4+2];

				int p01i = iy1 * width + ix0;
				float p01b = srcPix[p01i*4];
				float p01g = srcPix[p01i*4+1];
				float p01r = srcPix[p01i*4+2];
				
				int p11i = iy1 * width + ix1;
				float p11b = srcPix[p11i*4];
				float p11g = srcPix[p11i*4+1];
				float p11r = srcPix[p11i*4+2];
				float topr = lerp(p00r, p10r, tx);
				float topg = lerp(p00g, p10g, tx);
				float topb = lerp(p00b, p10b, tx);
				float bottomr = lerp(p01r, p11r, tx);
				float bottomg = lerp(p01g, p11g, tx);
				float bottomb = lerp(p01b, p11b, tx);
				float finalr = lerp(topr, bottomr, ty);
				float finalg = lerp(topg, bottomg, ty);
				float finalb = lerp(topb, bottomb, ty);
				int pixi = y_d * target_width + x_d;
				dstPix[pixi*4] = (unsigned char)(finalb + 0.5f);
				dstPix[pixi*4+1] = (unsigned char)(finalg + 0.5f);
				dstPix[pixi*4+2] = (unsigned char)(finalr + 0.5f);
				dstPix[pixi*4+3] = 255;
			}
		}
	}

	// Copyright and License for this function 
	// SPDX-License-Identifier: MIT
	// SPDX-FileCopyrightText: 2026 Meitar Basson
	// Gist:https://gist.github.com/meitarBass/ff224ae087e1ffd3a1b50b74b6721e1a
	void Image::ScaleNearest(Image* destination) {
		int target_width = destination->GetCanvas()->GetWidth();
		int target_height = destination->GetCanvas()->GetHeight();
		if (target_width <= 0 || target_height <= 0) {
			return;
		}
		unsigned char* srcPix = canvas->GetPixelBuffer();
		unsigned char* dstPix = destination->GetCanvas()->GetPixelBuffer();
		int width = canvas->GetWidth();
		int height = canvas->GetHeight();
		float scale_x = width / static_cast<float>(target_width);
		float scale_y = height / static_cast<float>(target_height);
		for (int y_d = 0; y_d < target_height; y_d++) {
			for (int x_d = 0; x_d < target_width; x_d++) {
				float src_x = (x_d + 0.5f) * scale_x - 0.5f;
				float src_y = (y_d + 0.5f) * scale_y - 0.5f;
				int x_scaled = static_cast<int>(std::round(std::clamp(src_x, 0.0f, width - 1.0f)));
				int y_scaled = static_cast<int>(std::round(std::clamp(src_y, 0.0f, height - 1.0f)));
				int dst = y_d * target_width + x_d;
				int src = y_scaled * width + x_scaled;
				dstPix[dst*4] = srcPix[src*4];
				dstPix[dst*4+1] = srcPix[src*4+1];
				dstPix[dst*4+2] = srcPix[src*4+2];
				dstPix[dst*4+3] = 255;
			}
		}
	}
}

