#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>

namespace Ui {
class AddTransactionDialog;
}

class AddTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransactionDialog(QWidget *parent = nullptr);
    ~AddTransactionDialog();

private:
    Ui::AddTransactionDialog *ui;
};

#endif // ADDTRANSACTIONDIALOG_H
