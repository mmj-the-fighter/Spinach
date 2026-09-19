/*
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <https://unlicense.org/>
*/

//Program
//for splitting rectangle into tiles in tiled rendering scenarios
#ifndef TILER_H
#define TILER_H
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace rtutil
{
    struct Rect
    {
        int left;
        int top;
        int width;
        int height;
    };

    inline static Rect MakeRect(int x, int y, int w, int h) {
        Rect r;
        r.left = x;
        r.top = y;
        r.width = w;
        r.height = h;
        return r;
    }

    static std::vector<Rect> SplitRectangleToTiles(Rect rect, int maximumNumberOfRows, int maximumNumberOfColumns) {
        if (maximumNumberOfRows <= 0 || maximumNumberOfColumns <= 0) {
            return std::vector<Rect>();
        }
        int x = rect.left;
        int y = rect.top;
        int tileHeight;
        int tileWidth;
        int rowCount = 0;
        int columnCount = 0;
        std::vector<Rect> tiles;

        int normalTileHeight = rect.height / maximumNumberOfRows;
        int lastTileHeight = rect.height - (maximumNumberOfRows - 1) * normalTileHeight;

        int normalTileWidth = rect.width / maximumNumberOfColumns;
        int lastTileWidth = rect.width - (maximumNumberOfColumns - 1) * normalTileWidth;

        tileHeight = normalTileHeight;
        while (rowCount < maximumNumberOfRows) {
            if (rowCount == maximumNumberOfRows - 1) {
                tileHeight = lastTileHeight;
            }
            columnCount = 0;
            x = rect.left;
            tileWidth = normalTileWidth;
            while (columnCount < maximumNumberOfColumns) {
                if (columnCount == maximumNumberOfColumns - 1) {
                    tileWidth = lastTileWidth;
                }
                tiles.push_back(MakeRect(x, y, tileWidth, tileHeight));
                x += tileWidth;
                ++columnCount;
            }
            y += tileHeight;
            ++rowCount;
        }
        return tiles;
    }

    // AI-generated
    static void ComputeGrid(int N, int W, int H, int& rows, int& cols, int k = 2) {
        int T = k * N;
        double aspect = double(W) / double(H);
        cols = (std::max)(1, (int)std::round(std::sqrt(T * aspect)));
        rows = (std::max)(1, (int)std::round(double(T) / cols));
    }
}

#endif
