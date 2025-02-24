#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,homeWidget(nullptr)
    ,transactionsListWidget(nullptr)
    , currentWidget(nullptr)
    , currentUserId(-1)
{

    ui->setupUi(this);
    QPalette palette = ui->sectionsColumn->palette();
    palette.setColor(ui->sectionsColumn->backgroundRole(), QColor("#a6a6a6"));
    ui->sectionsColumn->setAutoFillBackground(true);
    ui->sectionsColumn->setPalette(palette);
    // ui->sectionsColumn->setStyleSheet(
    //     "SectionsColumn {"
    //     "    border-top-right-radius: 20px;" // Закруглення кута
    //     "}"
    //     );
    ui->accountButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QPushButton:hover {"
        "    background-color: #D0D0D0;"
        "}"
        );


    QPalette palette1 = ui->contentWidget->palette();
    palette.setColor(ui->sectionsColumn->backgroundRole(), QColor("#ffffff"));
    ui->contentWidget->setAutoFillBackground(true);
    ui->contentWidget->setPalette(palette);

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowTitle("Transaction Manager");
    setWindowIcon(QIcon(":/img/img/bill.png"));
    //login
    LoginWindow *loginWindow = new LoginWindow(&manager, this);
    connect(loginWindow, &LoginWindow::loginSuccessful, this, &MainWindow::setCurrentUserId);

    if (loginWindow->exec() != QDialog::Accepted) {
        // user close the window without authorization
        QTimer::singleShot(0, qApp, &QApplication::quit);// more safe, clean the trash
        return;
    }
    // Підключення сигналу натискання кнопки
    connect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::showAccountMenu);
    //init homeWidget
    homeWidget=ui->contentWidget;
    if(homeWidget){
        homeWidget->initialize(&manager, currentUserId);
        currentWidget = homeWidget;
    }
    else{
        QMessageBox::critical(this,"Error","Failed to initialize HomeWidget");
        QApplication::quit();
        return;
    }
    //init transactionsList
    transactionsListWidget=new TransactionsList(&manager,currentUserId,ui->contentWidget);
    transactionsListWidget->hide();

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
        connect(sectionsColumn,&SectionsColumn::transactionsListButtonClicked,this,&MainWindow::showTransactionsList);
    }
    else{
        qDebug()<<"SectionsColumn not founded";
    }
}


void MainWindow::showHomeWidget() {
    qDebug() << "Caught signal from Home Button";

    if (currentWidget && currentWidget != homeWidget) {
        currentWidget->hide();


    ui->contentWidget->setVisible(true);

    homeWidget->show();
    homeWidget->setGeometry(ui->contentWidget->geometry());
    homeWidget->refreshHomeCategories();
    homeWidget->pieChartscreation();

    ui->sectionsColumn->setGeometry(ui->sectionsColumn->geometry());
    currentWidget = homeWidget;
    }
}

void MainWindow::showTransactionsList() {
    qDebug() << "Caught signal from Transactions List Button";

    if (currentWidget && currentWidget != transactionsListWidget) {
        currentWidget->hide();
    }

    transactionsListWidget->setParent(ui->contentWidget);
    transactionsListWidget->setGeometry(0, 0, ui->contentWidget->width(), ui->contentWidget->height());

    ui->contentWidget->setVisible(true);
    transactionsListWidget->refreshTransactionsList();
    transactionsListWidget->show();

    currentWidget = transactionsListWidget;
}

void MainWindow::showAccountMenu(){
    QMenu*accountMenu= new QMenu(this);
    QAction* logoutAction=accountMenu->addAction("Logout");
    connect (logoutAction,&QAction::triggered,this, &MainWindow::performLogout);
    QPoint pos=ui->accountButton->mapToGlobal(QPoint(0,ui->accountButton->height()));
    accountMenu->exec(pos);
    accountMenu->deleteLater();
}
void MainWindow::performLogout(){
    currentUserId=-1;
    close();
    LoginWindow* loginWindow=new LoginWindow(&manager);
    connect(loginWindow,&LoginWindow::loginSuccessful,this,&MainWindow::setCurrentUserId);
    if(loginWindow->exec()==QDialog::Accepted){
        if(homeWidget){
            homeWidget->initialize(&manager, currentUserId);
        }
        if(transactionsListWidget){
            delete transactionsListWidget;
            transactionsListWidget = new TransactionsList(&manager, currentUserId, ui->contentWidget);
            transactionsListWidget->hide();
        }
        show();

    }
}
