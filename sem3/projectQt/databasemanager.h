#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase> //basic class to open with Qt
#include <QtSql/QSqlQuery> //class for querries
#include <QtSql/QSqlError> //get text of the SQL error
#include <QDebug>
#include <QDateTime> //exact time
#include <QCryptographicHash> //for hashing data

class DatabaseManager
{
public:
    DatabaseManager();

    bool initializeDatabase();
    bool addUser(const QString&login, const QString&email,const QString&password);
    int validateUser(const QString&email, const QString&password);
    bool userExists(const QString& login);
    bool emailExists(const QString &email);

    //methods to work with transactions
    bool addTransaction(int userId,bool isExpense, const QString&category,
                    double amount, const QDateTime&date,const QString&comment);
    bool updateTransaction(int transactionId,bool isExpense, const QString&category,
                           double amount,const QDateTime&date, const QString& comment);
    bool deleteTransaction(int transactionId);
    QSqlQuery getUserTransactions(int userId);
    QSqlQuery getTransactionById(int transactionId);

private:
    QSqlDatabase db;
    bool createTables();
    QString hashPassword (const QString&password);
    bool connectToDatabase();
};

#endif // DATABASEMANAGER_H
