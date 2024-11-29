#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include<QObject>
#include<QtNetwork/QNetworkAccessManager> //HTTP/HTTPS requests
#include<QtNetwork/QNetworkReply> //get reply form HTTP requests
#include<QJsonDocument> //reading and modification of JSON docs
#include<QJsonObject> //JSON obj (key-value)
#include<QJsonArray>//JSON arrays
#include<QDateTime>// date&time
#include<QCryptographicHash> //password hashing
#include<QEventLoop>//synchronization of asynchronous requests
#include<QDebug>


class DatabaseManager:public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject*parent=nullptr);

    int getNextUserId();
    bool addUser(const QString& login, const QString& email, const QString& password);
    bool userExists(const QString& login);
    bool emailExists(const QString& email);
    QString validateUser(const QString& email, const QString&password);

    //Methods to work with transactions
    bool addTransaction(int userId,bool isExpense,const QString& category,
                        double amount, const QDateTime& date,const QString&comment);
    bool updateTransaction(const QString& transactionId, bool isExpense, const QString& category,
                          double amount, const QDateTime& date, const QString& comment);
    bool deleteTransaction(const QString& transactionId);
    QJsonArray getUserTransactions(int userId);
    QJsonObject getTransactionById(const QString& transactionId);


signals:
    void operationCompleted(bool succes);
    void dataReceived(const QJsonDocument&data);

private:
    QNetworkAccessManager*manager;
    QString projectURL;
    QString constructUrl(const QString&path);
    QJsonDocument synchronousRequest(const QString&path, const QString&method,
                                     const QJsonDocument&data=QJsonDocument());
    QString hashPassword(const QString& password);
};

#endif // DATABASEMANAGER_H
