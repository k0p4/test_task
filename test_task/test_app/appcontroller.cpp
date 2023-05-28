#include "appcontroller.h"

#include <QQmlEngine>
#include <QDir>
#include <QImage>
#include <QDataStream>
#include <QFile>
#include <QIODevice>


#include <archivator.h>

namespace Internal {

bool parseBarchFile(const QString& fileName, int& width, int& height)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ AppController ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream in(&file);

    QString id;

    in >> id;
    in >> width;
    in >> height;

    file.close();

    if (id != "BA") {
        qDebug() << "[ AppController ] Seems like incorrect format for " << fileName;
        return false;
    }

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
            Internal::parseBarchFile(file, width, height);
        } else {
            qDebug() << "[ AppController ] Unknown file format: " << file;
            continue;
        }

        impl().m_model.addItem(file, width, height);
    }

    return true;
}

void AppController::process(const QString& fileName)
{
    qDebug() << "[ AppController ] Run compression for " << fileName;

    Archivator::RawImageData data;
    Archivator::compressImage(data);
}

int AppController::active() const
{
    return impl().m_active;
}

MainModel* AppController::mainModel()
{
    return &impl().m_model;
}
