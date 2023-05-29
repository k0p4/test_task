#include "appcontroller.h"

#include <QQmlEngine>
#include <QDir>
#include <QImage>

#include "utils/barchutils.h"
#include "worker.h"

#include <archivator.h>

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
        if (QUrl url { path }; url.isValid()) {
            searchPath = url.path();
        } else {
            searchPath = QDir::currentPath();
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

    Worker* worker = new Worker;
    QObject::connect(worker, &Worker::finished, this, [worker, fileName, this] () {
        impl().m_model.setActive(fileName, false);
        worker->deleteLater();
    }, Qt::QueuedConnection);

    QObject::connect(worker, &Worker::error, this, [worker, fileName, this] (QString err) {
        impl().m_model.setActive(fileName, false);
        worker->deleteLater();
    }, Qt::QueuedConnection);

    impl().m_model.setActive(fileName, true);

    worker->process(fileName);
}

int AppController::active() const
{
    return impl().m_active;
}

MainModel* AppController::mainModel()
{
    return &impl().m_model;
}
