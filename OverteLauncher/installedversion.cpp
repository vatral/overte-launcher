#include <QDir>
#include <QStandardPaths>
#include <QProcess>

#include "installedversion.h"

Q_LOGGING_CATEGORY(InstalledVersionLog, "InstalledVersion")


InstalledVersion::InstalledVersion() {}

bool InstalledVersion::launch(const QStringList arguments, bool startDetached) {

}

QList<InstalledVersion> InstalledVersion::findInstalledVersions() {
    auto versions = QList<InstalledVersion>();

#if defined(Q_OS_UNIX) && !defined(Q_OS_APPLE)
    QDir downloadDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!downloadDir.exists("downloads")) {
        downloadDir.mkpath("downloads");
    }
    downloadDir.cd("downloads");

    qCInfo(InstalledVersionLog) << "Scanning for installed versions in" << downloadDir;


    auto files = downloadDir.entryInfoList(QDir::Files, QDir::Time);
    for (const auto &file : files) {
        if (!file.fileName().endsWith(".AppImage")) {
            qCWarning(InstalledVersionLog) << "Skipping non-AppImage" << file;
            continue;
        }

        if (!file.isExecutable()) {
            qCWarning(InstalledVersionLog) << "Skipping non-executable" << file;
            continue;
        }

        InstalledVersion version;
        version._description = file.completeBaseName();
        version._date = file.birthTime();
        version._size = file.size();
        version._fullBinaryPath = file.absoluteFilePath();

        versions.append(version);
    }


    qCInfo(InstalledVersionLog) << "Returning" << versions.count() << "installs";
    return versions;
#else
    qCCritical(InstalledVersionLog) << "Current operating system not recognized, don't know how to detect installs";
    return versions;
#endif
}
