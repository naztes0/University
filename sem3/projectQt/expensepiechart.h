#ifndef EXPENSEPIECHART_H
#define EXPENSEPIECHART_H

#include <QWidget>
#include <QLabel>
#include <QChartView>
#include <QPieSeries>
#include <QChart>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDate>
#include <QDateTime>
#include"databasemanager.h"

class ExpensePieChart : public QWidget
{
    Q_OBJECT

public:
    explicit ExpensePieChart(DatabaseManager* dbManager, int userId, QWidget* parent = nullptr);
    void updateChart();

private:
    DatabaseManager* m_dbManager;
    int m_userId;


    QChartView* m_incomeChartView;
    QChartView* m_expenseChartView;

    QWidget* m_legendWidget;
    QHBoxLayout* m_legendLayout;

    QVector<QColor> m_colors;

    QMap<QString, double> calculateCategoryData(bool isExpense);
    QWidget* createLegendItem(const QString& category, const QColor& color, bool isExpense);
    void createChart(QChartView* chartView, const QMap<QString, double>& data, bool isExpense);

};

#endif // EXPENSEPIECHART_H
