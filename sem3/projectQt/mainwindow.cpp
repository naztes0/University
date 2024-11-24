#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,homeWidget(nullptr)
    , currentWidget(nullptr)
    , currentUserId(-1)
{

    ui->setupUi(this);
    LoginWindow *loginWindow = new LoginWindow(&manager, this);
    connect(loginWindow, &LoginWindow::loginSuccessful, this, &MainWindow::setCurrentUserId);

    if (loginWindow->exec() != QDialog::Accepted) {
        // user close the window without authorization
        QTimer::singleShot(0, qApp, &QApplication::quit);// more safe, clean the trash
        return;
    }
    homeWidget=qobject_cast<HomeWidget*>(ui->contentWidget);
    if(homeWidget){
        homeWidget->setDatabaseManager(&manager);
        homeWidget->setUserId(currentUserId);
        currentWidget=homeWidget;
    }
    else{
        QMessageBox::critical(this,"Error","Failed to initialize HomeWidget");
        QApplication::quit();
        return;
    }

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections(){
    sectionsColumn=ui->sectionsColumn;
    if (sectionsColumn){
        qDebug()<<"SectionsColumn founded";
        connect(sectionsColumn,&SectionsColumn::homeButtonClicked,this,&MainWindow::showHomeWidget);
    }
    else{
        qDebug()<<"SectionsColumn not founded";
    }
}


void MainWindow::showHomeWidget(){
    qDebug()<<"Caught signal from Home Button";
    if(currentWidget && currentWidget != homeWidget){
        currentWidget->hide();
    }
    homeWidget->show();
    currentWidget = homeWidget;
}

