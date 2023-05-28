#include "appcontroller.h"

#include <QQmlEngine>

#include "model.h"

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
    return true;
}

void AppController::runCompress(const QString& fileName)
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
