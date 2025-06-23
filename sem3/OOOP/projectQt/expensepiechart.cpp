#include "expensepiechart.h"
#include <QDebug>
#include <QPainter>
#include <QFontMetrics>
#include <cmath>

ExpensePieChart::ExpensePieChart(DatabaseManager* dbManager, int userId, QWidget* parent)
    : QWidget(parent), m_dbManager(dbManager), m_userId(userId)
{
    // Input validation
    if (!dbManager) {
        qCritical() << "DatabaseManager is NULL in ExpensePieChart constructor";
        return;
    }

    if (userId < 0) {
        qCritical() << "Invalid User ID in ExpensePieChart constructor";
        return;
    }

    // Main layout setup
    QVBoxLayout* mainLayout = parent ?
                                  qobject_cast<QVBoxLayout*>(parent->layout()) :
                                  new QVBoxLayout(this);

    if (!mainLayout) {
        mainLayout = new QVBoxLayout(this);
    }

    // Chart Views
    m_expenseChartView = new QChartView(this);
    m_expenseChartView->setRenderHint(QPainter::Antialiasing);

    m_incomeChartView = new QChartView(this);
    m_incomeChartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->addWidget(m_expenseChartView);
    chartsLayout->addWidget(m_incomeChartView);

    mainLayout->addLayout(chartsLayout);

    // Legend
    m_legendWidget = new QWidget(this);
    m_legendLayout = new QHBoxLayout(m_legendWidget);
    m_legendWidget->setLayout(m_legendLayout);
    mainLayout->addWidget(m_legendWidget);

    // Initialize color palette
    resetColorPalette();

    // Update the chart
    updateChart();
}

void ExpensePieChart::resetColorPalette() {
    // Base color palette with distinct colors
    m_baseColors = {
        QColor(31, 119, 180),   // Blue
        QColor(255, 127, 14),   // Orange
        QColor(44, 160, 44),    // Green
        QColor(214, 39, 40),    // Red
        QColor(148, 103, 189),  // Purple
        QColor(140, 86, 75),    // Brown
        QColor(227, 119, 194),  // Pink
        QColor(127, 127, 127),  // Gray
        QColor(188, 189, 34),   // Olive
        QColor(23, 190, 207),   // Cyan
        QColor(248, 156, 116),  // Salmon
        QColor(100, 149, 237),  // Cornflower Blue
        QColor(64, 224, 208),   // Turquoise
        QColor(255, 69, 0),     // Red-Orange
        QColor(32, 178, 170)    // Light Sea Green
    };

    // Reset available colors to the full base palette
    m_availableColors = m_baseColors;

    // Clear existing category colors
    m_categoryColors.clear();
}

void ExpensePieChart::updateChart() {
    // Clear existing legend
    QLayoutItem* item;
    while ((item = m_legendLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            m_legendLayout->removeWidget(widget);
            delete widget;
        }
        delete item;
    }

    // Reset color palette
    resetColorPalette();

    // Prepare category data
    prepareCategoryData();

    // Create charts
    createCharts();
}

void ExpensePieChart::prepareCategoryData() {
    // Reset category data map
    m_categoryDataMap.clear();

    QDate currentDate = QDate::currentDate();
    int currentYear = currentDate.year();
    QString currentMonth = currentDate.toString("MMMM");

    QJsonArray transactions = m_dbManager->getUserTransactions(m_userId);

    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();

        // Skip empty transactions
        if (transaction.isEmpty()) {
            qWarning() << "Empty transaction object" << transaction;
            continue;
        }

        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        // Check if transaction is in the current month and year
        if (transactionDate.date().year() == currentYear &&
            transactionDate.toString("MMMM") == currentMonth) {

            QString category = transaction["category"].toString();
            double amount = transaction["amount"].toDouble();
            bool isExpense = transaction["is_expense"].toBool();

            // Update category data
            if (isExpense) {
                m_categoryDataMap[category].expenseAmount += amount;
            } else {
                m_categoryDataMap[category].incomeAmount += amount;
            }
        }
    }
}

QString formatCategoryName(const QString& categoryName) {
    int maxLength = 10;
    if (categoryName.length() <= maxLength) {
        return categoryName;
    } else {
        return categoryName.left(maxLength - 3) + "...";
    }
}

QColor ExpensePieChart::getColorForCategory(const QString& category) {
    // If category already has a color, return it
    if (m_categoryColors.contains(category)) {
        return m_categoryColors[category];
    }

    // If no colors available, reset the palette
    if (m_availableColors.isEmpty()) {
        resetColorPalette();
    }

    // Select and remove a color from available colors
    QColor selectedColor = m_availableColors.takeFirst();
    m_categoryColors[category] = selectedColor;

    return selectedColor;
}

QWidget* ExpensePieChart::createLegendItem(const QString& category, const QColor& color) {
    QWidget* itemWidget = new QWidget(this);
    QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(5, 2, 5, 2);

    QString shortCategory = formatCategoryName(category);

    // Color indicator
    QLabel* colorIndicator = new QLabel(this);
    colorIndicator->setFixedSize(10, 10);
    colorIndicator->setStyleSheet(QString("background-color: %1; border-radius: 7px;").arg(color.name()));

    // Category label
    QLabel* categoryLabel = new QLabel(shortCategory, this);
    categoryLabel->setWordWrap(false);
    categoryLabel->setMaximumWidth(100);

    itemLayout->addWidget(colorIndicator);
    itemLayout->addWidget(categoryLabel);
    itemLayout->addStretch();

    return itemWidget;
}

void ExpensePieChart::createCharts() {
    // If no data, create empty charts
    if (m_categoryDataMap.isEmpty()) {
        QPieSeries* emptyExpenseSeries = new QPieSeries();
        emptyExpenseSeries->append("No Data", 1);

        QChart* expenseChart = new QChart();
        expenseChart->addSeries(emptyExpenseSeries);
        expenseChart->setTitle("Expenses (No Data)");
        expenseChart->legend()->setVisible(false);

        m_expenseChartView->setChart(expenseChart);

        QPieSeries* emptyIncomeSeries = new QPieSeries();
        emptyIncomeSeries->append("No Data", 1);

        QChart* incomeChart = new QChart();
        incomeChart->addSeries(emptyIncomeSeries);
        incomeChart->setTitle("Income (No Data)");
        incomeChart->legend()->setVisible(false);

        m_incomeChartView->setChart(incomeChart);

        return;
    }

    // Create pie series for expenses and income
    QPieSeries* expenseSeries = new QPieSeries();
    QPieSeries* incomeSeries = new QPieSeries();

    expenseSeries->setHoleSize(0.4);
    incomeSeries->setHoleSize(0.4);

    // Create legend items
    for (auto it = m_categoryDataMap.begin(); it != m_categoryDataMap.end(); ++it) {
        const QString& category = it.key();
        const CategoryData& data = it.value();

        QColor categoryColor = getColorForCategory(category);

        // Add slices to respective series if amounts are positive
        if (data.expenseAmount > 0) {
            QPieSlice* expenseSlice = expenseSeries->append(category, data.expenseAmount);
            expenseSlice->setColor(categoryColor);
            expenseSlice->setLabelVisible(false);
        }

        if (data.incomeAmount > 0) {
            QPieSlice* incomeSlice = incomeSeries->append(category, data.incomeAmount);
            incomeSlice->setColor(categoryColor);
            incomeSlice->setLabelVisible(false);
        }

        // Create legend item
        QWidget* legendItem = createLegendItem(category, categoryColor);
        m_legendLayout->addWidget(legendItem);
    }

    // Create charts
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(13);

    QChart* expenseChart = new QChart();
    expenseChart->addSeries(expenseSeries);
    expenseChart->setTitle("Expenses");
    expenseChart->setTitleFont(titleFont);
    expenseChart->legend()->setVisible(false);

    QChart* incomeChart = new QChart();
    incomeChart->addSeries(incomeSeries);
    incomeChart->setTitle("Income");
    incomeChart->setTitleFont(titleFont);
    incomeChart->legend()->setVisible(false);

    m_expenseChartView->setChart(expenseChart);
    m_incomeChartView->setChart(incomeChart);
}
