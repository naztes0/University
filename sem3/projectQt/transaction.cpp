#include "transaction.h"

Transaction::Transaction(double amt,const Date&dt, const QString&descrip,const QString&categ, bool income )
        :amount(amt),
        date(dt),
        description(descrip),
        category(income?"Income":categ),
        isIncome(income)
{

}

void Transaction::setAmount(double amt) {
    if(amt>0)
    amount = amt;
}

void Transaction::setDate(const Date& dt) {
    date = dt;
}

void Transaction::setDescription(const QString& desc) {
    description = desc;
}

void Transaction::setCategory(const QString& categ) {
    if(!isIncome&&!categ.isEmpty()){
    category = categ;
    }
}

IncomeTransaction::IncomeTransaction(double amount,const Date&dt,const QString&descrip,const QString&src)
    : Transaction(amount, dt, descrip, "Income", true)
    , source(src)
{}

void IncomeTransaction::setSource(const QString&newSource){
    source=newSource;
}

void IncomeTransaction::setCategory(const QString& categ) {
    // Категорія завжди залишається "Income"
}
ExpenseTransaction::ExpenseTransaction(double amount,const Date&dt, const QString&descrip, const QString&categ)
     : Transaction(amount, dt, descrip, categ, false)
{}


