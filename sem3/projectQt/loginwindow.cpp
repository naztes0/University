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
    QString email=ui->emailLineEdit->text();
    QString password=ui->passwordLineEdit->text();
    if(email.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"Error","Please fill in all fields");
        return;
    }
    int userId=m_dbManager->validateUser(email,password);
    if(userId>0){
        emit loginSuccessful(userId);
        accept();//close login wind
    }
    else{
        QMessageBox::warning(this, "Error", "The user with this email and password wasn`t."
                                            "Plesae check the entered data or Sign Up ");
    }
}

void LoginWindow::on_signupButton_clicked()
{
    showSignupForm();
}

void LoginWindow::on_createAccount_clicked(){
    QString login=ui->regLoginLineEdit->text();
    QString email=ui->regEmailLineEdit->text();
    QString password=ui->regPasswordLineEdit->text();
    if(login.isEmpty()||email.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"Error","Please fill in all fields");
        return;
    }
    if(m_dbManager->addUser(login,email,password)){
        //custom message box with tick
        QMessageBox msgBox;
        msgBox.setWindowTitle("Success");
        msgBox.setText("Successfuly signed up");
        msgBox.setIconPixmap(QPixmap(":/img/img/check-circle.svg"));
        msgBox.exec();
        int userId=m_dbManager->validateUser(email,password);
        if(userId>0){
            emit loginSuccessful(userId);
            accept();
        }
    }
    else{
        bool loginExists = m_dbManager->userExists(login);
        bool emailExists = m_dbManager->emailExists(email);
        QString errorMsg;
        if (loginExists && emailExists) {
            errorMsg = "User with this login and email already exists.";
        } else if (loginExists) {
            errorMsg = "User with this login already exists. Choose another one.";
        } else if (emailExists) {
            errorMsg = "User with this email already exists. Enter another one.";
        }

        QMessageBox::warning(this, "Registration failed", errorMsg);
    }
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


