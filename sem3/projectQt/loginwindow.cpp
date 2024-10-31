#include "loginwindow.h"
#include "ui_loginwindow.h"
#include<QMessageBox>
#include<QRegularExpression>

LoginWindow::LoginWindow(DatabaseManager*dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
    ,m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->regPasswordLineEdit->setEchoMode(QLineEdit::Password);

    showLoginForm();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginButton_clicked()
{

}

void LoginWindow::on_signupButton_clicked()
{

}
void LoginWindow::showLoginForm(){
    ui->stackedWidget->setCurrentIndex(0);
    setWindowTitle("Log In");
    clearForms();
}

void LoginWindow::showSignupForm(){
    ui->stackedWidget->setCurrentIndex(1);
    setWindowTitle("Sign Up");
    clearForms();
}


void LoginWindow::clearForms(){
    ui->emailLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->regLoginLineEdit->clear();
    ui->regEmailLineEdit->clear();
    ui->regPasswordLineEdit->clear();
}
void LoginWindow::on_goToSignupButton_clicked(){
    showSignupForm();
}

void LoginWindow::on_createAccount_clicked(){
    showLoginForm();
}
