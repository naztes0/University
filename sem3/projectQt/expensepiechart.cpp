#include "expensepiechart.h"

ExpensePieChart::ExpensePieChart(DatabaseManager*dbManager, int userId, QWidget* parent)
    :QWidget(parent), m_dbManager(dbManager), m_userId(userId)
{
    // Жорстка перевірка вхідних параметрів
    if (!dbManager) {
        qCritical() << "DatabaseManager is NULL in ExpensePieChart constructor";
        return;
    }

    if (userId < 0) {
        qCritical() << "Invalid User ID in ExpensePieChart constructor";
        return;
    }

    QVBoxLayout* mainLayout = parent ?
                                  qobject_cast<QVBoxLayout*>(parent->layout()) :
                                  new QVBoxLayout(this);

    if (!mainLayout) {
        mainLayout = new QVBoxLayout(this);
    }

    // Chart View
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


    // Color Palette (Expanded)
    m_colors = {
        QColor(0, 136, 254),
        QColor(0, 196, 159),
        QColor(255, 187, 40),
        QColor(255, 128, 66),
        QColor(136, 132, 216),
        QColor(255, 72, 66),
        QColor(46, 204, 113),
        QColor(52, 152, 219),
        QColor(241, 196, 15)
    };

    // Update the chart
    updateChart();
    qDebug() << "Creating ExpensePieChart - END";
}

void ExpensePieChart::updateChart() {
    qDebug() << "UpdateChart - START";
    QLayoutItem* item;
    while ((item = m_legendLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            m_legendLayout->removeWidget(widget);
            delete widget;
        }
        delete item;
    }

    QMap<QString, double> expensesData = calculateCategoryData(true);
    QMap<QString, double> incomeData = calculateCategoryData(false);

    qDebug() << "Expenses data size:" << expensesData.size();
    qDebug() << "Income data size:" << incomeData.size();

    createChart(m_expenseChartView, expensesData, true);
    createChart(m_incomeChartView, incomeData, false);

    qDebug() << "UpdateChart - END";
}

QString formatCategoryName(const QString& categoryName) {
    int maxLength=15;
    if (categoryName.length() <=maxLength) {
        return categoryName;
    } else {
        return categoryName.left(maxLength - 3) + "...";
    }
}

QWidget* ExpensePieChart::createLegendItem(const QString& category, const QColor& color, bool isExpense) {
    QWidget* itemWidget = new QWidget(this);
    QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(5, 2, 5, 2);

    QString shortCategory = formatCategoryName(category);

    QLabel* colorIndicator = new QLabel(this);
    colorIndicator->setFixedSize(10, 10);
    colorIndicator->setStyleSheet(QString("background-color: %1; border-radius: 7px;").arg(color.name()));

    QLabel* categoryLabel = new QLabel(shortCategory, this);
    categoryLabel->setWordWrap(false);
    categoryLabel->setMaximumWidth(100);

    itemLayout->addWidget(colorIndicator);
    itemLayout->addWidget(categoryLabel);
    itemLayout->addStretch();

    return itemWidget;
}
QMap<QString, double> ExpensePieChart::calculateCategoryData(bool isExpense)
{
    qDebug() << "CalculateCategoryData - START" << (isExpense ? "Expenses" : "Income");

    QDate currentDate = QDate::currentDate();
    int currentYear = currentDate.year();
    QString currentMonth = currentDate.toString("MMMM");

    QJsonArray transactions = m_dbManager->getUserTransactions(m_userId);
    qDebug() << "Total transactions:" << transactions.size();

    QMap<QString, double> categoryData;

    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();

        // Додаткова перевірка транзакції
        if (transaction.isEmpty()) {
            qWarning() << "Empty transaction object"<<transaction;
            continue;
        }

        bool transactionIsExpense = transaction["is_expense"].toBool();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        if (transactionIsExpense == isExpense &&
            transactionDate.date().year() == currentYear &&
            transactionDate.toString("MMMM") == currentMonth) {

            QString category = transaction["category"].toString();
            double amount = transaction["amount"].toDouble();

            categoryData[category] += amount;
        }
    }

    qDebug() << "Calculated categories:" << categoryData.keys();

    return categoryData;
}
void ExpensePieChart::createChart(QChartView* chartView, const QMap<QString, double>& data, bool isExpense) {
    qDebug() << "CreateChart - START" << (isExpense ? "Expenses" : "Income");

    if (!chartView) {
        qCritical() << "ChartView is NULL";
        return;
    }

    if (data.isEmpty()) {
        QPieSeries* emptySeries = new QPieSeries();
        emptySeries->append("No Data", 1);

        QChart* chart = new QChart();
        chart->addSeries(emptySeries);
        chart->setTitle(isExpense ? "Expenses (No Data)" : "Income (No Data)");
        chart->legend()->setVisible(false);

        chartView->setChart(chart);
        return;
    }

    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.4);

    QVBoxLayout* legendLayout = new QVBoxLayout();
    m_legendLayout->addLayout(legendLayout);

    QHBoxLayout* currentRowLayout = new QHBoxLayout();
    int currentRowWidth = 0;
    const int MAX_ROW_WIDTH = 400;  // Максимальна ширина рядка
    int colorIndex = 0;

    QFontMetrics fontMetrics(this->font()); // Для обчислення ширини тексту

    for (auto it = data.begin(); it != data.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice* slice = series->append(it.key(), it.value());
            if (!slice) {
                qCritical() << "Failed to create slice for" << it.key();
                continue;
            }

            QColor currentColor = m_colors[colorIndex % m_colors.size()];
            slice->setColor(currentColor);
            slice->setLabelVisible(false);

            QWidget* legendItem = createLegendItem(it.key(), currentColor, isExpense);
            int itemWidth = fontMetrics.horizontalAdvance(it.key()) + 50; // Ширина тексту + відступи/іконка

            if (currentRowWidth + itemWidth > MAX_ROW_WIDTH) {
                legendLayout->addLayout(currentRowLayout);
                currentRowLayout = new QHBoxLayout();
                currentRowWidth = 0;
            }

            currentRowLayout->addWidget(legendItem);
            currentRowWidth += itemWidth;
            colorIndex++;
        }
    }

    if (currentRowLayout->count() > 0) {
        legendLayout->addLayout(currentRowLayout);
    }

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(isExpense ? "Expenses" : "Income");
    chart->legend()->setVisible(false);

    chartView->setChart(chart);
    qDebug() << "CreateChart - END";
}
