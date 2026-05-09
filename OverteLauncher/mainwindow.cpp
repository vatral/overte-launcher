#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadlistdialog.h"
#include "downloadprogressdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _sortModel.setSourceModel(&_model);

    ui->installedVersions->setModel(&_sortModel);
    ui->installedVersions->setRootIsDecorated(false);
    ui->installedVersions->setAlternatingRowColors(true);
    ui->installedVersions->setUniformRowHeights(true);

    ui->installedVersions->setSortingEnabled(true);
    ui->installedVersions->sortByColumn(DownloadListModel::Date, Qt::DescendingOrder);

    ui->installedVersions->header()->setStretchLastSection(false);
    ui->installedVersions->header()->setSectionResizeMode(DownloadListModel::Description, QHeaderView::Stretch);
    ui->installedVersions->header()->setSectionResizeMode(DownloadListModel::Date, QHeaderView::ResizeToContents);
    ui->installedVersions->header()->setSectionResizeMode(DownloadListModel::Size, QHeaderView::ResizeToContents);

    emit updateInstalledVersions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDownloadListDialog()
{
    auto dlg = new DownloadListDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(true);

    connect(dlg, &QDialog::finished, this, [this, dlg](int result) {
        auto selectedFile = dlg->selectedFile();

        QDir downloadDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        if (!downloadDir.exists("downloads")) {
            downloadDir.mkpath("downloads");
        }
        downloadDir.cd("downloads");

        QString path = downloadDir.absoluteFilePath(QFileInfo(selectedFile.fileName).fileName());

        qInfo() << "Selected file   : " << selectedFile;
        qInfo() << "Downloading into: " << path;


        auto dlg = new DownloadProgressDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setModal(true);
        dlg->show();
        dlg->startDownload(selectedFile, path);

    });

    dlg->show();
}

void MainWindow::updateInstalledVersions()
{
    _installedVersions = InstalledVersion::findInstalledVersions();
    _model.setEntries(_installedVersions);
}
