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

    Q_INVOKABLE bool move(int srcIdx, int destIdx);

public slots:
    void addItem(const QString& item);

private:
    DECLARE_PIMPL_EX(MainModel);
};
