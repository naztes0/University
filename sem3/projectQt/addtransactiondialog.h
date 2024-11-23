#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include"transaction.h"
#include"databasemanager.h"
#include<QDateTime>

namespace Ui {
class AddTransactionDialog;
}

class AddTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    ~AddTransactionDialog();
    explicit AddTransactionDialog(DatabaseManager*dbManager,int userId ,QWidget *parent = nullptr);
signals:
    //send wh new transaction is crating
    void transactionAdded(Transaction*transaction);

private slots:
    //Ok,Cancel
    void validateAndAccept();
    void on_buttonBox_rejected();

private:
    Ui::AddTransactionDialog *ui;
    DatabaseManager* m_dbManager;
    int userId;
    void initializeCategories();
    Transaction*createTransaction();
};

#endif // ADDTRANSACTIONDIALOG_H
