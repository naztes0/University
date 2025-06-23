#ifndef CATEGORYWIDGETCACHE_H
#define CATEGORYWIDGETCACHE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include <QString>
#include <QPixmap>
#include"homewidget.h"

class CategoryWidgetCache {
public:
    // Singleton instance accessor
    static CategoryWidgetCache& getInstance();

    // Get or create a category widget
    QWidget* getCategoryWidget(const QString& category, double expenses, QWidget* parent);

    // Prevent copying of singleton
    CategoryWidgetCache(const CategoryWidgetCache&) = delete;
    CategoryWidgetCache& operator=(const CategoryWidgetCache&) = delete;

private:
    // Private constructor for singleton
    CategoryWidgetCache() = default;

    // Cached widgets map
    QMap<QString, QWidget*> m_cachedWidgets;

    // Create a new optimized category widget
    QWidget* createOptimizedCategoryWidget(const QString& category, double expenses, QWidget* parent);

    // Update category icon
    void updateCategoryIcon(QLabel* iconLabel, const QString& category);

    // Update expenses label
    void updateExpensesLabel(QLabel* expensesLabel, double expenses);

    // Update widget expenses
    void updateWidgetExpenses(QWidget* container, double expenses);

    // Get icon path for a category
    QString getIconPathForCategory(const QString& category);
};

#endif // CATEGORYWIDGETCACHE_H
