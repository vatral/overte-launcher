#ifndef DOWNLOADLISTDIALOG_H
#define DOWNLOADLISTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include "s3bucketfilter.h"
#include "s3bucketlister.h"

class DownloadListModel : public QAbstractTableModel {
public:
    enum Column {
        Description = 0,
        Date = 1,
        Size = 2,
        ColumnCount = 3
    };

    enum Role {
        SortRole = Qt::UserRole + 1,
        RawRole = Qt::UserRole + 2
    };

    void setEntries(const QList<S3BucketLister::FileData> &files) {
        beginResetModel();
        _files = files;
        endResetModel();
    }

    const QList<S3BucketLister::FileData>& getEntries() const {
        return _files;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return _files.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
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

private:
    QString humanReadableSize(qint64 size) const;

    QList<S3BucketLister::FileData> _files;
};


class DownloadSortProxyModel : public QSortFilterProxyModel {
public:
    explicit DownloadSortProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
    }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};




namespace Ui {
class DownloadListDialog;
}

class DownloadListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadListDialog(QWidget *parent = nullptr);

    std::optional<S3BucketLister::FileData> selectedFile() const;

    ~DownloadListDialog();
public slots:
    void bucketFilesFound(const QList<S3BucketLister::FileData> &files);
    void bucketListingDone();
    void bucketErrorOccurred(const QString &error);
    void downloadSelectionChanged();


private:
    Ui::DownloadListDialog *ui;
    S3BucketLister _bucketLister;
    QList<S3BucketLister::FileData> _files; // All files found, which apply to our platform
    QList<S3BucketLister::FileData> _filteredFiles; // The subset of files the user chose to see with the combobox
    DownloadListModel _model;
    DownloadSortProxyModel _sortModel;
    S3BucketFilter _filter;
};

#endif // DOWNLOADLISTDIALOG_H
