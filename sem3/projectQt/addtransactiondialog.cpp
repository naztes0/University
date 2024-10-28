#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"
#include <QMessageBox>

AddTransactionDialog::AddTransactionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTransactionDialog)
{
    ui->setupUi(this);
    setupInitialState();
    setupConnections();
}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}

void AddTransactionDialog::setupInitialState()
{
    ui->expenseRadio->setChecked(true);
    ui->dateEdit->setDate(QDate::currentDate());

    // Додаємо базові категорії витрат
    ui->categoriesComboBox->clear();
    ui->categoriesComboBox->addItems({"Food", "Transport", "Entertainment", "Bills", "Other"});
}

void AddTransactionDialog::setupConnections()
{
    connect(ui->expenseRadio, &QRadioButton::toggled, this, &AddTransactionDialog::onTransactionTypeChanged);
    connect(ui->incomeRadio, &QRadioButton::toggled, this, &AddTransactionDialog::onTransactionTypeChanged);
}

void AddTransactionDialog::onTransactionTypeChanged()
{
    ui->categoriesComboBox->clear();

    if (ui->incomeRadio->isChecked()) {
        ui->categoriesComboBox->addItems({"Salary", "Investment", "Gift", "Other"});
    } else {
        ui->categoriesComboBox->addItems({"Food", "Transport", "Entertainment", "Bills", "Other"});
    }
}

void AddTransactionDialog::accept()
{
    // Валідація введених даних
    bool ok;
    double amount = ui->sumLineEdit->text().toDouble(&ok);

    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid positive number for the amount.");
        return;
    }

    if (ui->commentLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a description.");
        return;
    }

    Transaction* transaction = createTransaction();
    if (transaction) {
        emit transactionCreated(transaction);
        QDialog::accept();
    }
}

Transaction* AddTransactionDialog::createTransaction()
{
    double amount = ui->sumLineEdit->text().toDouble();
    QDate qDate = ui->dateEdit->date();
    Date date(qDate.day(), qDate.month(), qDate.year());
    QString description = ui->commentLineEdit->text();
    QString category = ui->categoriesComboBox->currentText();

    if (ui->incomeRadio->isChecked()) {
        return new IncomeTransaction(amount, date, description, category);
    } else {
        return new ExpenseTransaction(amount, date, description, category);
    }
}
