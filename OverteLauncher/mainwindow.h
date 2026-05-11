#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "installedversion.h"
#include "installedlistmodel.h"
#include "programrunner.h"

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

    std::optional<InstalledVersion> getSelectedVersion() const;

private:
    QList<InstalledVersion> _installedVersions;
    InstalledListModel _model;
    InstalledListProxyModel _sortModel;
    ProgramRunner _runner;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
