#ifndef TRANSACTIONSLIST_H
#define TRANSACTIONSLIST_H

#include <QWidget>

namespace Ui {
class TransactionsList;
}

class TransactionsList : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsList(QWidget *parent = nullptr);
    ~TransactionsList();

private:
    Ui::TransactionsList *ui;
};

#endif // TRANSACTIONSLIST_H
