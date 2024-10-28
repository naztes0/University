#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include "addtransactiondialog.h"
#include "transaction.h"

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
    void onAddTransactionClicked();
    void onTransactionCreated(Transaction* transaction);

private:
    Ui::HomeWidget *ui;
    AddTransactionDialog *transactionDialog;
};

#endif
