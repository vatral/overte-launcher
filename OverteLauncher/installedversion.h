#ifndef INSTALLEDVERSION_H
#define INSTALLEDVERSION_H
#include <QDateTime>
#include <QString>
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonObject>

#include "enums.h"
#include "s3bucketlister.h"

Q_DECLARE_LOGGING_CATEGORY(InstalledVersionLog)



/**
 * @brief Installed Overte version
 * This class contains the information about an Overte install.
 */
class InstalledVersion {
public:

    /**
     * @brief Metadata regarding an installed version
     *
     * This contains extra metadata obtained at download time from the S3 bucket,
     * and which may not be easily obtainable from the downloaded file on disk.
     */
    class Metadata {
    public:
        Metadata(const QString &description, const QDateTime &dateTime, ReleaseType type) :
            _description(description), _uploadDate(dateTime), _releaseType(type) {}

        Metadata(const S3BucketLister::FileData &fileData, ReleaseType type) :
            _description(fileData.fileName), _uploadDate(fileData.lastModified), _releaseType(type) {}

        const QString &getDescription() const { return _description; }
        const QDateTime &getUploadDate() const { return _uploadDate; }
        const ReleaseType &getReleaseType() const { return _releaseType; }

        QJsonObject toJson() const;
        static std::optional<Metadata> fromJson(const QJsonObject &obj);

    private:
        QString _description;
        QDateTime _uploadDate;
        ReleaseType _releaseType;

        friend class InstalledVersion;
    };


    /**
     * @brief The description to be shown to the user
     * @return Description string
     */
    const QString &getDescription() const {
        return _description;
    }

    /**
     * @brief The date of this version. This is the release date, not the install date.
     * @return Date
     */
    const QDateTime &getDate() const {
        return _date;
    }

    /**
     * @brief The size of the install in bytes. 0 if unknown.
     * @return
     */
    quint64 getSize() const {
        return _size;
    }

    const QString &getFullBinaryPath() const {
        return _fullBinaryPath;
    }

    /**
     * @brief Find all the installed versions of Overte on this system.
     * What it does underneath is OS dependent.
     * @return List of installed versions
     */
    static QList<InstalledVersion> findInstalledVersions();



private:
    InstalledVersion();


    QString _description;
    QString _fullBinaryPath;
    QDateTime _date;
    quint64 _size;
};

inline QDebug& operator<<(QDebug &debug, const InstalledVersion &data) {
        debug.nospace() << "InstalledVersion(description=" << data.getDescription()
                        << ", date=" << data.getDate().toString(Qt::ISODate)
                        << ", size=" << data.getSize()
                        << ")";

    return debug.space();
}

#endif // INSTALLEDVERSION_H
