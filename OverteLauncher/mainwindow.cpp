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
