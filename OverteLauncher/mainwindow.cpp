#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadlistdialog.h"
#include "downloadprogressdialog.h"
#include "constants.h"

Q_LOGGING_CATEGORY(MainWindowLog, "MainWindow")

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

    //ui->changelogWebView->setUrl(CHANGELOG_URL);

    loadChangelog();
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

        QString path = downloadDir.absoluteFilePath(QFileInfo(selectedFile->fileName).fileName());

        qInfo() << "Selected file   : " << selectedFile;
        qInfo() << "Downloading into: " << path;


        auto dlg = new DownloadProgressDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setModal(true);
        dlg->show();
        dlg->startDownload(selectedFile.value(), path);

    });

    dlg->show();
}

void MainWindow::updateInstalledVersions()
{
    _installedVersions = InstalledVersion::findInstalledVersions();
    _model.setEntries(_installedVersions);
}

void MainWindow::launchSelected()
{
    auto current = getSelectedVersion();
    if (!current) {
        return;
    }

    _runner.runProgram(current->getFullBinaryPath(), QStringList(), false);
}

void MainWindow::reportBug()
{

}

void MainWindow::goToCommunity()
{

}

void MainWindow::loadChangelog()
{
    QNetworkRequest req(CHANGELOG_URL);

    qCInfo(MainWindowLog) << "Loading from" << req.url();

    QNetworkReply *reply = _networkManager.get(req);


    QObject::connect(reply, &QNetworkReply::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal) {
        qCInfo(MainWindowLog) << "Downloading" << bytesReceived << "of" << bytesTotal;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, [this](QNetworkReply::NetworkError code){
        qCInfo(MainWindowLog) << "Error" << code;
    });

    QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {
        qCInfo(MainWindowLog) << "Download finished";
        auto text = reply->readAll();

        ui->changelogTextBrowser->setMarkdown(text);

    });


    QObject::connect(reply, &QNetworkReply::sslErrors, [this](const QList<QSslError> &errors) {
        qCInfo(MainWindowLog) << "SSL Error" << errors;
    });
}

std::optional<InstalledVersion> MainWindow::getSelectedVersion() const
{

    auto proxy_idx = ui->installedVersions->currentIndex();
    auto *proxy = qobject_cast<QSortFilterProxyModel*>(ui->installedVersions->model());

    if (!ui->installedVersions->selectionModel()->selectedIndexes().isEmpty()) {
        auto first = ui->installedVersions->selectionModel()->selectedIndexes().first();
        qInfo() << "First selected item:" << first;

        auto source_first = proxy->mapToSource(first);
        qInfo() << "Mapped to source:" << source_first;

        if (source_first.isValid()) {
            auto ver = _installedVersions.at(source_first.row());
            qInfo() << "Selected file:" << ver;
            return ver;
        }

    }

    return {};
}
