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
#include <QSet>
#include "databasemanager.h"

class ExpensePieChart : public QWidget
{
    Q_OBJECT

public:
    explicit ExpensePieChart(DatabaseManager* dbManager, int userId, QWidget* parent = nullptr);
    void updateChart();

private:
    struct CategoryData {
        double incomeAmount = 0.0;
        double expenseAmount = 0.0;
    };

    DatabaseManager* m_dbManager;
    int m_userId;

    QChartView* m_expenseChartView;
    QChartView* m_incomeChartView;

    QWidget* m_legendWidget;
    QHBoxLayout* m_legendLayout;

    QVector<QColor> m_baseColors;
    QVector<QColor> m_availableColors;
    QMap<QString, QColor> m_categoryColors;
    QMap<QString, CategoryData> m_categoryDataMap;

    void prepareCategoryData();
    void createCharts();
    QWidget* createLegendItem(const QString& category, const QColor& color);
    QColor getColorForCategory(const QString& category);
    void resetColorPalette();
};

#endif // EXPENSEPIECHART_H
