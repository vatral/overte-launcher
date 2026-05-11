#ifndef PROGRAMRUNNER_H
#define PROGRAMRUNNER_H

#include <QObject>
#include <QLoggingCategory>
#include <QProcess>

Q_DECLARE_LOGGING_CATEGORY(ProgramRunnerLog)

class ProgramRunner : public QObject
{
    Q_OBJECT
public:
    explicit ProgramRunner(QObject *parent = nullptr);
    bool runProgram(const QString &path, const QStringList &arguments = QStringList(), bool startDetached = false);

private slots:
    void processStdout();
    void processStderr();
    void processError(QProcess::ProcessError error);
    void processStateChanged(QProcess::ProcessState newState);
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);


signals:

private:
    QProcess _process;
};

#endif // PROGRAMRUNNER_H
