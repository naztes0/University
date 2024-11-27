#include "transactionslist.h"
#include "ui_transactionslist.h"

TransactionsList::TransactionsList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TransactionsList)
{
    ui->setupUi(this);
}

TransactionsList::~TransactionsList()
{
    delete ui;
}
