#include "worker.h"

#include <QtConcurrent/QtConcurrent>
#include <QImage>

#include "utils/barchutils.h"

#include <archivator.h>
#include <iostream>

Worker::Worker(QObject *parent)
    : QObject{parent}
{

}

void Worker::process(const QString& fileName)
{
    [[maybe_unused]] auto future = QtConcurrent::run([this, fileName] () {
        if (fileName.endsWith(".barch")) {

            Archivator::RawImageData data;
            std::vector<unsigned char> indexes;

            if (!Internal::parseBarchFile(fileName, data, indexes)) {
                qDebug() << "[ Worker ] Failed to load the image: " << fileName;

                emit error("Failed to load the image");
                return;
            }

            auto decompressed = Archivator::decompressImage(data, indexes);

            auto filled = QImage(decompressed.data.data(), decompressed.width, decompressed.height, QImage::Format_Grayscale8);
            if (filled.isNull()) {
                emit error("Failed to load the image");
                return;
            }
            // Save the grayscale image to the destination path
            if (!filled.save("decompressed_test.png")) {
                qDebug() << "Failed to save the grayscale image to:" << "image_test.png";
                return;
            }

            qDebug() << "[ Worker ] Decompressed!";

            emit finished();
            return;
        }

        QImage image(fileName);

        if (image.isNull()) {
            qDebug() << "[ Worker ] Failed to load the image: " << fileName;

            emit error("Failed to load the image");
            return;
        }

        Archivator::RawImageData data;

        // Convert the image to grayscale
        QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);

        uchar* originalData = grayImage.bits();
        int originalBytesPerLine = grayImage.bytesPerLine();

        data.data.resize(grayImage.sizeInBytes());

        //! AK: data deep  copy
        data.data = std::vector<unsigned char>(grayImage.bits(), grayImage.bits() + grayImage.sizeInBytes());
        data.width = grayImage.width();
        data.height = grayImage.height();

        auto filled = QImage(data.data.data(), data.width, data.height, QImage::Format_Grayscale8);
        if (filled.isNull()) {
            emit error("Failed to load the image");
            return;
        }

        if (!filled.save("image_test_decomp.png")) {
            qDebug() << "Failed to save the grayscale image to:" << "image_test.png";
            return;
        }

        std::vector<unsigned char> indexes;
        auto compressed = Archivator::compressImage(data, indexes);

        auto decompressed = Archivator::decompressImage(compressed, indexes);

        if (!Internal::saveBarchFile("compressed.barch", data, indexes)) {
            qDebug() << "[ Worker ] Could not save compressed data!";

            emit error("Could not save compressed data!");
            return;
        }

        qDebug() << "[ Worker ] Compressed!";

        emit finished();
    });
}

