#include "homewidget.h"
#include "ui_homewidget.h"
#include <QDebug>

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    ui->addTrPushButton->setText("Add Transaction");
}

HomeWidget::~HomeWidget()
{
    qDeleteAll(transactions);
    transactions.clear();
    delete ui;
}

void HomeWidget:: on_addTrPushButton_clicked(){
    AddTransactionDialog dialog(m_dbManager, m_userId, this);
    connect(&dialog,&AddTransactionDialog::transactionAdded,this,&HomeWidget::onTransactionAdded);
    dialog.exec();
}
void HomeWidget::onTransactionAdded(Transaction* transaction)
{
    // Додаємо транзакцію до вектору
    transactions.append(transaction);

    // Тут можна додати код для оновлення інтерфейсу
    qDebug() << "Added new transaction:";
    qDebug() << "Type:" << transaction->getType();
    qDebug() << "Amount:" << transaction->getAmount();
    qDebug() << "Category:" << transaction->getCategory();
    qDebug() << "Description:" << transaction->getDescription();
    qDebug() << "Date:" << transaction->getDate();
}
