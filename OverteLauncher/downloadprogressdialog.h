#ifndef DOWNLOADPROGRESSDIALOG_H
#define DOWNLOADPROGRESSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QFile>

#include "s3bucketlister.h"
Q_DECLARE_LOGGING_CATEGORY(DownloadProgressDialogLog);


namespace Ui {
class DownloadProgressDialog;
}

class DownloadProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadProgressDialog(QWidget *parent = nullptr);
    void startDownload(const S3BucketLister::FileData &data, const QString &filePath);
    ~DownloadProgressDialog();

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadErrorOccurred(QNetworkReply::NetworkError code);
    void downloadFinished();
    void downloadSslErrors(const QList<QSslError> &errors);

private:
    Ui::DownloadProgressDialog *ui;
    QNetworkAccessManager _networkManager;
    QFile _outputFile;
    QString _filename;
    std::optional<S3BucketLister::FileData> _data;
};

#endif // DOWNLOADPROGRESSDIALOG_H
