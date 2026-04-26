#ifndef SPN_IMAGE_H
#define SPN_IMAGE_H

namespace spn
{
	class Canvas;

	enum ScaleMethod {
		NEAREST,
		BLERP
	};

	class Image
	{
	public:
		Image(unsigned int aWidth, unsigned int aHeight);
		Image(Canvas* aCanvas);
		Image();
		~Image();
		bool CreateSolidColorBlockImage(int width, int height,
			int colorR, int colorG, int colorB, float alpha);
		bool CreateCheckerImage(int width, int height, 
			int tileSize, 
			int color1R, int color1G, int color1B, 
			int color2R, int color2G, int color2B);
		bool CreateFromSvg(const char* fileName, float dpi);
		bool CreateFromPng(const char* fileName);
		bool CreateFromPpmRaw(const char* fileName);
		bool CreateFromPpmAscii(const char* fileName);
		bool SaveAsPng(const char* fileName);
		inline Canvas* GetCanvas() {
			return canvas;
		}
		inline void SetCanvas(Canvas *aCanvas) {
			canvas = aCanvas;
		}
		Image* Clone();
		void Scale(ScaleMethod sm, Image* dstImage);
	private:
		Canvas *canvas;
		inline float lerp(float s, float e, float t) {
			return s + t * (e - s);
		}
		void ScaleNearest(Image* dstImage);
		void ScaleBlerp(Image* dstImage);
		void DestroyCanvas();

	};
}

#endif 
