#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "installedversion.h"
#include "installedlistmodel.h"

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

private:
    QList<InstalledVersion> _installedVersions;
    InstalledListModel _model;
    InstalledListProxyModel _sortModel;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
