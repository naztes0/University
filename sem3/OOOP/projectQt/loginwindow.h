#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include"databasemanager.h"
#include"mainwindow.h"


namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(DatabaseManager*dbManager,QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccessful(int userId);
private slots:
    void on_loginButton_clicked();
    void on_goToSignupButton_clicked();
    void on_signupButton_clicked();
    void on_createAccountButton_clicked();

private:
    Ui::LoginWindow *ui;
    DatabaseManager* manager;
    void showLoginForm();
    void showSignupForm();
    void clearForms();
    void validateLoginForms();
    void validateSignupForms();
};

#endif // LOGINWINDOW_H
