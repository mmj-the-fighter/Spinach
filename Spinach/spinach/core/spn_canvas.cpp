#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "spn_canvas.h"
#include "spn_image.h"
#include "spn_rfont.h"
#include "../common/spn_utils.h"

namespace spn 
{
	Canvas::Canvas(unsigned int aWidth, unsigned int aHeight)
		:width(aWidth), height(aHeight), lastFrameTime(0.0f), font(nullptr),
		primaryColorR(255), primaryColorG(255), primaryColorB(255),
		clearColorR(0), clearColorG(0), clearColorB(0), isAlphaBlendingEnabled(false)
	{
		channels = 4;
		pitch = width * 4;
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		numOfPixels = aWidth * aHeight;
		pixelDataLength = numOfPixels * 4;
		pixBuffer = new unsigned char[pixelDataLength];
	}

	Canvas::~Canvas(){
		if (pixBuffer != nullptr) {
			delete[] pixBuffer;
			pixBuffer = nullptr;
		}
		if (font != nullptr){
			delete font;
			font = nullptr;
		}
	}

	Canvas* Canvas::Clone(){
		Canvas* newCanvas = new Canvas(width, height);
		if (pixBuffer != nullptr) {
			memcpy(newCanvas->GetPixelBuffer(), GetPixelBuffer(), pixelDataLength);
		}
		if (font != nullptr){
			RFont* newFont = font->Clone();
			newCanvas->SetFont(newFont);
		}
		return newCanvas;
	}

	//For debugging 
	void Canvas::FillAnything()
	{
		unsigned int half = numOfPixels * 4 / 2;
		for (unsigned int i = 0; i < numOfPixels * 4; i += 4) {
			if (i < half){
				pixBuffer[i] = 128;
				pixBuffer[i + 1] = 0;
				pixBuffer[i + 2] = 0;
				pixBuffer[i + 3] = 255;
			}
			else{
				pixBuffer[i] = 0;
				pixBuffer[i + 1] = 255;
				pixBuffer[i + 2] = 0;
				pixBuffer[i + 3] = 255;
			}
		}
	}

	void Canvas::FlipHorizontally() 
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width/2; ++x)
			{
				unsigned char* aLoc = pixBuffer + pitch * y + x * channels;
				unsigned char* bLoc = pixBuffer + pitch * y + (width-1-x) * channels;

				unsigned char tBlue = *aLoc;
				unsigned char tGreen = *(aLoc+1);
				unsigned char tRed = *(aLoc+2);
				unsigned char tAlpha = *(aLoc+3);

				*aLoc = *bLoc;
				*(aLoc+1) = *(bLoc+1);
				*(aLoc+2) = *(bLoc+2);
				*(aLoc+3) = *(bLoc+3);

				*bLoc = tBlue;
				*(bLoc + 1) = tGreen;
				*(bLoc + 2) = tRed;
				*(bLoc + 3) = tAlpha;
			}
		}
	}

	void Canvas::FlipVertically() 
	{
		
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height/2; ++y)
			{
				unsigned char* aLoc = pixBuffer + pitch * y + x * channels;
				unsigned char* bLoc = pixBuffer + pitch * (height-1-y) + x * channels;

				unsigned char tBlue = *aLoc;
				unsigned char tGreen = *(aLoc + 1);
				unsigned char tRed = *(aLoc + 2);
				unsigned char tAlpha = *(aLoc + 3);

				*aLoc = *bLoc;
				*(aLoc + 1) = *(bLoc + 1);
				*(aLoc + 2) = *(bLoc + 2);
				*(aLoc + 3) = *(bLoc + 3);

				*bLoc = tBlue;
				*(bLoc + 1) = tGreen;
				*(bLoc + 2) = tRed;
				*(bLoc + 3) = tAlpha;
			}
		}
	}

	void Canvas::Clear()
	{
		for (unsigned int i = 0; i < numOfPixels*4; i += 4) {
			pixBuffer[i] = clearColorB;
			pixBuffer[i + 1] = clearColorG;
			pixBuffer[i + 2] = clearColorR;
			pixBuffer[i + 3] = 255;
		}
	}

	void Canvas::DrawLine(int x0, int y0, int x1, int y1)
	{
		if (x0 < 0 || x0 > width - 1 || y0 < 0 || y0 > height - 1) {
			return;
		}
		if (x1 < 0 || x1 > width - 1 || y1 < 0 || y1 > height - 1) {
			return;
		}

		float x, y, xIncr, yIncr;
		int steps;
		int dx = x1 - x0;
		int dy = y1 - y0;

		if (abs(dx) > abs(dy)) {
			steps = abs(dx);
		}
		else {
			steps = abs(dy);
		}
		xIncr = dx / static_cast<float>(steps);
		yIncr = dy / static_cast<float>(steps);
		x = x0;
		y = y0;
		for (int i = 0; i < steps; i++) {
			int ix = static_cast<int>(x + 0.5f);
			int iy = static_cast<int>(y + 0.5f);
			unsigned char* dstLoc = pixBuffer + pitch * iy + ix * channels;
			*dstLoc++ = primaryColorB;
			*dstLoc++ = primaryColorG;
			*dstLoc++ = primaryColorR;
			*dstLoc = 255;
			x += xIncr;
			y += yIncr;
		}
	}

	//This function was developed with the help of ChatGPT AI agent
	void Canvas::DrawCircle(int xPos, int yPos, int radius) {
		int x = 0;
		int y = radius;
		int p = 1 - radius;

		SetPixelWithPrimaryColor(xPos + x, yPos + y);
		SetPixelWithPrimaryColor(xPos - x, yPos + y);
		SetPixelWithPrimaryColor(xPos + x, yPos - y);
		SetPixelWithPrimaryColor(xPos - x, yPos - y);
		SetPixelWithPrimaryColor(xPos + y, yPos + x);
		SetPixelWithPrimaryColor(xPos - y, yPos + x);
		SetPixelWithPrimaryColor(xPos + y, yPos - x);
		SetPixelWithPrimaryColor(xPos - y, yPos - x);
		
		while (x < y) {
			if (p < 0) {
				p += 2 * x + 3;
				++x;
			}
			else {
				p += 2 * (x - y) + 5;
				++x;
				y--;
			}
			SetPixelWithPrimaryColor(xPos + x, yPos + y);
			SetPixelWithPrimaryColor(xPos - x, yPos + y);
			SetPixelWithPrimaryColor(xPos + x, yPos - y);
			SetPixelWithPrimaryColor(xPos - x, yPos - y);
			SetPixelWithPrimaryColor(xPos + y, yPos + x);
			SetPixelWithPrimaryColor(xPos - y, yPos + x);
			SetPixelWithPrimaryColor(xPos + y, yPos - x);
			SetPixelWithPrimaryColor(xPos - y, yPos - x);
		}
	}

	//This function was developed with the help of ChatGPT AI agent
	void Canvas::DrawCubicBezierUniform(
		float x0, float y0,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		int segments)
	{
		if (segments < 1) segments = 1;
		Vec2 prev{ x0, y0 };
		for (int i = 1; i <= segments; ++i) {
			float t = (float)i / (float)segments;
			float mt = 1.0f - t;

			// cubic bezier parametric
			float x = mt * mt * mt * x0
				+ 3 * mt * mt * t * x1
				+ 3 * mt * t * t * x2
				+ t * t * t * x3;
			float y = mt * mt * mt * y0
				+ 3 * mt * mt * t * y1
				+ 3 * mt * t * t * y2
				+ t * t * t * y3;

			DrawLine((int)round(prev.x), (int)round(prev.y),
				(int)round(x), (int)round(y));
			prev.x = x; prev.y = y;
		}
	}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
	//This function was developed with the help of ChatGPT AI agent
	void Canvas::DrawArc(float x0, float y0,
		float rx, float ry,
		float xAxisRotationDeg,
		bool largeArcFlag, bool sweepFlag,
		float x1, float y1)
	{
		// 1. Convert degrees to radians
		float phi = xAxisRotationDeg * (M_PI / 180.0f);
		float cosPhi = cosf(phi);
		float sinPhi = sinf(phi);

		// 2. Step 1: Compute midpoint between start and end
		float dx2 = (x0 - x1) / 2.0f;
		float dy2 = (y0 - y1) / 2.0f;

		// 3. Transform to the rotated coordinate system
		float x1p = cosPhi * dx2 + sinPhi * dy2;
		float y1p = -sinPhi * dx2 + cosPhi * dy2;

		// 4. Correct radii if too small
		float rx_sq = rx * rx;
		float ry_sq = ry * ry;
		float x1p_sq = x1p * x1p;
		float y1p_sq = y1p * y1p;

		float radicant = (rx_sq * ry_sq - rx_sq * y1p_sq - ry_sq * x1p_sq)
			/ (rx_sq * y1p_sq + ry_sq * x1p_sq);
		if (radicant < 0) radicant = 0;
		float coef = (largeArcFlag != sweepFlag ? 1 : -1) * sqrtf(radicant);

		float cxp = coef * ((rx * y1p) / ry);
		float cyp = coef * (-(ry * x1p) / rx);

		// 5. Transform back to original coordinate system
		float cx = cosPhi * cxp - sinPhi * cyp + (x0 + x1) / 2.0f;
		float cy = sinPhi * cxp + cosPhi * cyp + (y0 + y1) / 2.0f;

		// 6. Compute start and end angles
		auto angleBetween = [](float ux, float uy, float vx, float vy) {
			float dot = ux * vx + uy * vy;
			float len = sqrtf((ux * ux + uy * uy) * (vx * vx + vy * vy));
			float ang = acosf(std::clamp(dot / len, -1.0f, 1.0f));
			if (ux * vy - uy * vx < 0) ang = -ang;
			return ang;
			};

		float ux = (x1p - cxp) / rx;
		float uy = (y1p - cyp) / ry;
		float vx = (-x1p - cxp) / rx;
		float vy = (-y1p - cyp) / ry;

		float startAngle = angleBetween(1, 0, ux, uy);
		float deltaAngle = angleBetween(ux, uy, vx, vy);
		if (!sweepFlag && deltaAngle > 0)
			deltaAngle -= 2 * M_PI;
		else if (sweepFlag && deltaAngle < 0)
			deltaAngle += 2 * M_PI;

		// 7. Sample points along arc
		int segments = static_cast<int>(fabs(deltaAngle) * 60); // approx 1° per segment
		for (int i = 0; i <= segments; ++i)
		{
			float theta = startAngle + deltaAngle * (i / (float)segments);
			float x = cosPhi * (rx * cosf(theta)) - sinPhi * (ry * sinf(theta)) + cx;
			float y = sinPhi * (rx * cosf(theta)) + cosPhi * (ry * sinf(theta)) + cy;

			if (x >= 0 && y >= 0 && x < GetWidth() && y < GetHeight())
				SetPixelWithPrimaryColor((int)x, (int)y);
		}
	}


	void Canvas::DrawFilledRectangularRegion(int left, int top, int right, int bottom) {

		unsigned char* loc;
		int x, y;

		//clip points
		//left
		if (left >= width) {
			left = width - 1;
		}
		else if (left < 0) {
			left = 0;
		}
		//right
		if (right >= width) {
			right = width - 1;
		}
		else if (right < 0) {
			right = 0;
		}
		//top
		if (top >= height) {
			top = height - 1;
		}
		else if (top < 0) {
			top = 0;
		}
		//bottom
		if (bottom >= height) {
			bottom = height - 1;
		}
		else if (bottom < 0) {
			bottom = 0;
		}

		for (y = top; y <= bottom; ++y)
		{
			for (x = left; x <= right; ++x)
			{
				loc = pixBuffer + pitch * y + x * channels;
				*loc++ = primaryColorB;
				*loc++ = primaryColorG;
				*loc++ = primaryColorR;
				*loc = 255;
			}
		}
	}

	void Canvas::DrawImage(Image* image, int x, int y)
	{
		Canvas *imCanvas = image->GetCanvas();
		BitBlockTransfer(imCanvas->GetPixelBuffer(),
			imCanvas->GetWidth(),
			imCanvas->GetHeight(),
			x, y);
		//CopyAllPixels(imCanvas->GetWidth(),
		//	imCanvas->GetHeight(),
		//	imCanvas->GetPixelBuffer(),
		//	x, y);
	}

	void Canvas::DrawSubImage(Image* image, int x, int y,
		int tileStartX, int tileStartY,
		int tileWidth, int tileHeight
	) {
		Canvas* imCanvas = image->GetCanvas();

		BitBlockTransfer(imCanvas->GetPixelBuffer(),
			imCanvas->GetWidth(),
			imCanvas->GetHeight(),
			x, y, tileStartX, tileStartY, tileWidth, tileHeight);


	}

	void Canvas::DrawImageChromaKeyed(Image* image,
		int x, int y,
		int chromaR, int chromaG, int chromaB)
	{
		Canvas *imCanvas = image->GetCanvas();
		BitBlockTransfer(imCanvas->GetPixelBuffer(),
			imCanvas->GetWidth(),
			imCanvas->GetHeight(), x, y, 0, 0, -1, -1, 
			chromaR, chromaG, chromaB);
		//CopyPixels(
		//	0,
		//	0,
		//	imCanvas->GetWidth(),
		//	imCanvas->GetHeight(),
		//	imCanvas->GetWidth(),
		//	imCanvas->GetHeight(),
		//	imCanvas->GetPixelBuffer(),
		//	x,
		//	y,
		//	chromaR,
		//	chromaG,
		//	chromaB
		//	);

	}

	void Canvas::DrawSubImageChromaKeyed(Image* image,
		int x, int y,
		int tileStartX, int tileStartY,
		int tileWidth, int tileHeight,
		int chromaR, int chromaG, int chromaB
	) {
		Canvas* imCanvas = image->GetCanvas();

		BitBlockTransfer(imCanvas->GetPixelBuffer(),
			imCanvas->GetWidth(),
			imCanvas->GetHeight(),
			x, y, tileStartX, tileStartY, tileWidth, tileHeight,
			chromaR, chromaG, chromaB);

		//CopyPixels(
		//	tileStartX,
		//	tileStartY,
		//	tileWidth,
		//	tileHeight,
		//	imCanvas->GetWidth(),
		//	imCanvas->GetHeight(),
		//	imCanvas->GetPixelBuffer(),
		//	x,
		//	y,
		//	chromaR,
		//	chromaG,
		//	chromaB
		//);

	}

	void Canvas::DrawText(const char* text, int x, int y)
	{
		if (font == nullptr) {
			return;
		}
		int atlasWidth = font->GetCanvas()->GetWidth();
		int atlasHeight = font->GetCanvas()->GetHeight();
		int cellWidth = font->GetCellWidth();
		int cellHeight = font->GetCellHeight();
		int maxCols = atlasWidth / cellWidth;
		unsigned char baseChar = font->GetBaseChar();
		unsigned char* srcPixels = font->GetCanvas()->GetPixelBuffer();
		if (srcPixels == nullptr){
			return;
		}
		
		
		int xpos = x;
		int i = 0;
		unsigned char c;
		while ((c = text[i]) != '\0') {
			if (c != ' ') {
				unsigned char charDiff = c - baseChar;
				int row = charDiff / maxCols;
				int col = charDiff - (row*maxCols);
				int atlasX = col * cellWidth;
				int atlasY = row * cellHeight;
				//draw a region of atlas at xpos
				BitBlockTransfer(srcPixels, atlasWidth, atlasHeight,
					xpos, y, atlasX, atlasY, cellWidth,
					cellHeight, 0, 0, 0, primaryColorR,
					primaryColorG, primaryColorB);

				//CopyPixels(atlasX, atlasY, cellWidth, cellHeight,
				//	atlasWidth, atlasHeight, srcPixels, xpos, y,
				//	0, 0, 0,
				//	primaryColorR,
				//	primaryColorG,
				//	primaryColorB
				//	);

			}
			xpos += font->GetCharWidth(c);
			++i;
		}


	}

	void Canvas::DrawString(const std::string& str, int x, int y)
	{
		if (font == nullptr) {
			return;
		}
		int atlasWidth = font->GetCanvas()->GetWidth();
		int atlasHeight = font->GetCanvas()->GetHeight();
		int cellWidth = font->GetCellWidth();
		int cellHeight = font->GetCellHeight();
		int maxCols = atlasWidth / cellWidth;
		unsigned char baseChar = font->GetBaseChar();
		unsigned char* srcPixels = font->GetCanvas()->GetPixelBuffer();
		if (srcPixels == nullptr){
			return;
		}


		int xpos = x;
		for (int i = 0; i < str.size(); ++i) {
			unsigned char c = str.at(i);
			if (c != ' ') {
				unsigned char charDiff = c - baseChar;
				int row = charDiff / maxCols;
				int col = charDiff - (row*maxCols);
				int atlasX = col * cellWidth;
				int atlasY = row * cellHeight;
				//draw a region of atlas at xpos

				BitBlockTransfer(srcPixels, atlasWidth, atlasHeight,
					xpos, y, atlasX, atlasY, cellWidth,
					cellHeight, 0, 0, 0, primaryColorR,
					primaryColorG, primaryColorB);

				//CopyPixels(atlasX, atlasY, cellWidth, cellHeight,
				//	atlasWidth, atlasHeight, srcPixels, xpos, y,
				//	0, 0, 0,
				//	primaryColorR,
				//	primaryColorG,
				//	primaryColorB
				//	);


			}
			xpos += font->GetCharWidth(c);
		}


	}

	void Canvas::SetAlpha(float alpha) {
		int ialpha = alpha * 255.0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				unsigned char* loc = pixBuffer + pitch * y + x * 4;
				*(loc + 3) = ialpha;
			}
		}
	}

	void Canvas::BitBlockTransfer(
		unsigned char* srcPixels,
		int srcTotalWidth,
		int srcTotalHeight,
		int dstXStart,
		int dstYStart,
		int srcXStart,
		int srcYStart,
		int srcWidth,
		int srcHeight,
		int chromaR,
		int chromaG,
		int chromaB,
		int newColorR,
		int newColorG,
		int newColorB
	) {
#define ONEOVER255 0.00392156862745098f
		bool isSourceCropped = srcWidth > 0 && srcHeight > 0;
		if (!isSourceCropped) {
			srcWidth = srcTotalWidth;
			srcHeight = srcTotalHeight;
		}
		int dstX, dstY;
		int numOfChannels = channels;
		unsigned char* dstPixels = pixBuffer;
		unsigned char* dstLoc, * srcLoc;
		int srcPitch = srcTotalWidth * numOfChannels;
		int dstWidth = width;
		int dstHeight = height;
		int dstPitch = pitch;
		int x, y;
		

		/* culling */
		if (
			(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
			(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
			)
		{
			return;
		}


		/*compute intersection of e.g.: r1(image) with r2(screen) */
		Rect r1;
		r1.left = dstXStart;
		r1.top = dstYStart;
		r1.width = srcWidth;
		r1.height = srcHeight;

		Rect r2;
		r2.left = 0;
		r2.top = 0;
		r2.width = dstWidth;
		r2.height = dstHeight;

		Rect ir;
		FindRectToRectIntersection(r1, r2, ir);


		/*clip new */
		int sx, sy, tw, th;
		int srcX, srcY;
		sx = ir.left - r1.left;
		sy = ir.top - r1.top;

		//correction
		tw = ir.width;
		th = ir.height;

		bool fColorReplace = newColorR >= 0 && newColorG >= 0 && newColorR >= 0;
		bool fChromaKeyed = chromaR >= 0 && chromaG >= 0 && chromaB >= 0;

		/*iterate through source rectangle * /
		/* copy to destination */
		for (srcY = srcYStart + sy, y = 0; srcY < (srcYStart + sy + th); ++srcY, ++y)
		{
			for (srcX = srcXStart + sx, x = 0; srcX < (srcXStart + sx + tw); ++srcX, ++x)
			{
				dstX = ir.left + x;
				dstY = ir.top + y;
				srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
				dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
				unsigned char sB = *srcLoc;
				unsigned char sG = *(srcLoc + 1);
				unsigned char sR = *(srcLoc + 2);
				unsigned char sA = *(srcLoc + 3);
				bool dontDraw = fChromaKeyed && chromaB == sB && chromaG == sG && chromaR == sR;
				if (dontDraw) {
					continue;
				}
				if (fColorReplace) {
					sB = newColorB;
					sG = newColorG;
					sR = newColorR;
				}
				if (!isAlphaBlendingEnabled) {
					*dstLoc++ = sB;
					*dstLoc++ = sG;
					*dstLoc++ = sR;
					*dstLoc = sA;
					srcLoc += 4;
				}
				else {
					float srcAlpha = sA * ONEOVER255;
					float oneMinusSrcAlpha = 1.0f - srcAlpha;
					float b = srcAlpha * sB + oneMinusSrcAlpha * (*dstLoc);
					float g = srcAlpha * sG + oneMinusSrcAlpha * (*(dstLoc + 1));
					float r = srcAlpha * sR + oneMinusSrcAlpha * (*(dstLoc + 2));
					*dstLoc++ = static_cast<unsigned char> (b);
					*dstLoc++ = static_cast<unsigned char> (g);
					*dstLoc++ = static_cast<unsigned char> (r);
					*dstLoc = 255;
				}
			}
		}
#undef ONEOVER255
	}


	//void Canvas::CopyAllPixels(
	//	int srcWidth, int srcHeight,
	//	unsigned char * srcPixels,
	//	int dstXStart, int dstYStart)
	//{
	//	Rect r1, r2, ir;
	//	int sx, sy, tw, th;
	//	int srcX, srcY;
	//	int dstX, dstY;
	//	int numOfChannels = channels;
	//	unsigned char *dstPixels = pixBuffer;
	//	unsigned char *dstLoc, *srcLoc;
	//	int srcPitch = srcWidth * numOfChannels;
	//	int dstWidth = width;
	//	int dstHeight = height;
	//	int dstPitch = pitch;
	//	int x, y;
	//	double oneOver255 = 1.0 / 255.0;
	//	unsigned char srcAlphaUchar;



	//	/* culling */
	//	if (
	//		(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
	//		(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
	//		)
	//	{
	//		return;
	//	}

	//	/*compute intersection*/
	//	r1.left = dstXStart;
	//	r1.top = dstYStart;
	//	r1.width = srcWidth;
	//	r1.height = srcHeight;

	//	r2.left = 0;
	//	r2.top = 0;
	//	r2.width = dstWidth;
	//	r2.height = dstHeight;

	//	IntersctRectRect(r1, r2, ir);


	//	/*clip new */
	//	sx = ir.left - r1.left;
	//	sy = ir.top - r1.top;

	//	//correction
	//	tw = ir.width;
	//	th = ir.height;


	//	/*iterate through source rectangle * /
	//	/* copy to destination */
	//	for (srcY = sy, y = 0; srcY < (sy + th); ++srcY, ++y)
	//	{
	//		for (srcX = sx, x = 0; srcX < (sx + tw); ++srcX, ++x)
	//		{
	//			dstX = ir.left + x;
	//			dstY = ir.top + y;
	//			srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
	//			dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc = *srcLoc;
	//		}
	//	}

	//}

	//void Canvas::CopyPixels(
	//	int srcXStart,
	//	int srcYStart,
	//	int srcWidth,
	//	int srcHeight,
	//	int srcTotalWidth,
	//	int srcTotalHeight,
	//	unsigned char * srcPixels,
	//	int dstXStart,
	//	int dstYStart)
	//{
	//	Rect r1, r2, ir;
	//	int sx, sy, tw, th;
	//	int srcX, srcY;
	//	int dstX, dstY;
	//	int numOfChannels = channels;
	//	unsigned char *dstPixels = pixBuffer;
	//	unsigned char *dstLoc, *srcLoc;
	//	int srcPitch = srcTotalWidth * numOfChannels;
	//	int dstWidth = width;
	//	int dstHeight = height;
	//	int dstPitch = pitch;
	//	int x, y;


	//	/* culling */
	//	if (
	//		(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
	//		(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
	//		)
	//	{
	//		return;
	//	}

	//	/*compute intersection*/
	//	r1.left = dstXStart;
	//	r1.top = dstYStart;
	//	r1.width = srcWidth;
	//	r1.height = srcHeight;

	//	r2.left = 0;
	//	r2.top = 0;
	//	r2.width = dstWidth;
	//	r2.height = dstHeight;

	//	IntersctRectRect(r1, r2, ir);


	//	/*clip new */
	//	sx = ir.left - r1.left;
	//	sy = ir.top - r1.top;

	//	//correction
	//	tw = ir.width;
	//	th = ir.height;


	//	/*iterate through source rectangle * /
	//	/* copy to destination */
	//	for (srcY = srcYStart + sy, y = 0; srcY < (srcYStart+sy + th); ++srcY, ++y)
	//	{
	//		for (srcX = srcXStart + sx, x = 0; srcX < (srcXStart+sx + tw); ++srcX, ++x)
	//		{
	//			dstX = ir.left + x;
	//			dstY = ir.top + y;
	//			srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
	//			dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc++ = *srcLoc++;
	//			*dstLoc = *srcLoc;
	//		}
	//	}
	//}


	//void Canvas::CopyPixels(
	//	int srcXStart,
	//	int srcYStart,
	//	int srcWidth,
	//	int srcHeight,
	//	int srcTotalWidth,
	//	int srcTotalHeight,
	//	unsigned char * srcPixels,
	//	int dstXStart,
	//	int dstYStart,
	//	int chromaR,
	//	int chromaG,
	//	int chromaB
	//	)
	//{
	//	Rect r1, r2, ir;
	//	int sx, sy, tw, th;
	//	int srcX, srcY;
	//	int dstX, dstY;
	//	int numOfChannels = channels;
	//	unsigned char *dstPixels = pixBuffer;
	//	unsigned char *dstLoc, *srcLoc;
	//	int srcPitch = srcTotalWidth * numOfChannels;
	//	int dstWidth = width;
	//	int dstHeight = height;
	//	int dstPitch = pitch;
	//	int x, y;



	//	/* culling */
	//	if (
	//		(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
	//		(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
	//		)
	//	{
	//		return;
	//	}

	//	/*compute intersection*/
	//	r1.left = dstXStart;
	//	r1.top = dstYStart;
	//	r1.width = srcWidth;
	//	r1.height = srcHeight;

	//	r2.left = 0;
	//	r2.top = 0;
	//	r2.width = dstWidth;
	//	r2.height = dstHeight;

	//	IntersctRectRect(r1, r2, ir);


	//	/*clip new */
	//	sx = ir.left - r1.left;
	//	sy = ir.top - r1.top;

	//	//correction
	//	tw = ir.width;
	//	th = ir.height;


	//	/*iterate through source rectangle * /
	//	/* copy to destination */
	//	for (srcY = srcYStart + sy, y = 0; srcY < (srcYStart + sy + th); ++srcY, ++y)
	//	{
	//		for (srcX = srcXStart + sx, x = 0; srcX < (srcXStart + sx + tw); ++srcX, ++x)
	//		{
	//			dstX = ir.left + x;
	//			dstY = ir.top + y;
	//			srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
	//			dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
	//			if (chromaB != *srcLoc ||
	//				chromaG != *(srcLoc + 1) ||
	//				chromaR != *(srcLoc + 2)
	//				)
	//			{
	//				*dstLoc++ = *srcLoc++;
	//				*dstLoc++ = *srcLoc++;
	//				*dstLoc++ = *srcLoc++;
	//				*dstLoc = *srcLoc;
	//			}
	//		}
	//	}
	//}

	//void Canvas::CopyPixels(
	//	int srcXStart,
	//	int srcYStart,
	//	int srcWidth,
	//	int srcHeight,
	//	int srcTotalWidth,
	//	int srcTotalHeight,
	//	unsigned char * srcPixels,
	//	int dstXStart,
	//	int dstYStart,
	//	int chromaR,
	//	int chromaG,
	//	int chromaB,
	//	int textColorR,
	//	int textColorG,
	//	int textColorB
	//	)
	//{
	//	Rect r1, r2, ir;
	//	int sx, sy, tw, th;
	//	int srcX, srcY;
	//	int dstX, dstY;
	//	int numOfChannels = channels;
	//	unsigned char *dstPixels = pixBuffer;
	//	unsigned char *dstLoc, *srcLoc;
	//	int srcPitch = srcTotalWidth * numOfChannels;
	//	int dstWidth = width;
	//	int dstHeight = height;
	//	int dstPitch = pitch;
	//	int x, y;



	//	/* culling */
	//	if (
	//		(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
	//		(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
	//		)
	//	{
	//		return;
	//	}

	//	/*compute intersection*/
	//	r1.left = dstXStart;
	//	r1.top = dstYStart;
	//	r1.width = srcWidth;
	//	r1.height = srcHeight;

	//	r2.left = 0;
	//	r2.top = 0;
	//	r2.width = dstWidth;
	//	r2.height = dstHeight;

	//	IntersctRectRect(r1, r2, ir);


	//	/*clip new */
	//	sx = ir.left - r1.left;
	//	sy = ir.top - r1.top;

	//	//correction
	//	tw = ir.width;
	//	th = ir.height;


	//	/*iterate through source rectangle * /
	//	/* copy to destination */
	//	for (srcY = srcYStart + sy, y = 0; srcY < (srcYStart + sy + th); ++srcY, ++y)
	//	{
	//		for (srcX = srcXStart + sx, x = 0; srcX < (srcXStart + sx + tw); ++srcX, ++x)
	//		{
	//			dstX = ir.left + x;
	//			dstY = ir.top + y;
	//			srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
	//			dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
	//			if (chromaB != *srcLoc ||
	//				chromaG != *(srcLoc + 1) ||
	//				chromaR != *(srcLoc + 2)
	//				)
	//			{
	//				*dstLoc++ = textColorB;
	//				*dstLoc++ = textColorG;
	//				*dstLoc++ = textColorR;
	//				*dstLoc = 255;
	//			}
	//		}
	//	}
	//}
}
