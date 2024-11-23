#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"
#include <QMessageBox>
#include <QPushButton>

AddTransactionDialog::AddTransactionDialog(DatabaseManager*dbManager,int userId,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTransactionDialog)
    ,m_dbManager(dbManager)
    ,userId(userId)

{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->disconnect();
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),&QPushButton::clicked,
            this,&AddTransactionDialog::validateAndAccept);
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
    ui->categoriesComboBox->addItem("...");
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

void AddTransactionDialog::validateAndAccept(){
    if(ui->sumLineEdit->text().isEmpty()){
        QMessageBox::warning(this, "Warning", "Please enter the amount");
        return;
    }

    bool ok;
    double amount=ui->sumLineEdit->text().toDouble(&ok);
    QString category=ui->categoriesComboBox->currentText();
    if(category=="..."){
        QMessageBox::warning(this, "Warning", "Please choose category");
        return;
    }
    if(!ok||amount<=0){
        QMessageBox::warning(this, "Warning", "Please enter a valid positive amount");
        return;
    }
    bool isExpense=ui->expenseRadio->isChecked();
    QString comment= ui->commentLineEdit->text();
    QDateTime dateTime(ui->dateEdit->date(),QTime::currentTime());

    // Transaction*transaction=createTransaction();
    // emit transactionAdded(transaction);
    // accept();//Close dialog Window
    if(m_dbManager->addTransaction(userId,isExpense,category,amount,dateTime,comment)){
        accept();
    }
    else{
        QMessageBox::warning(this,"Error","Failed to add transaction");

    }


}

void AddTransactionDialog::on_buttonBox_rejected(){
    reject();
}
