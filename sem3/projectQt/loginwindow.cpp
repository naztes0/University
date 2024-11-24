#include "loginwindow.h"
#include "ui_loginwindow.h"
#include<QMessageBox>
#include<QRegularExpression>

LoginWindow::LoginWindow(DatabaseManager*dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
    ,manager(dbManager)
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
    QString email=ui->emailLineEdit->text().trimmed();
    QString password=ui->passwordLineEdit->text();
    if(email.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"Error","Please fill in all fields");
        return;
    }
    QRegularExpression emailRegex ("^[A_Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if(!emailRegex.match(email).hasMatch()){
        QMessageBox::warning(this,"Error","Please enter a valid email adress");
        return;
    }

    QString userId= manager->validateUser(email,password);
    if(!userId.isEmpty()){
        emit loginSuccessful(userId.toInt());
        accept();//closing window
    }
    else{
        QMessageBox::warning(this, "Login Failed", "Invalid email or password. Please check your credentials or sign up for a new account.");
         ui->passwordLineEdit->clear();
    }
}

void LoginWindow::on_createAccountButton_clicked(){
    QString login=ui->regLoginLineEdit->text().trimmed();
    QString email=ui->regEmailLineEdit->text().trimmed();
    QString password=ui->regPasswordLineEdit->text();

    if(login.isEmpty()||email.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"Error","Please fill in all fields");
        return;
    }
    //email validation
    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Error", "Please enter a valid email address");
        return;
    }
    //password length checker
    if(password.length()<6){
        QMessageBox::warning(this,"Error","Password must be at least 6 characters long");
        return;
    }
    //login validation
    QRegularExpression loginRegex("^[A-Za-z0-9_]{3,20}$");
    if (!loginRegex.match(login).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username must be 3-20 characters long and can only contain letters, numbers, and underscores");
        return;
    }

    if(manager->addUser(login,email,password)){
        //custom message box with tick
        QMessageBox msgBox;
        msgBox.setWindowTitle("Success");
        msgBox.setText("Successfuly signed up");
        msgBox.setIconPixmap(QPixmap(":/img/img/check-circle.svg"));
        msgBox.exec();

        //Automatic login after sign up
        QString userId=manager->validateUser(email,password);
        if(!userId.isEmpty()){

            emit loginSuccessful(userId.toInt());
            accept();

        }
    }
        else{
        bool loginExists = manager->userExists(login);
        bool emailExists = manager->emailExists(email);

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

void LoginWindow::on_signupButton_clicked(){
    showSignupForm();
}

