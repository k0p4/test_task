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
    QString m_searchPath;
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
    impl().m_searchPath = path;
    if (path.isEmpty() || !QDir::isAbsolutePath(path)) {        
        if (QUrl url { path }; url.isValid()) {
            impl().m_searchPath = url.path();
        } else {
            impl().m_searchPath = QDir::currentPath();
        }
    }

    qDebug() << "[ AppController ] Setting search path to: " << impl().m_searchPath;

    QStringList filters;
    filters << "*.bmp" << "*.png" << "*.barch";

    QDir directory(impl().m_searchPath);
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

void AppController::process(const QString& fileName)
{
    qDebug() << "[ AppController ] Run compression for " << fileName;

    Worker* worker = new Worker;
    QObject::connect(worker, &Worker::finished, this, [worker, fileName, this] () {
        impl().m_model.setActive(fileName, false);
        worker->deleteLater();

        setSearchPath(impl().m_searchPath);
    }, Qt::QueuedConnection);

    QObject::connect(worker, &Worker::error, this, [worker, fileName, this] (const QString& err) {
        impl().m_model.setActive(fileName, false);
        worker->deleteLater();

        emit error(err);
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
