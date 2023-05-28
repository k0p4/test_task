//! Qt includes
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

//! Project includes
#include "appcontroller.h"

//! 3rd patry Includes
#include <alog/all.h>
DEFINE_ALOGGER_MODULE_NS(App_EventLoop);

namespace Internal {

void registerTypes()
{
    AppController::registerType();
}

void loadMainWindow(QQmlApplicationEngine& engine, const QGuiApplication& app)
{
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url] (QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);
}

}

int main(int argc, char *argv[])
{
    auto filters = ALog::Filters::Chain::create({
        std::make_shared<ALog::Filters::Always>(true),
    });

    ALog::DefaultLogger logger;
    logger->setMode(ALog::Logger::LoggerMode::AsynchronousSort);
    logger->setAutoflush(true);
    logger->pipeline().filters().set(filters);
    logger.markReady();

    int ret { 0 };
    try
    {
        AppController controller;

        Internal::registerTypes();

        QGuiApplication app(argc, argv);
        QQmlApplicationEngine engine;

        Internal::loadMainWindow(engine, app);

        LOGD << "Starting app!";

        ret = app.exec();

        LOGD << "Finalizing... ";
    } catch (...) {
        LOGE << "Unhandled exception caught. Finalizing.";
        return -1;
    }

    LOGW << "Eventloop ended with " << ret;

    return ret;
}
