#ifndef TRANSACTION_H
#define TRANSACTION_H

#include"date.h"
#include<QString>
class Transaction
{
protected:
    double amount;
    Date date;
    QString description;
    QString category;
    bool isIncome;
public:
    Transaction(double amount, const Date& dt, const QString& descrip,
                const QString& categ, bool income = false);
    virtual ~Transaction() = default;

    double getAmount() const{return amount;}
    Date getDate() const {return date;}
    QString getDescription()const{return description;}
    QString getCategory()const {return category;}
    bool getIsIncome() const{return isIncome;}

    void setAmount(double amount);
    void setDate(const Date& dt);
    void setDescription(const QString& desc);
    virtual void setCategory(const QString& categ);


    bool isValid() const;
};

class IncomeTransaction:public Transaction{
private:
    QString source;

public:
    IncomeTransaction(double amount, const Date& dt, const QString& descrip,const QString& src);
    QString getSource() const { return source; }
    void setSource(const QString& newSource);
    void setCategory(const QString&categ) override;
};

class ExpenseTransaction:public Transaction{
private:
    QString category;

public:
    ExpenseTransaction(double amount, const Date& dt, const QString& descrip, const QString& category);

};

#endif // TRANSACTION_H
