#pragma once

#include <vector>

namespace Archivator
{

struct RawImageData {
    int width { 0 };  // image width in pixels
    int height { 0 }; // image height in pixels
    std::vector<unsigned char> data; // Image data. data[j * width + i] is color of pixel in row j and column i.
};

[[nodiscard]] RawImageData compressImage(const RawImageData& imageData, std::vector<unsigned char>& indexes);
[[nodiscard]] RawImageData decompressImage(const RawImageData& compressedImageData, const std::vector<unsigned char>& indexes);

} // Archivator
