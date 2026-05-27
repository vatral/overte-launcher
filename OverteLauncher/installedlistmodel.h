#ifndef INSTALLEDLISTMODEL_H
#define INSTALLEDLISTMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include <QObject>

#include "installedversion.h"


class InstalledListModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum Column {
        Description = 0,
        Date = 1,
        Size = 2,
        ColumnCount = 3
    };

    enum Role {
        SortRole = Qt::UserRole + 1,
    };

    void setEntries(const QList<InstalledVersion> &versions) {
        beginResetModel();
        _installedVersions = versions;
        endResetModel();
    }

    const QList<InstalledVersion>& getEntries() const {
        return _installedVersions;
    }

    int rowCount([[maybe_unused]]const QModelIndex &parent = QModelIndex()) const override {
        return _installedVersions.size();
    }

    int columnCount([[maybe_unused]]const QModelIndex &parent = QModelIndex()) const override {
        return Column::ColumnCount;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags  flags(const QModelIndex &index) const override {
        if (!index.isValid()) {
            return {};
        }

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    explicit InstalledListModel(QObject *parent = nullptr);
private:
    QString humanReadableSize(qint64 size) const;

    QList<InstalledVersion> _installedVersions;
};


class InstalledListProxyModel : public QSortFilterProxyModel {
public:
    explicit InstalledListProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
    }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};




#endif // INSTALLEDLISTMODEL_H
