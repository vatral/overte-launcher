#ifndef S3BUCKETLISTER_H
#define S3BUCKETLISTER_H

#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(S3BucketListerLog);




/**
 * @brief Lists files in a S3 bucket.
 *
 * This class is made for the purposes of the launcher only, to avoid having an external dependency.
 */

class S3BucketLister : public QObject
{
    Q_OBJECT
public:
    class FileData {
    public:

        QUrl baseUrl;
        QString fileName;
        QDateTime lastModified;
        qint64 size;

        QUrl getFullUrl() const {
            QUrl url = baseUrl;
            url.setPath(url.path() + "/" + fileName);
            return url;
        }
    private:
        explicit FileData() {};
        friend class S3BucketLister;

    };

    explicit S3BucketLister(QObject *parent = nullptr);

    /**
     * @brief List the files in the bucket
     * @param address Bucket URL
     *
     * Emits filesFound for each API response (may be multiple times), listingDone at the end.
     */
    void listBucket(const QUrl &address);

private slots:
    void networkRequestFinished();

signals:
    void filesFound(const QList<FileData> &files);
    void listingDone();
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager _networkManager;
};

inline QDebug& operator<<(QDebug &debug, const S3BucketLister::FileData &data) {
    debug.nospace() << "FileData(base=" << data.baseUrl
                    << ", fileName=" << data.fileName
                    << ", lastModified=" << data.lastModified.toString(Qt::ISODate)
                    << ", size=" << data.size
                    << ")";

    return debug.space();
}
#endif // S3BUCKETLISTER_H

