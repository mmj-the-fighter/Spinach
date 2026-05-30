//AI generated code by Google Gemini and Anthropic Claude
#ifndef BLUE_NOISE_H

#define BLUE_NOISE_H

#include <stdint.h>
#include <vector>
#include <array>
#include <random>
#include <cstring>
#include <cfloat>

static inline float ToroidalDistanceSq(size_t x1, size_t y1, size_t x2, size_t y2, size_t imageSize)
{
    float dx = std::abs((float)x2 - (float)x1);
    float dy = std::abs((float)y2 - (float)y1);

    float half = (float)(imageSize / 2);
    if (dx > half) dx = (float)imageSize - dx;
    if (dy > half) dy = (float)imageSize - dy;

    return dx*dx + dy*dy;
}

/**
 * Generates blue noise using Mitchell's Best-Candidate algorithm.
 *
 * @param buffer        Output buffer, must be at least imageSize*imageSize bytes.
 * @param imageSize     Width/height of the square buffer (e.g. 256).
 * @param numSamples    Number of pixels to set to 255.
 * @param multiplier    Candidate multiplier — higher = better quality, but O(n²) slower.
 */
static inline void GenerateBlueNoiseMitchell(
    unsigned char* buffer,
    size_t imageSize,
    size_t numSamples,
    size_t multiplier = 1)
{
    if (!buffer || imageSize == 0 || numSamples == 0) return;

    memset(buffer, 0, imageSize * imageSize);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, imageSize - 1);

    std::vector<std::array<size_t, 2>> placed;
    placed.reserve(numSamples);

    for (size_t i = 0; i < numSamples; ++i)
    {
        size_t numCandidates = placed.size() * multiplier + 1;

        float   bestDist = -1.0f;
        size_t  bestX = 0, bestY = 0;

        for (size_t c = 0; c < numCandidates; ++c)
        {
            size_t x = dist(rng);
            size_t y = dist(rng);

            // Find this candidate's distance to its nearest neighbour
            float minDist = FLT_MAX;
            for (const auto& s : placed)
            {
                float d = ToroidalDistanceSq(x, y, s[0], s[1], imageSize);
                if (d < minDist) minDist = d;
            }

            // Keep the candidate that is farthest from all existing samples
            if (minDist > bestDist)
            {
                bestDist = minDist;
                bestX = x;
                bestY = y;
            }
        }

        placed.push_back({ bestX, bestY });
        buffer[bestY * imageSize + bestX] = 255;
    }
}

/**
 * Samples a tileable blue noise buffer at image coordinates.
 *
 * @param noiseBuffer  The buffer produced by GenerateBlueNoiseMitchell.
 * @param noiseSize    The imageSize used when generating (e.g. 256).
 * @param imageX       X coordinate in the destination image (any size).
 * @param imageY       Y coordinate in the destination image (any size).
 */
static inline bool SampleBlueNoise(
    const unsigned char* noiseBuffer,
    size_t noiseSize,
    int imageX, int imageY)
{
    int nx = imageX % (int)noiseSize;
    int ny = imageY % (int)noiseSize;
    return noiseBuffer[ny * noiseSize + nx] == 255;
}

#endif
