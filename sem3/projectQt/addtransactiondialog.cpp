#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"
#include <QMessageBox>

AddTransactionDialog::AddTransactionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTransactionDialog)
{
    ui->setupUi(this);
    setWindowTitle("Add Transaction");
    ui->transactionDetails->setTitle("Transaction Details");
    //current date
    ui->dateEdit->setDate(QDate::currentDate());
    initializeCategories();
    //as default:expense
    ui->expenseRadio->setChecked(true);
}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}

void AddTransactionDialog::initializeCategories(){
    ui->categoriesComboBox->addItem("Food");
    ui->categoriesComboBox->addItem("Transport");
    ui->categoriesComboBox->addItem("Entertainment");
    ui->categoriesComboBox->addItem("Salary");
    ui->categoriesComboBox->addItem("Investment");
}

Transaction*AddTransactionDialog::createTransaction(){
    double amount=ui->sumLineEdit->text().toDouble();
    QString category=ui->categoriesComboBox->currentText();
    QString description=ui->commentLineEdit->text();
    QDate date=ui->dateEdit->date();

    Transaction*transaction;
    if(ui->incomeRadio->isChecked()){
        transaction=new IncomeTransaction(0,amount,category,description,date);
    }
    else{
        transaction=new ExpenseTransaction(0,amount,category,description,date);
    }
    return transaction;
}

void AddTransactionDialog::on_buttonBox_accepted(){
    if(ui->sumLineEdit->text().isEmpty()){
        QMessageBox::warning(this, "Warning", "Please enter the amount");
        return;
    }

    bool ok;
    double amount=ui->sumLineEdit->text().toDouble(&ok);
    if(!ok||amount<=0){
        QMessageBox::warning(this, "Warning", "Please enter a valid positive amount");
        return;
    }
    Transaction*transaction=createTransaction();
    emit transactionAdded(transaction);
    accept();//Close dialog Window

}

void AddTransactionDialog::on_buttonBox_rejected(){
    reject();
}
