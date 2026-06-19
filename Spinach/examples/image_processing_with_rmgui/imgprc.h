#ifndef IMG_PRC_H
#define IMG_PRC_H
#define NOISEIMGSIZE 32
#include <bluenoise.h>
#include <spn_rng.h>

struct RoiRect{
	int x0;
	int y0;
	int x1;
	int y1;
};

spn::RandomGen& rng = spn::RandomGen::GetInstance();

static unsigned char noisebuffer[NOISEIMGSIZE * NOISEIMGSIZE];
unsigned char* fullyFilteredImagePixels;
unsigned char* smoothedImagePixels;
unsigned char* horizontallyFilteredImagePixels;

static inline void ImgPrcInit(int width, int height, int channels){
	GenerateBlueNoiseMitchell(noisebuffer, NOISEIMGSIZE, 512, 2);
	fullyFilteredImagePixels = new unsigned char[width * height * channels];
	horizontallyFilteredImagePixels = new unsigned char[width * height * channels];
	smoothedImagePixels = new unsigned char[width * height * channels];
}

static inline void ImgPrcDeInit(){
	delete[] fullyFilteredImagePixels;
	delete[] horizontallyFilteredImagePixels;
	delete[] smoothedImagePixels;
}

static inline void Blur(unsigned char* op, unsigned char* ip, int w, int h, int channels, const RoiRect& roi,int kernelSize) {
	
	int halfKernelSize = kernelSize / 2;
	//init horizontal and vertical kernels (for box filtering)
	float filterValue = 1.0f / (float)kernelSize;

	//apply filter horizontally
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) continue;
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;

			for (int i = -halfKernelSize; i <= halfKernelSize; ++i) {
				int sampleLocationX = std::clamp(x + i, 0, w - 1);
				unsigned char* buffer = ip +
					(w * channels * y) +
					(sampleLocationX * channels);
				bs += filterValue * *buffer;
				gs += filterValue * *(buffer + 1);
				rs += filterValue * *(buffer + 2);
			}
			unsigned char* outbuffer =
				horizontallyFilteredImagePixels +
				(w * channels * y) +
				(x * channels);
			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	bool nop = false;

	//apply filter vertically
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) nop = true;
			if (nop) {
				unsigned char* buffer = ip +
					(w * channels * y) +
					(x * channels);
				unsigned char* outbuffer =
					op +
					(w * channels * y) +
					(x * channels);
				*outbuffer = *buffer;
				*(outbuffer + 1) = *(buffer + 1);
				*(outbuffer + 2) = *(buffer + 2);
				*(outbuffer + 3) = 255; //alpha
				nop = false;
				continue;
			}
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (int i = -halfKernelSize; i <= halfKernelSize; ++i) {
				int sampleLocationY = std::clamp(y + i, 0, h - 1);
				unsigned char* buffer =
					horizontallyFilteredImagePixels +
					(w * channels * sampleLocationY) +
					(x * channels);
				bs += filterValue * *buffer;
				gs += filterValue * *(buffer + 1);
				rs += filterValue * *(buffer + 2);
			}

			unsigned char* outbuffer =
				smoothedImagePixels +
				(w * channels * y) +
				(x * channels);

			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}

	}

}

static inline void BlueNoise(
	unsigned char* op, 
	unsigned char* ip, 
	int w, int h, int channels, 
	const RoiRect& roi,
	float noiseAmount) {
	bool nop = false;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) nop = true;
			if (nop) {
				unsigned char* buffer = ip +
					(w * channels * y) +
					(x * channels);
				unsigned char* outbuffer =
					op +
					(w * channels * y) +
					(x * channels);
				*outbuffer = *buffer;
				*(outbuffer + 1) = *(buffer + 1);
				*(outbuffer + 2) = *(buffer + 2);
				*(outbuffer + 3) = 255; //alpha
				nop = false;
				continue;
			}
			unsigned char* buffer = ip +
				(w * channels * y) +
				(x * channels);
			float brs, grs, rrs;
			if (SampleBlueNoise(noisebuffer, NOISEIMGSIZE, x, y)) {
				float displ = (2.0 * rng.GenerateFloat() - 1.0) * noiseAmount;
				brs = *buffer + displ;
				grs = *(buffer + 1) + displ;
				rrs = *(buffer + 2) + displ;
				brs = std::clamp(brs, 0.0f, 255.0f);
				grs = std::clamp(grs, 0.0f, 255.0f);
				rrs = std::clamp(rrs, 0.0f, 255.0f);
			}
			else {
				brs = *buffer;
				grs = *(buffer + 1);
				rrs = *(buffer + 2);
			}
			unsigned char* outbuffer =
				op +
				(w * channels * y) +
				(x * channels);
			*outbuffer = brs;
			*(outbuffer + 1) = grs;
			*(outbuffer + 2) = rrs;
			*(outbuffer + 3) = 255; //alpha
		}
	}
}


static inline void WhiteNoise(
	unsigned char* op,
	unsigned char* ip,
	int w, int h, int channels,
	const RoiRect& roi,
	float noiseAmount) {
	bool nop = false;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) nop = true;
			if (nop) {
				unsigned char* buffer = ip +
					(w * channels * y) +
					(x * channels);
				unsigned char* outbuffer =
					op +
					(w * channels * y) +
					(x * channels);
				*outbuffer = *buffer;
				*(outbuffer + 1) = *(buffer + 1);
				*(outbuffer + 2) = *(buffer + 2);
				*(outbuffer + 3) = 255; //alpha
				nop = false;
				continue;
			}
			unsigned char* buffer = ip +
				(w * channels * y) +
				(x * channels);
			float displ = (2.0 * rng.GenerateFloat() - 1.0) * noiseAmount;
			float brs = *buffer + displ;
			float grs = *(buffer + 1) + displ;
			float rrs = *(buffer + 2) + displ;
			brs = std::clamp(brs, 0.0f, 255.0f);
			grs = std::clamp(grs, 0.0f, 255.0f);
			rrs = std::clamp(rrs, 0.0f, 255.0f);
			unsigned char* outbuffer =
				op +
				(w * channels * y) +
				(x * channels);
			*outbuffer = brs;
			*(outbuffer + 1) = grs;
			*(outbuffer + 2) = rrs;
			*(outbuffer + 3) = 255; //alpha
		}
	}
}



/*

Thresholding partitions the the image into black and white

Pixels having intensity less than the threshold intensity
will become black

And pixels having intensity greater than or equal to threshold
will become white

The threshold intensity has a minimum value of 0
and a maximum value of 255

Automatic threshold demonstrated here is the
average intensity of the image

Note that the intensity calculation used
here is an approximation

*/

void Threshold(
unsigned char* op, unsigned char* ip, 
int w, int h, int channels, 
const RoiRect& roi, float thresh) {
	bool nop = false;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x){
			if (y < roi.y0 || y > roi.y1 || x < roi.x0 || x > roi.x1) nop=true;
			if (nop) {
				unsigned char* buffThreshold =
					ip + 
					(w * channels * y) +
					(x * channels);
				unsigned char* outbuffer =
					op +
					(w * channels * y) +
					(x * channels);
				*outbuffer = *buffThreshold;
				*(outbuffer + 1) = *(buffThreshold + 1);
				*(outbuffer + 2) = *(buffThreshold + 2);
				*(outbuffer + 3) = 255; //alpha
				nop = false;
				continue;
			}
			int i = (w * channels * y) + (x * channels);
			unsigned char* src = ip + i;
			unsigned char* dst = op + i;
			float intensity = static_cast<float>(*src + *(src + 1) + *(src + 2)) / 3.0f;
			
			if(intensity < thresh){
				*dst = 0;
				*(dst+1) = 0;
				*(dst+2) = 0;
				*(dst+3) = 255;
			}else{
				*dst = *src;
				*(dst+1) = *(src+1);
				*(dst+2) = *(src+2);
				*(dst+3) = 255;
			}
		}
	}

}


float GetAvgIntensity(unsigned char* ip, int n) {
	float sum = 0;
	int count = 0;
	for (int i = 0; i < n; ++i)
	{
		int b = *ip;
		int g = *(ip + 1);
		int r = *(ip + 2);
		float intensity = (float)((b + g + r)) / 3.0f;
		sum += intensity;
		++count;
		ip += 4;
	}
	float averageIntensity = sum / (float)count;
	return averageIntensity;
}


#endif