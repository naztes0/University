#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,homeWidget(nullptr)
    , currentWidget(nullptr)
    , currentUserId(-1)
{
    if(!m_dbManager.initializeDatabase()){
        QMessageBox::critical(this, "Error","Failed to initialize database");
        return;
    }
    ui->setupUi(this);
    LoginWindow *loginWindow = new LoginWindow(&m_dbManager, this);
    connect(loginWindow, &LoginWindow::loginSuccessful, this, &MainWindow::setCurrentUserId);

    if (loginWindow->exec() != QDialog::Accepted) {
        // user close the window without authorization
        QApplication::quit();
        return;
    }
    homeWidget=qobject_cast<HomeWidget*>(ui->contentWidget);
    if(homeWidget){
        homeWidget->setDatabaseManager(&m_dbManager);
        homeWidget->setUserId(currentUserId);
        currentWidget=homeWidget;
    }
    else{
        QMessageBox::critical(this,"Error","Failed to initialize HomeWidget");
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

