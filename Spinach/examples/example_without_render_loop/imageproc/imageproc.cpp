#include <algorithm>
#include <vector>
#include <spn_canvas.h>
#include "imageproc.h"



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


//This function was developed with the help of ChatGPT AI agent
void ApplyHalfToning(spn::Image *image)
{
    spn::Canvas *canvas = image->GetCanvas();

    const int width  = canvas->GetWidth();
    const int height = canvas->GetHeight();

    unsigned char* pixels = canvas->GetPixelBuffer();

    // Floating-point greyscale buffer.
    std::vector<float> grey(width * height);

    // Convert image to greyscale.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int pixel = (y * width + x) * 4;

            float b = pixels[pixel + 0];
            float g = pixels[pixel + 1];
            float r = pixels[pixel + 2];

            grey[y * width + x] =
                0.114f * b +
                0.587f * g +
                0.299f * r;
        }
    }

    constexpr float threshold = 128.0f;

    // Floyd-Steinberg error diffusion.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;

            float oldPixel = grey[index];

            unsigned char newPixel =
                (oldPixel >= threshold) ? 255u : 0u;

            float error = oldPixel - static_cast<float>(newPixel);

            int pixel = index * 4;

            pixels[pixel + 0] = newPixel;
            pixels[pixel + 1] = newPixel;
            pixels[pixel + 2] = newPixel;
            // Leave alpha unchanged.

            // Right (7/16)
            if (x + 1 < width)
                grey[index + 1] += error * (7.0f / 16.0f);

            if (y + 1 < height)
            {
                // Bottom-left (3/16)
                if (x > 0)
                    grey[(y + 1) * width + (x - 1)] +=
                        error * (3.0f / 16.0f);

                // Bottom (5/16)
                grey[(y + 1) * width + x] +=
                    error * (5.0f / 16.0f);

                // Bottom-right (1/16)
                if (x + 1 < width)
                    grey[(y + 1) * width + (x + 1)] +=
                        error * (1.0f / 16.0f);
            }
        }
    }
}