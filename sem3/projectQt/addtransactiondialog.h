#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include"transaction.h"

namespace Ui {
class AddTransactionDialog;
}

class AddTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransactionDialog(QWidget *parent = nullptr);
    ~AddTransactionDialog();
signals:
    //send wh new transaction is crating
    void transactionAdded(Transaction*transaction);

private slots:
    //Ok,Cancel
    void validateAndAccept();
    void on_buttonBox_rejected();

private:
    Ui::AddTransactionDialog *ui;
    void initializeCategories();
    Transaction*createTransaction();
};

#endif // ADDTRANSACTIONDIALOG_H
