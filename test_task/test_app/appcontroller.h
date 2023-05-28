#pragma once

#include <QObject>

#include <utils-cpp/pimpl.h>
#include <utils-cpp/singleton.h>

#define DECLARE_PIMPL_EX(classname) \
classname(const classname &) = delete; \
    classname &operator=(const classname &) = delete; \
    DECLARE_PIMPL

class AppController : public QObject, public Singleton<AppController>
{
    Q_OBJECT
public:
    static void registerType();

    AppController();
    ~AppController() override;

    Q_INVOKABLE void runCompress(const QString& fileName);

signals:
    void done(const QString& err);

private:
    DECLARE_PIMPL_EX(AppController);
};
