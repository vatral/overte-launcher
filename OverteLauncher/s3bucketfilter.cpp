#include <QSysInfo>

#include "s3bucketfilter.h"
#include "s3bucketlister.h"
#include "enums.h"

S3BucketFilter::S3BucketFilter() {
    setOS(OperatingSystem::Auto);
}


void S3BucketFilter::setOS(OperatingSystem os)
{
    _os = os;

    if (_os == OperatingSystem::Auto) {
#if defined(Q_OS_LINUX)
        _os = OperatingSystem::Linux;
#elif defined(Q_OS_MACOS)
        _os = OperatingSystem::MacOS;
#elif defined(Q_OS_WINDOWS)
        _os = OperatingSystem::Windows;
#else
    #error "Building on unknown OS, fix me!"
#endif
    }
}

QList<S3BucketLister::FileData> S3BucketFilter::filter(const QList<S3BucketLister::FileData> &data, ReleaseType type)
{
    QList<S3BucketLister::FileData> filteredFiles;
    QString arch = QSysInfo::currentCpuArchitecture();

    for (const auto &file : data) {

        switch(_os) {
            case OperatingSystem::Linux:
                if (!file.fileName.endsWith(".AppImage")) {
                    continue;
                }

                if (!file.fileName.contains(arch)) {
//                    qInfo() << "Skipping file" << file.fileName << "because it does not contain architecture" << arch;
                    continue;
                }

                break;
            case OperatingSystem::MacOS:
                if (!file.fileName.endsWith(".dmg")) {
                    continue;
                }
                break;
            case OperatingSystem::Windows:
                if (!file.fileName.endsWith(".exe")) {
                    continue;
                }
                break;
            case OperatingSystem::Auto:
                qCritical() << "Auto value should never make it here, fix me!";
                break;
        }

        switch(type) {
            case ReleaseType::Release:
                if (!file.fileName.contains("/release/")) {
                    continue;
                }
                break;
            case ReleaseType::ReleaseCandidate:
                if (!file.fileName.contains("/release-candidate/")) {
                    continue;
                }
                break;
            case ReleaseType::PR:
                if (!file.fileName.contains("/PR")) {
                    continue;
                }
                break;
            case ReleaseType::MasterBuild:
                if (!file.fileName.contains("/master")) {
                    continue;
                }
                break;
            case ReleaseType::Any:
                break;
        }


        filteredFiles.append(file);
    }

    return filteredFiles;
}
