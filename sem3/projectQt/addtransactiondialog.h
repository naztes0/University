#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include"transaction.h"
#include"databasemanager.h"
#include<QDateTime>
#include"customcategorycombobox.h"


namespace Ui {
class AddTransactionDialog;
}

class AddTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    ~AddTransactionDialog();
    explicit AddTransactionDialog(DatabaseManager*dbManager,int userId ,QWidget *parent = nullptr);
    void setTransactionData(const QJsonObject& transaction);
signals:
    //send wh new transaction is crating
    void transactionAdded();

private slots:
    //Ok,Cancel
    void validateAndAccept();
    void on_buttonBox_rejected();
    void onAddCategory();
    void onDeleteCategory(const QString& category);

private:
    Ui::AddTransactionDialog *ui;
    DatabaseManager* manager;
    int userId;
    CustomCategoryComboBox*m_categoriesComboBox;

    void initializeCategories();
    Transaction*createTransaction();
    QString m_transactionId;
    QDateTime m_originalDateTime;

    void setupCustomCategoryComboBox();

};

#endif // ADDTRANSACTIONDIALOG_H
