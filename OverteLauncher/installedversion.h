#ifndef INSTALLEDVERSION_H
#define INSTALLEDVERSION_H
#include <QDateTime>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(InstalledVersionLog)



/**
 * @brief Installed Overte version
 * This class contains the information about an Overte install.
 */
class InstalledVersion {
public:


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
     * @brief Launch this version of Overte.
     * @return
     */
    bool launch(const QStringList arguments = QStringList(), bool startDetached = false);

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
