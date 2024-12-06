#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include "addtransactiondialog.h"
#include "databasemanager.h"
#include "transactionslist.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include"expensepiechart.h"

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget* parent = nullptr);
    void initialize(DatabaseManager* dbManager, int userId);
    void refreshHomeCategories();
    void pieChartscreation();
   ~HomeWidget();
private slots:
    //adding tr
    void on_addTrPushButton_clicked();
    void onTransactionAdded();
    void onCategoryButtonClicked();

private:
    Ui::HomeWidget *ui;
    DatabaseManager*m_dbManager;
    int m_userId;
    QGridLayout* m_categoriesLayout;

    QWidget* createCategoryButton(const QString&category, double expenses);
    void setupCategoriesWidget();
    void loadUserCategories();
    double calculateCategoryExpenses(const QString& category, int year, const QString& month);
    QLabel* createCategoryIcon(const QString& category);
    QString getIconPathForCategory(const QString& category);
    void updateMonthlyTotals();
    void createPieCharts();


};

#endif
