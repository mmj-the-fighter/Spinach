#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>
#include <iostream>
#include <algorithm>
#include "../common/spn_utils.h"

namespace spn 
{
#define MAXREFRESHRECTS 256
	class Image;
	class RFont;

	class Canvas
	{
	public:
		Canvas(unsigned int aWidth, unsigned int aHeight);
		~Canvas();

		void FillAnything();
		void Clear();
		void DrawLine(int x0, int y0, int x1, int y1);
		void DrawFilledRectangularRegion(int left, int top, int right, int bottom);
		void DrawImage(Image* image, int x, int y);
		void DrawSubImage(Image* image, int x, int y,
			int tileStartX, int tileStartY,
			int tileWidth, int tileHeight
			);
		void DrawImageChromaKeyed(Image* image, 
			int x, int y, 
			int chromaR, int chromaG, int chromaB);
		void DrawSubImageChromaKeyed(Image* image,
			int x, int y,
			int tileStartX, int tileStartY,
			int tileWidth, int tileHeight,
			int chromaR, int chromaG, int chromaB
		);
		void DrawText(const char* text, int x, int y);
		void DrawString(const std::string& text, int x, int y);
		void FlipHorizontally();
		void FlipVertically();
		void SetAlpha(float alpha);

		inline void SetPixel(int x, int y, int r, int g, int b) {
			unsigned char* dstLoc = pixBuffer + pitch * y + x * channels;
			*dstLoc++ = b;
			*dstLoc++ = g;
			*dstLoc++ = r;
			*dstLoc = 255;
			if (calculateRefreshRects) {
				RecordDrawCall(x, y, 1, 1);
			}
		}

		inline void SetPrimaryColor(int r, int g, int b) {
			primaryColorR = r;
			primaryColorG = g;
			primaryColorB = b;
		}

		inline void SetClearColor(int r, int g, int b) {
			clearColorR = r;
			clearColorG = g;
			clearColorB = b;
		}

		inline unsigned int GetWidth() {
			return width;
		}

		inline unsigned int GetHeight() {
			return height;
		}

		inline unsigned int GetChannels() {
			return channels;
		}

		inline unsigned int GetPitch() {
			return pitch;
		}

		inline unsigned int GetNumOfPixels() {
			return numOfPixels;
		}

		inline unsigned int GetPixelDataLength() {
			return pixelDataLength;
		}

		inline float GetLastFrameTime() {
			return lastFrameTime;
		}

		inline void SetLastFrameTime(float aLastFrameTime) {
			lastFrameTime = aLastFrameTime;
		}


		unsigned char * GetPixelBuffer() {
			return pixBuffer;
		}
		Canvas * Clone();
		
		inline void SetFont(RFont* pRFont) {
			font = pRFont;
		}

		inline RFont* GetFont() {
			return font;
		}

		inline float GetAspectRatio() {
			return aspectRatio;
		}

		inline void EnableAlphaBlending(bool flag) {
			isAlphaBlendingEnabled = flag;
		}

		inline void EnableRefreshRectCalculation(bool flag) {
			calculateRefreshRects = flag;
		}
		
		inline bool IsRefreshRectCalculationEnabled() {
			return calculateRefreshRects;
		}

		inline void BeginRefreshRectCalculation() {
			drawCallIndex = -1;
			tooManyDrawCalls = false;
		}

		inline void EndRefreshRectCalculation() {
			if (drawCallIndex > MAXREFRESHRECTS - 1) {
				tooManyDrawCalls = true;
			}
			Rect temp[MAXREFRESHRECTS];
			int n = drawCallIndex + 1;
			int k = 0;
			for (int i = 0; i < n; i++) 
			{
				//add
				//check
				if (i == 0) {
					temp[0] = refreshRectArray[0];
					++k;
				}
				else 
				{
					temp[k] = refreshRectArray[i];
					k++;
					Rect& r = temp[k-1];
					bool unionOccured = false;
					for (int j = k - 2; j >= 0; --j) {
						Rect& rPrev = temp[j];
						if (CheckCollision(r, rPrev)) {
							Rect ur;
							FindRectToRectUnion(r, rPrev, rPrev);
							unionOccured = true;
						}
					}
					if (unionOccured) {
						--k;
					}
				}
			}

			for (int i = 0; i < k; i++) {
				refreshRectArray[i] = temp[i];
			}
			drawCallIndex = k - 1;


		}

		inline bool IsTooManyDrawcalls() {
			return tooManyDrawCalls;
		}

		inline int GetDrawcallCount() {
			return drawCallIndex + 1;
		}

		inline void StartRefreshRectArrayIteration() {
			refreshRectIndex = 0;
		}

		inline bool GetRefreshRectAndIterate(Rect& r) {
			//std::cout <<"drawCallIndex " << drawCallIndex << "\n";
			if (refreshRectIndex <= drawCallIndex) {
				r = refreshRectArray[refreshRectIndex];
				++refreshRectIndex;
				return true;
			}
			else {
				return false;
			}
		}

		void RecordDrawCall(int left, int top, int width, int height) {
			++drawCallIndex;
			if (drawCallIndex < MAXREFRESHRECTS - 1) {
				Rect& r = refreshRectArray[drawCallIndex];
				r.left = left;
				r.width = width;
				r.top = top;
				r.height = height;
			}
		}


	private:
		int refreshRectIndex;
		Rect refreshRectArray[MAXREFRESHRECTS];
		int drawCallIndex = -1;
		bool tooManyDrawCalls = false;
		RFont * font;
		bool calculateRefreshRects = false;
		unsigned char * pixBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		unsigned int pitch;
		unsigned int numOfPixels;
		unsigned int pixelDataLength;
		float aspectRatio;
		float lastFrameTime;
		unsigned char primaryColorR;
		unsigned char primaryColorG;
		unsigned char primaryColorB;
		unsigned char clearColorR;
		unsigned char clearColorG;
		unsigned char clearColorB;
		bool isAlphaBlendingEnabled;

		

		void BitBlockTransfer(
			unsigned char* srcPixels,
			int srcTotalWidth,
			int srcTotalHeight,
			int dstXStart,
			int dstYStart,
			int srcXStart = 0,
			int srcYStart = 0,
			int srcWidth = -1,
			int srcHeight = -1,
			int chromaR = -1,
			int chromaG = -1,
			int chromaB = -1,
			int newColorR = -1,
			int newColorG = -1,
			int newColorB = -1
		);

	//	void CopyAllPixels(
	//		int srcWidth,
	//		int srcHeight,
	//		unsigned char * srcPixels,
	//		int dstXStart,
	//		int dstYStart);

	//	void CopyPixels(
	//		int srcXStart,
	//		int srcYStart,
	//		int srcWidth,
	//		int srcHeight,
	//		int srcTotalWidth,
	//		int srcTotalHeight,
	//		unsigned char * srcPixels,
	//		int dstXStart, 
	//		int dstYStart);

	//	void CopyPixels(
	//		int srcXStart,
	//		int srcYStart,
	//		int srcWidth,
	//		int srcHeight,
	//		int srcTotalWidth,
	//		int srcTotalHeight,
	//		unsigned char * srcPixels,
	//		int dstXStart,
	//		int dstYStart,
	//		int chroma_r,
	//		int chroma_g,
	//		int chroma_b
	//		);

	//	void CopyPixels(
	//		int srcXStart,
	//		int srcYStart,
	//		int srcWidth,
	//		int srcHeight,
	//		int srcTotalWidth,
	//		int srcTotalHeight,
	//		unsigned char * srcPixels,
	//		int dstXStart,
	//		int dstYStart,
	//		int chroma_r,
	//		int chroma_g,
	//		int chroma_b,
	//		int textcolor_r,
	//		int textcolor_g,
	//		int textcolor_b
	//		);
	};
}


#endif
