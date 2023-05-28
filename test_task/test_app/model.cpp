#include "model.h"

#include <QQmlEngine>
#include <QFileInfo>

#include <vector>
#include <map>

struct MainModel::impl_t
{
    struct ItemData
    {
        QString filename;
        int width;
        int height;
    };

    QList<ItemData> m_data;
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

    const auto& itemData = impl().m_data[index.row()];

    switch(role) {
    case Qt::UserRole + 1:
        return QVariant(itemData.filename);
        break;
    case Qt::UserRole + 2:
        return QVariant(itemData.width);
        break;
    case Qt::UserRole + 3:
        return QVariant(itemData.height);
        break;
    default:
        break;
    }

    qDebug() << "[ MainModel ] Not supported role request.";
    return { };
}

QHash<int, QByteArray> MainModel::roleNames() const
{
    return {
        { Qt::UserRole + 1, "filename" },
        { Qt::UserRole + 2, "width" },
        { Qt::UserRole + 3, "height" }
    };
}

void MainModel::addItem(const QString& item, int width, int height)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    impl().m_data.append({ item, width, height });

    endInsertRows();

    QFileInfo fileInfo(item);
    qDebug() << "[ MainModel ] Item added: " << fileInfo.fileName() << " widht: " << width << " height:" << height;
}
