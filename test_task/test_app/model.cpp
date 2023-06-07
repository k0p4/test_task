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
        int width { 0 };
        int height { 0 };
        bool active { false };
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
    case Qt::UserRole + 4:
        return QVariant(itemData.active);
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
        { Qt::UserRole + 3, "height" },
        { Qt::UserRole + 4, "active" }
    };
}

void MainModel::clearModel()
{
    beginResetModel();
    impl().m_data.clear();
    endResetModel();
}

void MainModel::addItem(const QString& item, int width, int height)
{
    auto it = std::find_if(impl().m_data.begin(), impl().m_data.end(), [item] (const impl_t::ItemData& data) {
        return data.filename == item;
    });

    if (it != impl().m_data.end()) {
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    impl().m_data.append({ item, width, height, false });

    endInsertRows();

    QFileInfo fileInfo(item);
    qDebug() << "[ MainModel ] Item added: " << fileInfo.fileName() << " widht: " << width << " height:" << height;
}

void MainModel::setActive(const QString& item, bool active)
{
    auto it = std::find_if(impl().m_data.begin(), impl().m_data.end(), [item] (const MainModel::impl_t::ItemData& val) {
        return item == val.filename;
    });

    if (it == impl().m_data.end()) {
        qDebug() << "[ MainModel ] Invalid setActive call!";
        return;
    }

    if (it->active == active) {
        return;
    }

    it->active = active;

    int row = std::distance(impl().m_data.begin(), it);
    QModelIndex index = createIndex(row, 0);
    emit dataChanged(index, index, { Qt::UserRole + 4 });
}
