//! Qt includes
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

//! Project includes
#include "appcontroller.h"
#include "model.h"

namespace Internal {

void registerTypes()
{
    AppController::registerType();
    MainModel::registerTypes();
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
    int ret { 0 };
    try
    {
        AppController controller;

        Internal::registerTypes();

        QGuiApplication app(argc, argv);
        QQmlApplicationEngine engine;

        Internal::loadMainWindow(engine, app);

        qDebug() << "[ Eventloop ] Starting app!";

        QString arg(argv[1]);
        controller.setSearchPath(arg);

        ret = app.exec();

        qDebug() << "[ Eventloop ] Finalizing... ";
    } catch (...) {
        qWarning() << "[ Eventloop ] Unhandled exception caught. Finalizing.";
        return -1;
    }

    qWarning() << "[ Eventloop ] ended with " << ret;

    return ret;
}
