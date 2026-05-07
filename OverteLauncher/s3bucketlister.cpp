#include "s3bucketlister.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QUrlQuery>

Q_LOGGING_CATEGORY(S3BucketListerLog, "s3BucketLister")

S3BucketLister::S3BucketLister(QObject *parent)
    : QObject{parent}
{}

void S3BucketLister::listBucket(const QUrl &url) {
    QNetworkRequest req(url);

    qCInfo(S3BucketListerLog) << "Loading list from" << url;

    QNetworkReply *reply = _networkManager.get(req);
    QObject::connect(reply, &QNetworkReply::finished, this,  &S3BucketLister::networkRequestFinished);
}

void S3BucketLister::networkRequestFinished()
{
    auto reply = dynamic_cast<QNetworkReply*>(sender());
    assert(reply);

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        return;
    }

    qCInfo(S3BucketListerLog) << "Received response from" << reply->request().url();

    auto data = reply->readAll();

    QDomDocument doc("reply");
    auto parseResult = doc.setContent(data);

    if (!parseResult) {
        qCCritical(S3BucketListerLog) << "Failed to parse XML" << parseResult.errorMessage;
        emit errorOccurred("Failed to parse XML");
        return;
    }

    QList<FileData> files;
    auto root = doc.documentElement();
    if (root.tagName() != "ListBucketResult") {
        qCCritical(S3BucketListerLog) << "Failed to find ListBucketResult in XML:" << data;
        emit errorOccurred("No ListBucketResult found in XML");
        return;
    }

    bool isTruncated = QVariant(root.toElement().firstChildElement("IsTruncated").text()).toBool();

    auto contentsList = root.elementsByTagName("Contents");

    for(const auto &content : contentsList) {
        auto contentElement = content.toElement();

        QString storageClass = contentElement.firstChildElement("StorageClass").text();

        if (storageClass.contains("ARCHIVE") || storageClass.contains("GLACIER")) {
            // We mostly don't care about storage class, but we don't want
            // to retrieve any archived data here. Could be expensive.
            continue;
        }

        FileData fileData;
        fileData.baseUrl = reply->request().url();
        fileData.baseUrl.setQuery(QUrlQuery()); // Remove any parameters we use for paging
        fileData.fileName = contentElement.firstChildElement("Key").text();
        fileData.lastModified = QDateTime::fromString(contentElement.firstChildElement("LastModified").text(), Qt::ISODate);
        fileData.size = contentElement.firstChildElement("Size").text().toLongLong();
        files.append(fileData);
    }

    qCInfo(S3BucketListerLog) << "Found" << files.size() << "files";

    emit filesFound(files);

    reply->deleteLater();

    if (isTruncated) {
        qCInfo(S3BucketListerLog) << "Results are truncated, loading next batch with marker" << root.toElement().firstChildElement("NextMarker").text();

        QUrl url = reply->request().url();
        QUrlQuery query(url);
        query.addQueryItem("marker", root.toElement().firstChildElement("NextMarker").text());
        url.setQuery(query);
        QNetworkRequest req(url);

        QNetworkReply *reply = _networkManager.get(req);
        QObject::connect(reply, &QNetworkReply::finished, this,  &S3BucketLister::networkRequestFinished);

    } else {
        emit listingDone();
    }
}
