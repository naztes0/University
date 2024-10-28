#ifndef TRANSACTION_H
#define TRANSACTION_H

#include<QDate>
#include<QString>
class Transaction
{
protected:
    int id;
    double amount;
    QString category;
    QString description;
    QDate date;

public:
    Transaction();
    Transaction(int id,double amount, const QString&category, const QString& description, const QDate&date);
    virtual ~Transaction();
    //define type of transaction
    virtual QString getType()const=0;

    //getters
    int getId() const { return id; }
    double getAmount() const { return amount; }
    QString getDescription() const { return description; }
    QDate getDate() const { return date; }
    QString getCategory() const { return category; }

    //setters
    void setId(int newId) { id = newId; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setDescription(const QString& newDescription) { description = newDescription; }
    void setDate(const QDate& newDate) { date = newDate; }
    void setCategory(const QString& newCategory) { category = newCategory; }
};

class IncomeTransaction:public Transaction{
public:
    using Transaction::Transaction;
    QString getType() const override{return "Income";}
};

class ExpenseTransaction:public Transaction{
public:
    using Transaction::Transaction;
    QString getType() const override{return "Expense";}
};

#endif // TRANSACTION_H
