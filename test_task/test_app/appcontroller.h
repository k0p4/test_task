#pragma once

#include <QObject>

#include "model.h"
#include "utils/pimpl.h"
#include "utils/singletone.h"

class AppController : public QObject, public Singleton<AppController>
{
    Q_OBJECT
public:
    static void registerType();

    Q_PROPERTY(MainModel* mainModel READ mainModel CONSTANT)
    Q_PROPERTY(int active READ active NOTIFY activeChanged)

    AppController();
    ~AppController() override;

    Q_INVOKABLE bool setSearchPath(const QString& path);

    Q_INVOKABLE void process(const QString& fileName);

    //! property support
    int active() const;
    MainModel* mainModel();

signals:
    void activeChanged();
    //! property support

    void error(const QString& err);

private:
    DECLARE_PIMPL_EX(AppController);
};
