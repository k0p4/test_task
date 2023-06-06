#include "archivator.h"

#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>

namespace Archivator
{

namespace Internal
{

void writeBit(unsigned char& data, size_t bitIndex, bool value)
{
    size_t bitPosition = 7 - bitIndex;

    if (value) {
        data |= (1 << bitPosition);
    } else {
        data &= ~(1 << bitPosition);
    }
}

bool readBit(const unsigned char& data, size_t bitIndex)
{
    size_t bitPosition = 7 - bitIndex;
    return ((data >> bitPosition) & 1) != 0;
}

} // Internal

RawImageData compressImage(const RawImageData& imageData, std::vector<unsigned char>& indexes)
{
    RawImageData result;
    result.width = imageData.width;
    result.height = imageData.height;
    result.data.reserve(imageData.data.size() / 2);
    indexes.reserve(imageData.height);

    for (size_t rowIdx = 0; rowIdx < imageData.height; ++rowIdx) {
        bool empty { true };

        for (size_t colIdx = 0; colIdx < imageData.width; ++colIdx) {
            if (imageData.data[rowIdx * imageData.width + colIdx] != 0xFF) {
                empty = false;
                break;
            }
        }

        if (empty) {
            indexes.push_back(0);
        } else {
            indexes.push_back(1);
        }
    }

    size_t imageIdx { 0 };
    unsigned char currentByte { 0 };
    size_t currentBitIdx { 0 };
    size_t colIdx { 0 };

    auto writeBit = [&] (bool value) {
        Internal::writeBit(currentByte, currentBitIdx, value);

        ++currentBitIdx;

        if (currentBitIdx == 8) {
            result.data.push_back(currentByte);
            currentByte = 0;
            currentBitIdx = 0;
        }
    };

    auto flushUncompressed = [&] () {
        writeBit(true);
        writeBit(true);

        result.data.push_back(currentByte);

        for (size_t ix = 0; ix < 4; ++ix) {
            result.data.push_back(imageData.data[imageIdx + ix]);
        }

        currentBitIdx = 0;
        currentByte = 0;
        colIdx += 4;
    };

    for (size_t rowIdx = 0; rowIdx < imageData.height; ++rowIdx) {

        if (!indexes[rowIdx]) {
            continue;
        }

        colIdx = 0;
        while (colIdx + 4 < imageData.width) {
            imageIdx = rowIdx * imageData.width + colIdx;
            auto pixel = imageData.data[imageIdx];

            switch (pixel) {
            case 0xFF:
                // 4 in a row
                if (imageData.data[imageIdx + 1]  == 0xFF &&
                    imageData.data[imageIdx + 2]  == 0xFF &&
                    imageData.data[imageIdx + 3]  == 0xFF) {

                    writeBit(false);

                    colIdx += 4;
                    continue;
                }

                flushUncompressed();

                break;
            case 0:
                // 4 in a row
                if (imageData.data[imageIdx + 1] == 0 &&
                    imageData.data[imageIdx + 2] == 0 &&
                    imageData.data[imageIdx + 3] == 0) {

                    writeBit(true);
                    writeBit(false);

                    colIdx += 4;
                    continue;
                }

                flushUncompressed();

                break;
            default: // random
                flushUncompressed();
                break;
            }
        }

        auto bytes = imageData.width - colIdx;

        if (bytes == 0) {
            continue;
        }

        if (currentBitIdx != 0) {
            result.data.push_back(currentByte);
            currentByte = 0;
            currentBitIdx = 0;
        }

        while (bytes > 0) {
            result.data.push_back(imageData.data[rowIdx * imageData.width + imageData.width - bytes]);
            bytes--;
        }
    }

    return result;
}

RawImageData decompressImage(const RawImageData& compressedImageData, const std::vector<unsigned char>& indexes)
{
    RawImageData result;
    result.width = compressedImageData.width;
    result.height = compressedImageData.height;
    result.data.reserve(result.height);

    size_t currentBitIdx = 0;
    size_t processedBytes { 0 };
    size_t colIdx { 0 };

    auto readBit = [&] () {
        bool bit = Internal::readBit(compressedImageData.data[processedBytes], currentBitIdx);
        ++currentBitIdx;

        if (currentBitIdx == 8) {
            currentBitIdx = 0;
            ++processedBytes;
        }

        return bit;
    };

    for (size_t rowIdx = 0; rowIdx < compressedImageData.height; ++rowIdx) {
        if (indexes[rowIdx] == 0) {
            for (size_t i = 0; i < result.width; ++i) {
                result.data.push_back(0xFF);
            }

            continue;
        }

        colIdx = 0;

        while (colIdx < result.width) {
            if (colIdx + 4 >= result.width) { // AK: possibly it would be better to move last bytes checking after while.

                auto bytes = result.width - colIdx;

                if (currentBitIdx != 0) {
                    ++processedBytes;
                    currentBitIdx = 0;
                }

                for (size_t i = 0; i < bytes; ++i) {
                    result.data.push_back(compressedImageData.data[processedBytes + i]);
                }

                processedBytes += bytes;

                break;
            }

            bool bit = readBit();

            if (!bit) {
                for (size_t i = 0; i < 4; ++i) {
                    result.data.push_back(0xFF);
                }

                colIdx += 4;
                continue;
            }

            bool second = readBit();

            if (second) {
                ++processedBytes;

                for (size_t ix = 0; ix < 4; ++ix) {
                    result.data.push_back(compressedImageData.data[processedBytes + ix]);
                }

                colIdx += 4;
                processedBytes += 4;
                currentBitIdx = 0;
            } else {
                for (size_t i = 0; i < 4; ++i) {
                    result.data.push_back(0);
                }

                colIdx += 4;
                continue;
            }
        }
    }

    return result;
}

} // namespace Archivator

