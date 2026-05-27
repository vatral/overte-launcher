#include "downloadprogressdialog.h"
#include "ui_downloadprogressdialog.h"

#include <QNetworkReply>
#include <QMessageBox>

Q_LOGGING_CATEGORY(DownloadProgressDialogLog, "DownloadProgressDialog");


DownloadProgressDialog::DownloadProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DownloadProgressDialog)
{
    ui->setupUi(this);
}

void DownloadProgressDialog::startDownload(const S3BucketLister::FileData &fileData, const QString &filePath)
{
    QNetworkRequest req(fileData.getFullUrl());

    qCInfo(DownloadProgressDialogLog) << "Loading from" << fileData.getFullUrl() << "; expecting" << fileData.size << "bytes";


    QNetworkReply *reply = _networkManager.get(req);

    QObject::connect(reply, &QNetworkReply::downloadProgress, this,  &DownloadProgressDialog::downloadProgress);
    QObject::connect(reply, &QNetworkReply::errorOccurred, this,  &DownloadProgressDialog::downloadErrorOccurred);
    QObject::connect(reply, &QNetworkReply::finished, this,  &DownloadProgressDialog::downloadFinished);
    QObject::connect(reply, &QNetworkReply::sslErrors, this,  &DownloadProgressDialog::downloadSslErrors);

    _filename = filePath;
    _outputFile.setFileName(filePath + ".part");
    if (!_outputFile.open(QIODevice::WriteOnly, QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
        _data = fileData;
        qCCritical(DownloadProgressDialogLog) << "Failed to open output file" << filePath << ":" << _outputFile.errorString();
        QMessageBox::critical(this, "Error", "Failed to open output file: " + _outputFile.errorString());
        close();
        return;


    }

}

DownloadProgressDialog::~DownloadProgressDialog()
{
    delete ui;
}

void DownloadProgressDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    _outputFile.write(static_cast<QNetworkReply*>(sender())->readAll());
     if (bytesTotal > 0) {
         ui->progressBar->setMaximum(bytesTotal);
         ui->progressBar->setValue(bytesReceived);
     }
}

void DownloadProgressDialog::downloadErrorOccurred(QNetworkReply::NetworkError code)
{
    _outputFile.close();
    _outputFile.remove();

    QMessageBox msg(this);
    msg.setText("An error occurred during the download: " + QString::number(code));
    msg.exec();
    close();
}

void DownloadProgressDialog::downloadFinished()
{

    // We set the timestamp on the file to the same it had at the download
    // archive. We later show this to the user in the versions list.
    _outputFile.setFileTime(_data->lastModified, QFileDevice::FileBirthTime);
    _outputFile.setFileTime(_data->lastModified, QFileDevice::FileModificationTime);
    _outputFile.close();

    QFile f(_filename);
    if (f.exists()) {
        if (!f.remove()) {
            qCCritical(DownloadProgressDialogLog) << "Failed to remove existing file" << _filename << ":" << f.errorString();
            QMessageBox::critical(this, "Error", "Failed to remove existing file: " + f.errorString());
            close();
            return;
        }
    }

    if (!_outputFile.rename(_filename)) {
        qCCritical(DownloadProgressDialogLog) << "Failed to rename file to" << _filename << ":" << f.errorString();
        QMessageBox::critical(this, "Error", "Failed to rename file to" + _filename + ": " + f.errorString());
        close();
        return;
    }

    auto reply = static_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        qCCritical(DownloadProgressDialogLog) << "Download finished with error:" << reply->errorString();
        QMessageBox msg(this);
        msg.setText("An error occurred during the download: " + reply->errorString());
        msg.exec();
        close();
        return;
    }




    qCInfo(S3BucketListerLog) << "Download finished successfully";
    accept();
}

void DownloadProgressDialog::downloadSslErrors(const QList<QSslError> &errors)
{
    QMessageBox msg(this);
    QString errstr;

    for(const auto &err : errors) {
        errstr += err.errorString() + "\n";
    }

    qCCritical(DownloadProgressDialogLog) << "SSL errors during the download" << errstr;
    msg.setText("SSL errors occurred during the download:" + errstr);
    msg.exec();

    close();
}
