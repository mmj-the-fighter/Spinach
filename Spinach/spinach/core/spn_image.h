#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>

namespace spn
{
	class Canvas;

	class Image
	{
	public:
		Image(unsigned int aWidth, unsigned int aHeight);
		Image(Canvas* aCanvas);
		Image();
		~Image();
		bool CreateFromPng(const std::string& fileName);
		bool CreateFromPpmRaw(const std::string& fileName);
		bool CreateFromPpmAscii(const std::string& fileName);
		bool SaveAsPng(const std::string& fileName);
		inline Canvas* GetCanvas() {
			return canvas;
		}
		inline void SetCanvas(Canvas *aCanvas) {
			canvas = aCanvas;
		}
		Image* Clone();

	private:
		Canvas *canvas;
		void DestroyCanvas();
	};
}

#endif 
