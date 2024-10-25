#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sectionscolumn.h"
#include<QDebug>
#include"homewidget.h"

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
    void showHomeWidget();

private:
    Ui::MainWindow *ui;
    SectionsColumn *sectionsColumn;
    HomeWidget*homeWidget;
    QWidget*currentWidget;
    void setupConnections();
};
#endif // MAINWINDOW_H
