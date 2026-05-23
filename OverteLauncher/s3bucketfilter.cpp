#include <QSysInfo>

#include "s3bucketfilter.h"
#include "s3bucketlister.h"
#include "enums.h"

S3BucketFilter::S3BucketFilter() {}


QList<S3BucketLister::FileData> S3BucketFilter::filter(const QList<S3BucketLister::FileData> &data, ReleaseType type)
{
    QList<S3BucketLister::FileData> filteredFiles;
    QString arch = QSysInfo::currentCpuArchitecture();

    for (const auto &file : data) {

        if (!file.fileName.contains(arch)) {
            continue;
        }

#if defined(Q_OS_LINUX)
        if (!file.fileName.endsWith(".AppImage")) {
            continue;
        }
#elif defined(Q_OS_MACOS)
        if (!file.fileName.endsWith(".dmg")) {
            continue;
        }
#elif defined(Q_OS_WINDOWS)
        if (!file.fileName.endsWith(".exe")) {
            continue;
        }
#endif
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

    return std::move(filteredFiles);
}
