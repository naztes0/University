#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    homeWidget(nullptr), currentWidget(nullptr)

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
    }
    homeWidget=ui->contentWidget;
    currentWidget=homeWidget;
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


