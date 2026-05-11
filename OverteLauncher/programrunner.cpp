#include "programrunner.h"



Q_LOGGING_CATEGORY(ProgramRunnerLog, "Runner")

ProgramRunner::ProgramRunner(QObject *parent)
    : QObject{parent}
{

    connect(&_process, &QProcess::readyReadStandardOutput, this, &ProgramRunner::processStdout);
    connect(&_process, &QProcess::readyReadStandardError, this, &ProgramRunner::processStderr);
    connect(&_process, &QProcess::errorOccurred, this, &ProgramRunner::processError);
    connect(&_process, &QProcess::finished, this, &ProgramRunner::processFinished);
    connect(&_process, &QProcess::started, this, &ProgramRunner::processStarted);
    connect(&_process, &QProcess::stateChanged, this, &ProgramRunner::processStateChanged);



}

bool ProgramRunner::runProgram(const QString &path, const QStringList &arguments, bool startDetached) {
    qCInfo(ProgramRunnerLog) << "Launching" << path << "with arguments" << arguments;

    bool result;


    if (startDetached) {
        // Don't inherit stdin/out/err
        _process.setStandardInputFile(QProcess::nullDevice());
        _process.setStandardOutputFile(QProcess::nullDevice());
        _process.setStandardErrorFile(QProcess::nullDevice());
    }

    // Current AppImage has a problem with Wayland, force XCB
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "xcb");
    _process.setProcessEnvironment(env);

    if (startDetached) {
        auto result = _process.startDetached(path, arguments);
        if (!result) {
            qCCritical(ProgramRunnerLog) << "Failed to launch" << path;
            return false;
        }
        return true;
    } else {
        _process.start(path, arguments);
        //        if (!process.) {
        //          qCCritical(InstalledVersionLog) << "Failed to launch" << getDescription();
        //    }
        return result;
    }
}

void ProgramRunner::processStdout()
{
    qCInfo(ProgramRunnerLog) << "Process STDOUT:" << _process.readAllStandardOutput();
}

void ProgramRunner::processStderr()
{
    qCInfo(ProgramRunnerLog) << "Process STDERR:" << _process.readAllStandardError();
}

void ProgramRunner::processError(QProcess::ProcessError error)
{
    qCInfo(ProgramRunnerLog) << "Process error" << error;

}

void ProgramRunner::processStateChanged(QProcess::ProcessState newState)
{
    qCInfo(ProgramRunnerLog) << "Process state changed to" << newState;
}

void ProgramRunner::processStarted()
{
    qCInfo(ProgramRunnerLog) << "Process started";
}

void ProgramRunner::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCInfo(ProgramRunnerLog) << "Process finished with code"<< exitCode << "; status" << exitStatus;
}
