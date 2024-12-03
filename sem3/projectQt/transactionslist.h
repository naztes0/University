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
    void loadTransactionsByCategory(const QString& category, int year,const QString& month);
    double calculateCategoryExpenses(const QString& category, int year, const QString& month);

private:
    Ui::TransactionsList *ui;
    DatabaseManager*manager;
    int m_userId;
    QVBoxLayout*m_mainLayout;

    void setupUI();
    void loadtransactions();
    void createTransactionItem(const QJsonObject& transaction);
    void setupContextMenu(QWidget*transactionWidget, const QString& transactionId);
};

#endif // TRANSACTIONSLIST_H
