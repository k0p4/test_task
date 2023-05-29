#include "worker.h"

#include <QtConcurrent/QtConcurrent>
#include <QImageWriter>

#include "utils/barchutils.h"

#include <archivator.h>

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

            QImage image(decompressed.data.data(), decompressed.width, decompressed.height, QImage::Format_Grayscale8);

            QImageWriter writer("decompressed.png", "PNG");
            writer.write(image);

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

        data.width = image.width();
        data.height = image.height();
        data.data = std::vector<unsigned char>(image.bits(), image.bits() + image.sizeInBytes());

        std::vector<unsigned char> indexes;
        auto compressed = Archivator::compressImage(data, indexes);

        if (!Internal::saveBarchFile("compressed.barch", data, indexes)) {
            qDebug() << "[ Worker ] Could not save compressed data!";

            emit error("Could not save compressed data!");
            return;
        }

        emit finished();
    });
}

