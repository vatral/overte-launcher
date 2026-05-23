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


    S3BucketFilter();

    /**
     * @brief Filters the given list of files to only include those relevant for the current platform.
     * @param data The list of files to filter.
     * @return The filtered list of files. If somehow the proper architecture cannot be determined,
     * the original list will be returned unfiltered.
     */
    static QList<S3BucketLister::FileData> filter(const QList<S3BucketLister::FileData> &data, ReleaseType type = ReleaseType::Any);

};

#endif // S3BUCKETFILTER_H
