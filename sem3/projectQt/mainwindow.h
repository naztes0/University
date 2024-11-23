#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sectionscolumn.h"
#include<QDebug>
#include<QMessageBox>
#include"homewidget.h"
#include"databasemanager.h"
#include"loginwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();


public slots:
    void setCurrentUserId(int userId){currentUserId=userId;}
    void showHomeWidget();

private:
    Ui::MainWindow *ui;
    SectionsColumn *sectionsColumn;
    HomeWidget*homeWidget;
    QWidget*currentWidget;
    DatabaseManager m_dbManager;
    int currentUserId;
    void setupConnections();
};
#endif // MAINWINDOW_H
