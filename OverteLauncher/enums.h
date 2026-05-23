#ifndef ENUMS_H
#define ENUMS_H

#include <QDebug>

/**
 * @brief Type of Overte release
 */
enum class ReleaseType {
    Release,
    ReleaseCandidate,
    PR,
    MasterBuild,
    Any
};

Q_DECLARE_METATYPE(ReleaseType)

inline QDebug& operator<<(QDebug &debug, const ReleaseType type) {
    switch(type) {
        case ReleaseType::Release:
            debug.nospace() << "Release";
            break;
        case ReleaseType::ReleaseCandidate:
            debug.nospace() << "ReleaseCandidate";
            break;
        case ReleaseType::PR:
            debug.nospace() << "PR";
            break;
        case ReleaseType::MasterBuild:
            debug.nospace() << "MasterBuild";
            break;
        case ReleaseType::Any:
            debug.nospace() << "Any";
            break;
        default:
            debug.nospace() << "Unknown";
    }

    return debug.space();
}

#endif // ENUMS_H
