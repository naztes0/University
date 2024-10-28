#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include "addtransactiondialog.h"
#include "transaction.h"
#include<QVector>

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

private slots:
    //adding tr
    void on_addTrPushButton_clicked();
    //getting new tr
    void onTransactionAdded(Transaction* transaction);

private:
    Ui::HomeWidget *ui;
    QVector<Transaction*> transactions;

};

#endif
