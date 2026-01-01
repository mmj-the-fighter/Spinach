#ifndef SPN_CANVAS_H
#define SPN_CANVAS_H

#include <string>
#include "../common/spn_utils.h"

namespace spn 
{
	struct Vec2 { float x, y; Vec2() {} Vec2(float X, float Y) :x(X), y(Y) {} };

	class Image;
	class RFont;

	class Canvas
	{
	public:
		Canvas(unsigned int aWidth, unsigned int aHeight);
		~Canvas();

		void FillAnything();
		void Clear();
		void SetClippingRectangle(int left, int top, int widht, int height);
		inline bool IsOutsideBounds(int x, int y) {
			return (x < 0 || x > width - 1 || y < 0 || y > height - 1);
		}

		inline bool IsInsideBounds(int x, int y) {
			return (x >= 0 && x < width && y >= 0 && y < height);
		}
		void DrawLine(int x0, int y0, int x1, int y1);
		
		inline void ClipLineNaive(int& x0, int& y0, int& x1, int& y1) {
			if (x0 < 0) x0 = 0;
			if (x0 >= width) x0 = width - 1;
			if (y0 < 0) y0 = 0;
			if (y0 >= height) y0 = height - 1;
			if (x1 < 0) x1 = 0;
			if (x1 >= width) x1 = width - 1;
			if (y1 < 0) y1 = 0;
			if (y1 >= height) y1 = height - 1;
		}
		void DrawRectangle(int left, int top, int right, int bottom);
		void DrawFilledRectangle(int left, int top, int right, int bottom);
		void DrawCircle(int x, int y, int radius);
		void DrawCubicBezierUniform(
			float x0, float y0,
			float x1, float y1,
			float x2, float y2,
			float x3, float y3,
			int segments = 32);
		void DrawArc(float x0, float y0,
			float rx, float ry,
			float xAxisRotationDeg,
			bool largeArcFlag, bool sweepFlag,
			float x1, float y1);
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
		void DrawCString(const char* text, int x, int y);
		void DrawCStringInRange(const char* text, int startIndex, int endIndex, int x, int y);
		void DrawString(const std::string& text, int x, int y);
		void GetCharDisplaySize(char c, float& w, float& h);
		void GetCStringDisplaySize(const char* text, float& w, float& h);
		void GetStringDisplaySize(const std::string& text, float& w, float& h);
		void GetStringDisplaySizeInRange(const std::string& text, int start, int end, float& w, float& h);

		void FlipHorizontally();
		void FlipVertically();
		void SetAlpha(float alpha);

		inline void SetPixel(int x, int y, int r, int g, int b) {
			unsigned char* dstLoc = pixBuffer + pitch * y + x * channels;
			*dstLoc++ = b;
			*dstLoc++ = g;
			*dstLoc++ = r;
			*dstLoc = 255;
		}
		inline void SetPixelWithPrimaryColor(int x, int y) {
			if (x < 0 || x > width - 1 || y < 0 || y > height - 1) {
				return;
			}
			unsigned char* dstLoc = pixBuffer + pitch * y + x * channels;
			*dstLoc++ = primaryColorB;
			*dstLoc++ = primaryColorG;
			*dstLoc++ = primaryColorR;
			*dstLoc = 255;
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

		inline void SetPrimaryColorUint(unsigned int color) {
			primaryColorR = (unsigned char)((color >> 16) & 0xFF);
			primaryColorG = (unsigned char)((color >> 8) & 0xFF);
			primaryColorB = (unsigned char)(color & 0xFF);
		}

		inline void SetClearColorUint(unsigned int color) {
			clearColorR = (unsigned char)((color >> 16) & 0xFF);
			clearColorG = (unsigned char)((color >> 8) & 0xFF);
			clearColorB = (unsigned char)(color & 0xFF);
		}

		inline void SaveColors() {
			originalPrimaryColorR = primaryColorR;
			originalPrimaryColorG = primaryColorG;
			originalPrimaryColorB = primaryColorB;
			originalClearColorR = clearColorR;
			originalClearColorG = clearColorG;
			originalClearColorB = clearColorB;
		}

		inline void RestoreColors() {
			primaryColorR = originalPrimaryColorR;
			primaryColorG = originalPrimaryColorG;
			primaryColorB = originalPrimaryColorB;
			clearColorR = originalClearColorR;
			clearColorG = originalClearColorG;
			clearColorB = originalClearColorB;
		}

	private:
		Rect clipRect;
		RFont * font;
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

		unsigned char originalPrimaryColorR;
		unsigned char originalPrimaryColorG;
		unsigned char originalPrimaryColorB;

		unsigned char originalClearColorR;
		unsigned char originalClearColorG;
		unsigned char originalClearColorB;

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
