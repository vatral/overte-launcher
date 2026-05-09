#include "installedlistmodel.h"

InstalledListModel::InstalledListModel(QObject *parent)
    : QAbstractTableModel{parent}
{}


QVariant InstalledListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= _installedVersions.size()) {
        return {};
    }

    const auto &inst = _installedVersions.at(index.row());

    if (role == Qt::DisplayRole) {

        switch (index.column()) {
        case Column::Description:
            return inst.getDescription();
        case Column::Date:
            return inst.getDate().toString(Qt::TextDate);
        case Column::Size:
            return humanReadableSize(inst.getSize());
        default:
            return {};
        }
    } else if (role == Role::SortRole) {
        // RawRole is for retriveing the underlying exact data,
        // currently same thing as SortRole.
        switch (index.column()) {
        case Column::Description:
            return inst.getDescription();
        case Column::Date:
            return inst.getDate();
        case Column::Size:
            return inst.getSize();
        default:
            return {};
        }
    }

    return {};
}


QVariant InstalledListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ( orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case Column::Description:
        return "Description";
    case Column::Date:
        return "Date";
    case Column::Size:
        return "Size";
    default:
        return {};
    }
}

QString InstalledListModel::humanReadableSize(qint64 size) const {
    static const char *suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    int suffixIndex = 0;
    double humanSize = size;

    while (humanSize >= 1024 && suffixIndex < 4) {
        humanSize /= 1024;
        suffixIndex++;
    }

    return QString::number(humanSize, 'f', 2) + " " + suffixes[suffixIndex];
}


bool InstalledListProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto leftData = sourceModel()->data(left, InstalledListModel::Role::SortRole);
    auto rightData = sourceModel()->data(right, InstalledListModel::Role::SortRole);

    switch(left.column()) {
    case InstalledListModel::Column::Description:
        return QString::localeAwareCompare(leftData.toString(), rightData.toString());
        break;
    case InstalledListModel::Column::Date:
        return leftData.toDateTime() < rightData.toDateTime();
        break;
    case InstalledListModel::Column::Size:
        return leftData.toLongLong() < rightData.toLongLong();
        break;
    }

    return false;
}
