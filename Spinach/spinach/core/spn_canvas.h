#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>

namespace spn 
{
	class Image;
	class RFont;

	class Canvas
	{
	public:
		Canvas(unsigned int aWidth, unsigned int aHeight);
		~Canvas();

		void FillAnything();
		void DrawImage(Image* image, int x, int y);
		void DrawText(const std::string& text, int x, int y);
		inline void SetPrimaryColor(int r, int g, int b) {
			primaryColorR = r;
			primaryColorG = g;
			primaryColorB = b;
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
		
		inline void SetFont(RFont* pRFont){
			font = pRFont;
		}

		inline RFont* GetFont(){
			return font;
		}

	private:
		RFont * font;
		unsigned char * pixBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		unsigned int pitch;
		unsigned int numOfPixels;
		unsigned int pixelDataLength;
		float lastFrameTime;
		unsigned char primaryColorR;
		unsigned char primaryColorG;
		unsigned char primaryColorB;

		void CopyAllPixels(
			int srcWidth,
			int srcHeight,
			unsigned char * srcPixels,
			int dstXStart,
			int dstYStart);

		void CopyPixels(
			int srcXStart,
			int srcYStart,
			int srcWidth,
			int srcHeight,
			int srcTotalWidth,
			int srcTotalHeight,
			unsigned char * srcPixels,
			int dstXStart, 
			int dstYStart);

		void CopyPixels(
			int srcXStart,
			int srcYStart,
			int srcWidth,
			int srcHeight,
			int srcTotalWidth,
			int srcTotalHeight,
			unsigned char * srcPixels,
			int dstXStart,
			int dstYStart,
			int chroma_r,
			int chroma_g,
			int chroma_b
			);

		void CopyPixels(
			int srcXStart,
			int srcYStart,
			int srcWidth,
			int srcHeight,
			int srcTotalWidth,
			int srcTotalHeight,
			unsigned char * srcPixels,
			int dstXStart,
			int dstYStart,
			int chroma_r,
			int chroma_g,
			int chroma_b,
			int textcolor_r,
			int textcolor_g,
			int textcolor_b
			);
	};
}


#endif