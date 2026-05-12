#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QNetworkAccessManager>
#include <QLoggingCategory>

#include "installedversion.h"
#include "installedlistmodel.h"
#include "programrunner.h"

Q_DECLARE_LOGGING_CATEGORY(MainWindowLog)

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
public slots:
    void showDownloadListDialog();
    void updateInstalledVersions();
    void launchSelected();
    void reportBug();
    void goToCommunity();
    void loadChangelog();

    std::optional<InstalledVersion> getSelectedVersion() const;

private:
    QList<InstalledVersion> _installedVersions;
    InstalledListModel _model;
    InstalledListProxyModel _sortModel;
    ProgramRunner _runner;
    QNetworkAccessManager _networkManager;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
