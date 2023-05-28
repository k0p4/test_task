#include "appcontroller.h"

#include <QQmlEngine>
#include <QDir>
#include <QImage>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QImageWriter>

#include <archivator.h>

QDataStream& operator<<(QDataStream& out, const std::vector<unsigned char>& vec)
{
    out << static_cast<quint32>(vec.size());

    for (const auto& item : vec) {
        out << item;
    }

    return out;
}

QDataStream& operator>>(QDataStream& in, std::vector<unsigned char>& vec)
{
    quint32 size;
    in >> size;

    vec.resize(size);

    for (auto& item : vec) {
        in >> item;
    }

    return in;
}

namespace Internal {

[[nodiscard]] bool parseBarchFileHeader(const QString& fileName, int& width, int& height)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ AppController ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream in(&file);

    QString id;
    in >> id;

    if (id != "BA") {
        qDebug() << "[ AppController ] Seems like incorrect format for " << fileName;
        return false;
    }

    in >> width;
    in >> height;

    file.close();

    return true;
}

[[nodiscard]] bool parseBarchFile(const QString& fileName, Archivator::RawImageData& data)
{
    //! AK: duplicate code here is ok, cuz if call parse header first there will be additional file and datastream allocations.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ AppController ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream in(&file);

    QString id;
    in >> id;

    if (id != "BA") {
        qDebug() << "[ AppController ] Seems like incorrect format for " << fileName;
        return false;
    }

    in >> data.width;
    in >> data.height;
    in >> data.data;

    file.close();

    return true;
}

[[nodiscard]] bool saveBarchFile(const QString& fileName, const Archivator::RawImageData& data, const int compressedSize)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[ AppController ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream out(&file);

    QString id = "BA";
    out << id;
    out << data.width;
    out << data.height;
    out << data.data;

    file.close();

    return true;
}

} // Internal

struct AppController::impl_t
{
    int m_active { -1 };
    MainModel m_model;
};


void AppController::registerType()
{
    qmlRegisterSingletonType<AppController>("AppController", 1, 0, "AppController", [] (QQmlEngine* engine, QJSEngine*) -> QObject* {
        auto ret = AppController::instance();
        engine->setObjectOwnership(ret, QQmlEngine::CppOwnership);
        return ret;
    });
}

AppController::AppController()
{
    createImpl();
}

AppController::~AppController()
{

}

bool AppController::setSearchPath(const QString& path)
{
    QString searchPath = path;
    if (path.isEmpty() || !QDir::isAbsolutePath(path)) {
        QUrl url { path };
        if (!url.isValid()) {
            searchPath = QDir::currentPath();
        } else {
            searchPath = url.path();
        }
    }

    qDebug() << "[ AppController ] Setting search path to: " << searchPath;

    QStringList filters;
    filters << "*.bmp" << "*.png" << "*.barch";

    QDir directory(searchPath);
    QStringList files = directory.entryList(filters, QDir::Files);

    qDebug() << "[ AppController ] Filtered files in directory: " << files.size();

    for (const QString& file : files) {
        int width { 0 };
        int height { 0 };

        if (file.endsWith(".bmp") || file.endsWith("png")) {
            QImage image(file);

            if (image.isNull()) {
                qDebug() << "[ AppController ] Failed to load the image: " << file;
                continue;
            }

            width = image.width();
            height = image.height();
        } else if (file.endsWith(".barch")) {
            if (!Internal::parseBarchFileHeader(file, width, height)) {
                qDebug() << "[ AppController ] Failed to load the image: " << file;
                continue;
            }
        } else {
            qDebug() << "[ AppController ] Unknown file format: " << file;
            continue;
        }

        impl().m_model.addItem(file, width, height);
    }

    return true;
}

//! TODO: error notification

void AppController::process(const QString& fileName)
{
    qDebug() << "[ AppController ] Run compression for " << fileName;

    if (fileName.endsWith(".barch")) {
        Archivator::RawImageData data;

        if (!Internal::parseBarchFile(fileName, data)) {
            qDebug() << "[ AppController ] Failed to load the image: " << fileName;
        }

        auto decompressed = Archivator::decompressImage(data);

        QImage image(decompressed.data.data(), decompressed.width, decompressed.height, QImage::Format_Grayscale8);

        QImageWriter writer("decompressed.png", "PNG");
        writer.write(image);

        //! TODO: test call
        setSearchPath("");
        return;
    }

    QImage image(fileName);

    if (image.isNull()) {
        qDebug() << "[ AppController ] Failed to load the image: " << fileName;
        return;
    }

    Archivator::RawImageData data;

    data.width = image.width();
    data.height = image.height();

    data.data = std::vector<unsigned char>(image.bits(), image.bits() + image.sizeInBytes());

    int compressedSize = 0;
    auto compressed = Archivator::compressImage(data);

    if (!Internal::saveBarchFile("compressed.barch", data, compressedSize)) {
        qDebug() << "Could not save compressed data!";
    }

    //! TODO: test call
    setSearchPath("");
}

int AppController::active() const
{
    return impl().m_active;
}

MainModel* AppController::mainModel()
{
    return &impl().m_model;
}
