#include "categorywidgetcache.h"
#include <QDebug>

CategoryWidgetCache& CategoryWidgetCache::getInstance() {
    static CategoryWidgetCache instance;
    return instance;
}

QWidget* CategoryWidgetCache::getCategoryWidget(const QString& category, double expenses, QWidget* parent) {
    // Check if widget exists in cache
    auto it = m_cachedWidgets.find(category);
    if (m_cachedWidgets.isEmpty()) {
        qDebug() << "No categories found in categoryCache!";
        return;
    }

    if (it != m_cachedWidgets.end()) {
        // Update expenses and return cached widget
        updateWidgetExpenses(it.value(), expenses);
        return it.value();
    }

    // Create new widget if not in cache
    QWidget* widget = createOptimizedCategoryWidget(category, expenses, parent);
    m_cachedWidgets[category] = widget;
    return widget;
}

QWidget* CategoryWidgetCache::createOptimizedCategoryWidget(const QString& category, double expenses, QWidget* parent) {
    QWidget* container = new QWidget(parent);
    container->setObjectName("categoryContainer_" + category);

    // Use static layout to reduce allocation overhead
    static QVBoxLayout* staticLayout = nullptr;
    if (!staticLayout) {
        staticLayout = new QVBoxLayout();
        staticLayout->setAlignment(Qt::AlignCenter);
        staticLayout->setSpacing(5);
        staticLayout->setContentsMargins(5, 5, 5, 5);
    }
    container->setLayout(staticLayout);

    // Reuse labels with object names for identification
    QLabel* iconLabel = container->findChild<QLabel*>("categoryIcon");
    if (!iconLabel) {
        iconLabel = new QLabel(container);
        iconLabel->setObjectName("categoryIcon");
        staticLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    }
    updateCategoryIcon(iconLabel, category);

    QLabel* nameLabel = container->findChild<QLabel*>("categoryName");
    if (!nameLabel) {
        nameLabel = new QLabel(container);
        nameLabel->setObjectName("categoryName");
        nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
        staticLayout->addWidget(nameLabel, 0, Qt::AlignCenter);
    }
    nameLabel->setText(category);

    QLabel* expensesLabel = container->findChild<QLabel*>("categoryExpenses");
    if (!expensesLabel) {
        expensesLabel = new QLabel(container);
        expensesLabel->setObjectName("categoryExpenses");
        staticLayout->addWidget(expensesLabel, 0, Qt::AlignCenter);
    }
    updateExpensesLabel(expensesLabel, expenses);

    QPushButton* detailsButton = container->findChild<QPushButton*>("categoryDetailsButton");
    if (!detailsButton) {
        detailsButton = new QPushButton("Details", container);
        detailsButton->setObjectName("categoryDetailsButton");
        detailsButton->setProperty("categoryName", category);
        staticLayout->addWidget(detailsButton);
        HomeWidget* homeWidget = qobject_cast<HomeWidget*>(parent);
        if (homeWidget) {
            connect(detailsButton, &QPushButton::clicked, homeWidget, &HomeWidget::onCategoryButtonClicked);
        }
    }

    return container;
}

void CategoryWidgetCache::updateCategoryIcon(QLabel* iconLabel, const QString& category) {
    static QMap<QString, QPixmap> iconCache;

    // Check icon cache
    if (!iconCache.contains(category)) {
        QString iconPath = getIconPathForCategory(category);
        QPixmap pixmap(iconPath);

        if (pixmap.isNull()) {
            qWarning() << "Icon not found for category:" << category << ", using default.";
            pixmap.load(getIconPathForCategory("Default"));
        }

        iconCache[category] = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    iconLabel->setPixmap(iconCache[category]);
}

void CategoryWidgetCache::updateExpensesLabel(QLabel* expensesLabel, double expenses) {
    QString expensesText = QString::number(expenses, 'f', 2) + " UAH";
    QString expensesStyle;

    if (expenses > 0) {
        expensesStyle = "color: green; font-size: 10px;";
    } else if (expenses < 0) {
        expensesStyle = "color: red; font-size: 10px;";
    } else {
        expensesText = "0.00";
        expensesStyle = "color: gray; font-size: 10px;";
    }

    expensesLabel->setText(expensesText);
    expensesLabel->setStyleSheet(expensesStyle);
}

void CategoryWidgetCache::updateWidgetExpenses(QWidget* container, double expenses) {
    QLabel* expensesLabel = container->findChild<QLabel*>("categoryExpenses");
    if (expensesLabel) {
        updateExpensesLabel(expensesLabel, expenses);
    }
}

QString CategoryWidgetCache::getIconPathForCategory(const QString& category) {
    static const QMap<QString, QString> categoryIcons = {
        {"Food", ":/img/img/food.svg"},
        {"Transport", ":/img/img/transport.svg"},
        {"Entertainment", ":/img/img/entertainment.svg"},
        {"Investment", ":/img/img/investment.png"},
        {"Salary", ":/img/img/salary.png"},
        {"Default", ":/img/img/receipt.svg"}
    };

    return categoryIcons.value(category, categoryIcons["Default"]);
}
