#pragma once

#include <vector>

namespace Archivator
{

struct RawImageData {
    int width { 0 };  // image width in pixels
    int height { 0 }; // image height in pixels
    unsigned char* data { nullptr }; // Pointer to image data. data[j * width + i] is color of pixel in row j and column i.
};

std::vector<int> compressImage(const RawImageData& imageData);

} // Archivator
