#include "model.h"

#include <QQmlEngine>

#include <vector>
#include <map>

struct MainModel::impl_t
{
    QList<QString> m_data;
};

void MainModel::registerTypes()
{
    qmlRegisterUncreatableType<MainModel>("MainModel", 1, 0, "MainModel", "MainModel is uncreatable type");
}

MainModel::MainModel()
{
    createImpl();
}

MainModel::~MainModel()
{

}

int MainModel::rowCount(const QModelIndex& parent) const
{
    return impl().m_data.size();
}

QVariant MainModel::data(const QModelIndex& index, int role) const
{
    if ((index.row() < 0) || (index.row() >= impl().m_data.size())) {
        qDebug() << "[ MainModel ] invalid QModelIndex";
        return { };
    }

    if (role == Qt::UserRole + 1) {
        return QVariant(impl().m_data[index.row()]);
    }

    qDebug() << "[ MainModel ] Not supported role request.";
    return { };
}

QHash<int, QByteArray> MainModel::roleNames() const
{
    return { { Qt::UserRole + 1, "filename" } };
}

void MainModel::addItem(const QString& item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    impl().m_data.push_back(item);
    endInsertRows();
}
