#include <iostream>
#include "spn_canvas.h"
#include "spn_image.h"
#include "spn_rfont.h"
#include "../common/spn_utils.h"

namespace spn 
{
	Canvas::Canvas(unsigned int aWidth, unsigned int aHeight)
		:width(aWidth), height(aHeight), lastFrameTime(0.0f), font(nullptr),
		primaryColorR(255), primaryColorG(255), primaryColorB(255)
	{
		channels = 4;
		pitch = width * 4;
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

	void Canvas::DrawImage(Image* image, int x, int y)
	{
		Canvas *imCanvas = image->GetCanvas();
		CopyAllPixels(imCanvas->GetWidth(),
			imCanvas->GetHeight(),
			imCanvas->GetPixelBuffer(),
			x, y);
	}

	void Canvas::DrawText(const std::string& text, int x, int y)
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
		for (int i = 0; i < text.size(); ++i) {
			unsigned char c = text.at(i);
			if (c != ' ') {
				unsigned char charDiff = c - baseChar;
				int row = charDiff / maxCols;
				int col = charDiff - (row*maxCols);
				int atlasX = col * cellWidth;
				int atlasY = row * cellHeight;
				//draw a region of atlas at xpos
				CopyPixels(atlasX, atlasY, cellWidth, cellHeight,
					atlasWidth, atlasHeight, srcPixels, xpos, y,
					0, 0, 0,
					primaryColorR,
					primaryColorG,
					primaryColorB
					);
			}
			xpos += font->GetCharWidth(c);
		}


	}

	void Canvas::CopyAllPixels(
		int srcWidth, int srcHeight,
		unsigned char * srcPixels,
		int dstXStart, int dstYStart)
	{
		Rect r1, r2, ir;
		int sx, sy, tw, th;
		int srcX, srcY;
		int dstX, dstY;
		int numOfChannels = channels;
		unsigned char *dstPixels = pixBuffer;
		unsigned char *dstLoc, *srcLoc;
		int srcPitch = srcWidth * numOfChannels;
		int dstWidth = width;
		int dstHeight = height;
		int dstPitch = pitch;
		int x, y;
		double oneOver255 = 1.0 / 255.0;
		unsigned char srcAlphaUchar;



		/* culling */
		if (
			(dstXStart + srcWidth < 0 || dstXStart >= dstWidth) ||
			(dstYStart + srcHeight < 0 || dstYStart >= dstHeight)
			)
		{
			return;
		}

		/*compute intersection*/
		r1.left = dstXStart;
		r1.top = dstYStart;
		r1.width = srcWidth;
		r1.height = srcHeight;

		r2.left = 0;
		r2.top = 0;
		r2.width = dstWidth;
		r2.height = dstHeight;

		IntersctRectRect(r1, r2, ir);


		/*clip new */
		sx = ir.left - r1.left;
		sy = ir.top - r1.top;

		//correction
		tw = ir.width;
		th = ir.height;


		/*iterate through source rectangle * /
		/* copy to destination */
		for (srcY = sy, y = 0; srcY < (sy + th); ++srcY, ++y)
		{
			for (srcX = sx, x = 0; srcX < (sx + tw); ++srcX, ++x)
			{
				dstX = ir.left + x;
				dstY = ir.top + y;
				srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
				dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
				*dstLoc++ = *srcLoc++;
				*dstLoc++ = *srcLoc++;
				*dstLoc++ = *srcLoc++;
				*dstLoc = *srcLoc;
			}
		}

	}

	void Canvas::CopyPixels(
		int srcXStart,
		int srcYStart,
		int srcWidth,
		int srcHeight,
		int srcTotalWidth,
		int srcTotalHeight,
		unsigned char * srcPixels,
		int dstXStart,
		int dstYStart)
	{
		Rect r1, r2, ir;
		int sx, sy, tw, th;
		int srcX, srcY;
		int dstX, dstY;
		int numOfChannels = channels;
		unsigned char *dstPixels = pixBuffer;
		unsigned char *dstLoc, *srcLoc;
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

		/*compute intersection*/
		r1.left = dstXStart;
		r1.top = dstYStart;
		r1.width = srcWidth;
		r1.height = srcHeight;

		r2.left = 0;
		r2.top = 0;
		r2.width = dstWidth;
		r2.height = dstHeight;

		IntersctRectRect(r1, r2, ir);


		/*clip new */
		sx = ir.left - r1.left;
		sy = ir.top - r1.top;

		//correction
		tw = ir.width;
		th = ir.height;


		/*iterate through source rectangle * /
		/* copy to destination */
		for (srcY = srcYStart + sy, y = 0; srcY < (srcYStart+sy + th); ++srcY, ++y)
		{
			for (srcX = srcXStart + sx, x = 0; srcX < (srcXStart+sx + tw); ++srcX, ++x)
			{
				dstX = ir.left + x;
				dstY = ir.top + y;
				srcLoc = srcPixels + srcPitch * srcY + srcX * numOfChannels;
				dstLoc = dstPixels + dstPitch * dstY + dstX * numOfChannels;
				*dstLoc++ = *srcLoc++;
				*dstLoc++ = *srcLoc++;
				*dstLoc++ = *srcLoc++;
				*dstLoc = *srcLoc;
			}
		}
	}


	void Canvas::CopyPixels(
		int srcXStart,
		int srcYStart,
		int srcWidth,
		int srcHeight,
		int srcTotalWidth,
		int srcTotalHeight,
		unsigned char * srcPixels,
		int dstXStart,
		int dstYStart,
		int chromaR,
		int chromaG,
		int chromaB
		)
	{
		Rect r1, r2, ir;
		int sx, sy, tw, th;
		int srcX, srcY;
		int dstX, dstY;
		int numOfChannels = channels;
		unsigned char *dstPixels = pixBuffer;
		unsigned char *dstLoc, *srcLoc;
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

		/*compute intersection*/
		r1.left = dstXStart;
		r1.top = dstYStart;
		r1.width = srcWidth;
		r1.height = srcHeight;

		r2.left = 0;
		r2.top = 0;
		r2.width = dstWidth;
		r2.height = dstHeight;

		IntersctRectRect(r1, r2, ir);


		/*clip new */
		sx = ir.left - r1.left;
		sy = ir.top - r1.top;

		//correction
		tw = ir.width;
		th = ir.height;


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
				if (chromaB != *srcLoc ||
					chromaG != *(srcLoc + 1) ||
					chromaR != *(srcLoc + 2)
					)
				{
					*dstLoc++ = *srcLoc++;
					*dstLoc++ = *srcLoc++;
					*dstLoc++ = *srcLoc++;
					*dstLoc = *srcLoc;
				}
			}
		}
	}

	void Canvas::CopyPixels(
		int srcXStart,
		int srcYStart,
		int srcWidth,
		int srcHeight,
		int srcTotalWidth,
		int srcTotalHeight,
		unsigned char * srcPixels,
		int dstXStart,
		int dstYStart,
		int chromaR,
		int chromaG,
		int chromaB,
		int textColorR,
		int textColorG,
		int textColorB
		)
	{
		Rect r1, r2, ir;
		int sx, sy, tw, th;
		int srcX, srcY;
		int dstX, dstY;
		int numOfChannels = channels;
		unsigned char *dstPixels = pixBuffer;
		unsigned char *dstLoc, *srcLoc;
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

		/*compute intersection*/
		r1.left = dstXStart;
		r1.top = dstYStart;
		r1.width = srcWidth;
		r1.height = srcHeight;

		r2.left = 0;
		r2.top = 0;
		r2.width = dstWidth;
		r2.height = dstHeight;

		IntersctRectRect(r1, r2, ir);


		/*clip new */
		sx = ir.left - r1.left;
		sy = ir.top - r1.top;

		//correction
		tw = ir.width;
		th = ir.height;


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
				if (chromaB != *srcLoc ||
					chromaG != *(srcLoc + 1) ||
					chromaR != *(srcLoc + 2)
					)
				{
					*dstLoc++ = textColorB;
					*dstLoc++ = textColorG;
					*dstLoc++ = textColorR;
					*dstLoc = 255;
				}
			}
		}
	}
}
