#include <algorithm>
#include "imageproc.h"
#include "../spinach/core/spn_canvas.h"


//This function was developed with the help of ChatGPT AI agent
void ApplySepiaFilter(spn::Image *image)
{
	spn::Canvas *canvas = image->GetCanvas();
	unsigned char* pixels = canvas->GetPixelBuffer();
	double b, g, r;
	int nb, ng, nr;
	unsigned int n = canvas->GetPixelDataLength();
	for (unsigned int i = 0; i < n; i += 4) {
		b = pixels[i];
		g = pixels[i + 1];
		r = pixels[i + 2];
		nb = 0.272 * r + 0.534 * g + 0.131 * b;
		ng = 0.349 * r + 0.686 * g + 0.168 * b;
		nr = 0.393 * r + 0.769 * g + 0.189 * b;
		pixels[i] = std::min(nb, 255);
		pixels[i + 1] = std::min(ng, 255);
		pixels[i + 2] = std::min(nr, 255);
	}
}