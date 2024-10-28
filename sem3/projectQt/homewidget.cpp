#include "homewidget.h"
#include "ui_homewidget.h"
#include <QDebug>

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    transactionDialog = new AddTransactionDialog(this);
    connect(ui->addTrPushButton, &QPushButton::clicked, this, &HomeWidget::onAddTransactionClicked);
    connect(transactionDialog, &AddTransactionDialog::transactionCreated,
            this, &HomeWidget::onTransactionCreated);
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::onAddTransactionClicked()
{
    transactionDialog->exec();
}

void HomeWidget::onTransactionCreated(Transaction* transaction)
{
    // Тут можна додати логіку обробки нової транзакції
    // Наприклад, додати її до списку транзакцій, оновити відображення тощо
    qDebug() << "New transaction created:";
    qDebug() << "Amount:" << transaction->getAmount();
    qDebug() << "Category:" << transaction->getCategory();
    qDebug() << "Description:" << transaction->getDescription();
    qDebug() << "Is Income:" << transaction->getIsIncome();

    // Не забудьте видалити transaction, якщо він більше не потрібен
    // delete transaction;
}
