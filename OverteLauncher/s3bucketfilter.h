#ifndef S3BUCKETFILTER_H
#define S3BUCKETFILTER_H

#include "s3bucketlister.h"
#include "enums.h"

/**
 * @brief Filters the list of files from the S3 bucket to only include those relevant for the current platform.
 */
class S3BucketFilter
{
public:

    enum class OperatingSystem {
        Linux,
        MacOS,
        Windows,
        Auto
    };

    explicit S3BucketFilter();

    void setOS(OperatingSystem os);

    OperatingSystem getOS() const { return _os; }

    /**
     * @brief Filters the given list of files to only include those relevant for the current platform.
     * @param data The list of files to filter.
     * @return The filtered list of files. If somehow the proper architecture cannot be determined,
     * the original list will be returned unfiltered.
     */
    QList<S3BucketLister::FileData> filter(const QList<S3BucketLister::FileData> &data, ReleaseType type = ReleaseType::Any);

private:
    OperatingSystem _os = OperatingSystem::Auto;
};

#endif // S3BUCKETFILTER_H
