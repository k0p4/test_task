#pragma once

#include <QAbstractTableModel>

#include "utils/pimpl.h"

class MainModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static void registerTypes();

    MainModel();
    ~MainModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;



public slots:
    void addItem(const QString& item, int width, int height);
    void setActive(const QString& item, bool active);

private:
    DECLARE_PIMPL_EX(MainModel);
};
