#ifndef TRANSACTIONSLIST_H
#define TRANSACTIONSLIST_H

#include <QWidget>
#include<QVBoxLayout>
#include<QLabel>
#include<QMenu>
#include"databasemanager.h"

namespace Ui {
class TransactionsList;
}

class TransactionsList : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsList(DatabaseManager*dbManager,int userId,QWidget *parent = nullptr);
    ~TransactionsList();
    void refreshTransactionsList();


private:
    Ui::TransactionsList *ui;
    DatabaseManager*manager;
    int m_userId;
    QVBoxLayout*m_mainLayout;

    void setupUI();
    void loadtransactions();
    void createTransactionItem(const QJsonObject& transaction);
    void setupContextMenu(QWidget*transactionWidget, int transactionId);
};

#endif // TRANSACTIONSLIST_H
