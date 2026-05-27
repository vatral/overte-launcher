#include <QDateTime>
#include <QFileInfo>
#include <QPushButton>

#include "downloadlistdialog.h"
#include "ui_downloadlistdialog.h"
#include "s3bucketfilter.h"
#include "constants.h"
#include "enums.h"




QVariant DownloadListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= _files.size()) {
        return {};
    }

    const auto &file = _files.at(index.row());

    if (role == Qt::DisplayRole) {
        QFileInfo fi(file.fileName);

        switch (index.column()) {
        case Column::Description:
            return fi.completeBaseName();
        case Column::Date:
            return file.lastModified.toString(Qt::TextDate);
        case Column::Size:
            return humanReadableSize(file.size);
        default:
            return {};
        }
    } else if (role == Role::SortRole || role == Role::RawRole) {
        // RawRole is for retriveing the underlying exact data,
        // currently same thing as SortRole.
        switch (index.column()) {
        case Column::Description:
            return file.fileName;
        case Column::Date:
            return file.lastModified;
        case Column::Size:
            return file.size;
        default:
            return {};
        }
    }

    return {};
}

QVariant DownloadListModel::headerData(int section, Qt::Orientation orientation, int role) const {
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


QString DownloadListModel::humanReadableSize(qint64 size) const {
    static const char *suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    int suffixIndex = 0;
    double humanSize = size;

    while (humanSize >= 1024 && suffixIndex < 4) {
        humanSize /= 1024;
        suffixIndex++;
    }

    return QString::number(humanSize, 'f', 2) + " " + suffixes[suffixIndex];
}


bool DownloadSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto leftData = sourceModel()->data(left, DownloadListModel::Role::SortRole);
    auto rightData = sourceModel()->data(right, DownloadListModel::Role::SortRole);

    switch(left.column()) {
    case DownloadListModel::Column::Description:
        return QString::localeAwareCompare(leftData.toString(), rightData.toString());
        break;
    case DownloadListModel::Column::Date:
        return leftData.toDateTime() < rightData.toDateTime();
        break;
    case DownloadListModel::Column::Size:
        return leftData.toLongLong() < rightData.toLongLong();
        break;
    }

    return false;
}



DownloadListDialog::DownloadListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DownloadListDialog)
{
    ui->setupUi(this);

    connect(&_bucketLister, &S3BucketLister::errorOccurred, this, &DownloadListDialog::bucketErrorOccurred);
    connect(&_bucketLister, &S3BucketLister::listingDone, this, &DownloadListDialog::bucketListingDone);
    connect(&_bucketLister, &S3BucketLister::filesFound, this, &DownloadListDialog::bucketFilesFound);


    ui->typesCombo->addItem("Release", static_cast<int>(ReleaseType::Release));
    ui->typesCombo->addItem("Release candidate", static_cast<int>(ReleaseType::ReleaseCandidate));
    ui->typesCombo->addItem("PR", static_cast<int>(ReleaseType::PR));
    ui->typesCombo->addItem("Master build", static_cast<int>(ReleaseType::MasterBuild));
    ui->typesCombo->setCurrentIndex(0);

    _sortModel.setSourceModel(&_model);

    ui->downloadsTree->setModel(&_sortModel);
    ui->downloadsTree->setRootIsDecorated(false);
    ui->downloadsTree->setAlternatingRowColors(true);
    ui->downloadsTree->setUniformRowHeights(true);

    ui->downloadsTree->setSortingEnabled(true);
    ui->downloadsTree->sortByColumn(DownloadListModel::Date, Qt::DescendingOrder);

    ui->downloadsTree->header()->setStretchLastSection(false);
    ui->downloadsTree->header()->setSectionResizeMode(DownloadListModel::Description, QHeaderView::Stretch);
    ui->downloadsTree->header()->setSectionResizeMode(DownloadListModel::Date, QHeaderView::ResizeToContents);
    ui->downloadsTree->header()->setSectionResizeMode(DownloadListModel::Size, QHeaderView::ResizeToContents);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // This signal must be connected after setModel()
    connect(ui->downloadsTree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DownloadListDialog::downloadSelectionChanged);

    _files.clear();
    _bucketLister.listBucket(BUCKET_URL);

}

std::optional<S3BucketLister::FileData> DownloadListDialog::selectedFile() const
{

    auto *proxy = qobject_cast<QSortFilterProxyModel*>(ui->downloadsTree->model());

    if (!ui->downloadsTree->selectionModel()->selectedIndexes().isEmpty()) {
        auto first = ui->downloadsTree->selectionModel()->selectedIndexes().first();
        qInfo() << "First selected item:" << first;

        auto source_first = proxy->mapToSource(first);
        qInfo() << "Mapped to source:" << source_first;

        if (source_first.isValid()) {
            auto file = _filteredFiles.at(source_first.row());
            qInfo() << "Selected file:" << file;
            return file;
        }

    }



    return {};
}

DownloadListDialog::~DownloadListDialog()
{
    delete ui;
}

void DownloadListDialog::bucketFilesFound(const QList<S3BucketLister::FileData> &files)
{
    _files.append(_filter.filter(files));
}

void DownloadListDialog::bucketListingDone()
{
    _filteredFiles = _filter.filter(_files, static_cast<ReleaseType>(ui->typesCombo->currentData().toInt()));
    _model.setEntries(_filteredFiles);

    auto sel = ui->downloadsTree->selectionModel();
    if (sel) {
        sel->clearSelection();

        auto first = _model.index(0,0);
        if (first.isValid()) {
            sel->setCurrentIndex(first, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            ui->downloadsTree->scrollTo(first);
        }
    }

}

void DownloadListDialog::bucketErrorOccurred(const QString &error)
{
    qWarning() << "Bucket error:" << error;
}

void DownloadListDialog::downloadSelectionChanged()
{
    auto ok_button = ui->buttonBox->button(QDialogButtonBox::Ok);
    qInfo() << "Selection changed";

    if (ui->downloadsTree->selectionModel()->selectedIndexes().isEmpty()) {
        ok_button->setEnabled(false);
    } else {
        ok_button->setEnabled(true);
    }
}
