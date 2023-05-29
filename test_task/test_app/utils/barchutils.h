#pragma once

#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <archivator.h>

inline QDataStream& operator<<(QDataStream& out, const std::vector<unsigned char>& vec)
{
    out << static_cast<quint32>(vec.size());

    for (const auto& item : vec) {
        out << item;
    }

    return out;
}

inline QDataStream& operator>>(QDataStream& in, std::vector<unsigned char>& vec)
{
    quint32 size;
    in >> size;

    vec.resize(size);

    for (auto& item : vec) {
        in >> item;
    }

    return in;
}

namespace Internal {

[[nodiscard]] inline bool parseBarchFileHeader(const QString& fileName, int& width, int& height)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ BarchUtils ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream in(&file);

    QString id;
    in >> id;

    if (id != "BA") {
        qDebug() << "[ BarchUtils ] Seems like incorrect format for " << fileName;
        return false;
    }

    in >> width;
    in >> height;

    file.close();

    return true;
}

[[nodiscard]] inline bool parseBarchFile(const QString& fileName, Archivator::RawImageData& data, std::vector<unsigned char>& indexes)
{
    //! AK: duplicate code here is ok, cuz if call parse header first there will be additional file and datastream allocations.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ BarchUtils ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream in(&file);

    QString id;
    in >> id;

    if (id != "BA") {
        qDebug() << "[ BarchUtils ] Seems like incorrect format for " << fileName;
        return false;
    }

    in >> data.width;
    in >> data.height;
    in >> indexes;
    in >> data.data;

    file.close();

    return true;
}

[[nodiscard]] inline bool saveBarchFile(const QString& fileName, const Archivator::RawImageData& data, const std::vector<unsigned char>& indexes)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[ BarchUtils ] Failed to open the file: " << fileName;
        return false;
    }

    QDataStream out(&file);

    QString id = "BA";
    out << id;
    out << data.width;
    out << data.height;
    out << indexes;
    out << data.data;

    file.close();

    return true;
}

} // Internal
