#ifndef DATE_H
#define DATE_H

#include <QString>
#include <QDate>

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date(int d, int m, int y);
    Date(const QDate& qdate);

    bool isValid() const;
    bool isEarlier(const Date& date1, const Date& date2) const;
    QString toString() const;
    QDate toQDate() const;

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);
};

#endif
