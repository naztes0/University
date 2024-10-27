#include "date.h"

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {
}

Date::Date(const QDate& qdate)
    : day(qdate.day()), month(qdate.month()), year(qdate.year()) {
}

bool Date::isValid() const {
    return QDate(year, month, day).isValid();
}

bool Date::isEarlier(const Date& date1, const Date& date2) const {
    QDate d1(date1.year, date1.month, date1.day);
    QDate d2(date2.year, date2.month, date2.day);
    return d1 < d2;
}

QString Date::toString() const {
    return QString("%1.%2.%3")
    .arg(day, 2, 10, QChar('0'))
        .arg(month, 2, 10, QChar('0'))
        .arg(year);
}

QDate Date::toQDate() const {
    return QDate(year, month, day);
}

void Date::setDay(int d) {
    day = d;

}

void Date::setMonth(int m) {
    month = m;

}

void Date::setYear(int y) {
    year = y;

}
