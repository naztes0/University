#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"

AddTransactionDialog::AddTransactionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTransactionDialog)
{
    ui->setupUi(this);
}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}
