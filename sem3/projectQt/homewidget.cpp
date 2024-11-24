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
    AddTransactionDialog dialog(manager, userId, this);
    connect(&dialog,&AddTransactionDialog::transactionAdded,this,&HomeWidget::onTransactionAdded);
    dialog.exec();
}

