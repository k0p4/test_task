#include "worker.h"

#include <QtConcurrent/QtConcurrent>
#include <QImage>

#include <archivator.h>

#include "utils/barchutils.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{

}

void Worker::process(const QString& fileName, const QString& dir)
{
    [[maybe_unused]] auto future = QtConcurrent::run([this, fileName, dir] () {
        if (fileName.endsWith(".barch")) {

            Archivator::RawImageData data;
            std::vector<unsigned char> indexes;

            if (!Internal::parseBarchFile(dir + "/" + fileName, data, indexes)) {
                qDebug() << "[ Worker ] Failed to load the image: " << dir + fileName;

                emit error("Failed to load the image");
                return;
            }

            auto decompressed = Archivator::decompressImage(data, indexes);

            auto padding = 4 - decompressed.width % 4;

            std::vector<unsigned char> padded;
            std::vector<unsigned char>* target;

            if (padding == 4) {
                target = &decompressed.data;
            } else {
                padded.resize((decompressed.width + padding) * decompressed.height);

                for (size_t y = 0; y < decompressed.height; ++y) {
                    memcpy(&padded[y * (data.width + padding)], &decompressed.data[y * data.width], data.width);
                    memset(&padded[y * (data.width + padding) + data.width], 0, padding);
                }

                target = &padded;
            }

            auto filled = QImage(target->data(), decompressed.width, decompressed.height, QImage::Format_Grayscale8);
            if (filled.isNull()) {
                emit error("Failed to load the image");
                return;
            }

            QFileInfo fInfo(dir + "/" + fileName);

            if (!filled.save(fInfo.filePath().remove("_packed.barch") + "_unpacked.bmp")) {
                qDebug() << "Failed to save the grayscale image to:" << "image_test.png";
                return;
            }

            qDebug() << "[ Worker ] Decompressed!";

            emit finished();
            return;
        }

        QImage image(dir + "/" + fileName);

        if (image.isNull()) {
            qDebug() << "[ Worker ] Failed to load the image: " << dir + "/" + fileName;

            emit error("Failed to load the image");
            return;
        }

        QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);

        Archivator::RawImageData data;
        data.width = grayImage.width();
        data.height = grayImage.height();
        data.data.resize(grayImage.width() * grayImage.height());

        for (int y = 0; y < grayImage.height(); y++) {
            const auto cdata = grayImage.constScanLine(y);

            memcpy(&data.data[y * data.width], cdata, data.width);
        }

        std::vector<unsigned char> indexes;
        auto compressed = Archivator::compressImage(data, indexes);

        QFileInfo fInfo(dir + "/" + fileName);

        qDebug() << fileName;
        qDebug() << fInfo.absoluteFilePath() + "_packed.barch";

        if (!Internal::saveBarchFile(fInfo.absoluteFilePath() + "_packed.barch", compressed, indexes)) {
            qDebug() << "[ Worker ] Could not save compressed data!";

            emit error("Could not save compressed data!");
            return;
        }

        qDebug() << "[ Worker ] Compressed!";

        emit finished();
    });
}

