#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include"transaction.h"
#include"date.h"
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
    void transactionCreated();

private slots:
    void accept()override;
    void onTransactionChanged();

private:
    Ui::AddTransactionDialog *ui;
    void setupConnections();
    void setupInitialState();
    Transaction*createTransaction();
};

#endif // ADDTRANSACTIONDIALOG_H
