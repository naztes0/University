#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include "addtransactiondialog.h"
#include "transaction.h"
#include<QVector>
#include"databasemanager.h"


namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();
    void setDatabaseManager(DatabaseManager*dbmanager){m_dbManager=dbmanager;}
    void setUserId(int userId){m_userId=userId;}

private slots:
    //adding tr
    void on_addTrPushButton_clicked();
    //getting new tr
    void onTransactionAdded(Transaction* transaction);

private:
    Ui::HomeWidget *ui;
    QVector<Transaction*> transactions;
    DatabaseManager*m_dbManager;
    int m_userId;
};

#endif
