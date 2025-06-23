#include "transaction.h"


Transaction::Transaction()
    : id(0), amount(0.0), description(""), date(QDate::currentDate()), category("") {
}

// Реалізація параметризованого конструктора
Transaction::Transaction(int id, double amount,const QString& category,const QString& description,const QDate& date)
    : id(id), amount(amount), description(description), date(date), category(category) {
}

Transaction::~Transaction() {
}
