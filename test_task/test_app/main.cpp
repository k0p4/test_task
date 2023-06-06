//! Qt includes
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

//! Project includes
#include "appcontroller.h"
#include "model.h"

#include <archivator.h>
#include <ostream>
#include <iostream>

#include <QRandomGenerator>

void test()
{
    Archivator::RawImageData originalImageData;

    originalImageData.width = 9;
    originalImageData.height = 3;

    originalImageData.data = {
        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
    };


    std::vector<unsigned char> indexes;
    auto res = Archivator::compressImage(originalImageData, indexes);

    qDebug() << "------------------";

    auto decomp = Archivator::decompressImage(res, indexes);

    qDebug() << originalImageData.data.size();
    qDebug() << decomp.data.size();
    qDebug() << (decomp.data == originalImageData.data);
    assert(decomp.data == originalImageData.data);

    indexes.clear();
    originalImageData.data.clear();
    res.data.clear();

    originalImageData.width = 11;
    originalImageData.height = 3;

    originalImageData.data = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    qDebug() << "------------------";
    qDebug() << "------------------";
    qDebug() << "------------------";

    res = Archivator::compressImage(originalImageData, indexes);

    decomp = Archivator::decompressImage(res, indexes);

    qDebug() << originalImageData.data.size();
    qDebug() << decomp.data.size();
    qDebug() << (decomp.data == originalImageData.data);
    assert(decomp.data == originalImageData.data);

    indexes.clear();
    originalImageData.data.clear();
    res.data.clear();

    originalImageData.width = 4;
    originalImageData.height = 4;

    originalImageData.data = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    qDebug() << "------------------";
    qDebug() << "------------------";
    qDebug() << "------------------";

    res = Archivator::compressImage(originalImageData, indexes);

    decomp = Archivator::decompressImage(res, indexes);

    qDebug() << originalImageData.data.size();
    qDebug() << decomp.data.size();
    qDebug() << (decomp.data == originalImageData.data);
    assert(decomp.data == originalImageData.data);

    indexes.clear();
    originalImageData.data.clear();
    res.data.clear();

    originalImageData.width = 4;
    originalImageData.height = 4;

    originalImageData.data = {
        0xFF, 0x01, 0xFF, 0xFF,
        0x00, 0x01, 0x00, 0x00,
        0xFF, 0x01, 0xFF, 0xFF,
        0xFF, 0x01, 0xFF, 0xFF
    };

    qDebug() << "------------------";
    qDebug() << "------------------";
    qDebug() << "------------------";

    res = Archivator::compressImage(originalImageData, indexes);

    decomp = Archivator::decompressImage(res, indexes);

    qDebug() << originalImageData.data.size();
    qDebug() << decomp.data.size();
    qDebug() << (decomp.data == originalImageData.data);
    assert(decomp.data == originalImageData.data);

    while (true) {
        indexes.clear();
        originalImageData.data.clear();
        res.data.clear();

        // Generate random width between 4 and 49
        originalImageData.width = QRandomGenerator::global()->bounded(4, 1200);

        // Generate random height between 4 and 49
        originalImageData.height = QRandomGenerator::global()->bounded(4, 1200);

        // Resize the data vector to hold the appropriate number of pixels
        originalImageData.data.resize(originalImageData.width * originalImageData.height);

        // Fill the data vector with random values
        QRandomGenerator randomGenerator;
        for (int i = 0; i < originalImageData.width * originalImageData.height; ++i) {
            originalImageData.data[i] = randomGenerator.bounded(256);  // Generate random value between 0 and 255
        }

        res = Archivator::compressImage(originalImageData, indexes);

        decomp = Archivator::decompressImage(res, indexes);

        //qDebug() << originalImageData.data.size();
        //qDebug() << decomp.data.size();
        qDebug() << (decomp.data == originalImageData.data);
        if (decomp.data != originalImageData.data) {
            std::cout << " ";
            break;
        }
    }
}

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
{/*
    test();
    return 0;
*/
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
