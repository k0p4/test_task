#pragma once

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

    void process(const QString& fileName, const QString& dir);

signals:
    void finished();
    void error(const QString err);
};
