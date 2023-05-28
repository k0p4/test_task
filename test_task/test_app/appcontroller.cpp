#include "appcontroller.h"

#include <QQmlEngine>

#include <archivator.h>

#include <alog/all.h>
DEFINE_ALOGGER_MODULE_NS(AppController);

struct AppController::impl_t
{

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

void AppController::runCompress(const QString& fileName)
{
    LOGD << "Run compression for " << fileName;
    Archivator::RawImageData data;
    Archivator::compressImage(data);
}
