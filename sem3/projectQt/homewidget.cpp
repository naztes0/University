#include "homewidget.h"
#include "ui_homewidget.h"
#include <QDebug>

HomeWidget::HomeWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
    , m_dbManager(nullptr)
    , m_userId(-1)
{
    ui->setupUi(this);
    ui->addTrPushButton->setText("Add Transaction");
    QScrollArea* categoriesScrollArea = ui->scrollArea;
    QWidget* scrollContent = new QWidget();
    m_categoriesLayout = new QGridLayout(scrollContent);
    m_categoriesLayout->setAlignment(Qt::AlignTop);

    categoriesScrollArea->setWidget(scrollContent);
    categoriesScrollArea->setWidgetResizable(true);
    ui->totalExpense->setStyleSheet("font-weight: bold; font-size: 15px;");
    ui->totalIncome->setStyleSheet("font-weight: bold; font-size: 15px;");

}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::initialize(DatabaseManager *dbManager, int userId){
    m_dbManager=dbManager;
    m_userId=userId;
    loadUserCategories();
    updateMonthlyTotals();
    createPieCharts();
}

void HomeWidget:: on_addTrPushButton_clicked(){
    AddTransactionDialog dialog(m_dbManager, m_userId, this);
    connect(&dialog,&AddTransactionDialog::transactionAdded,this,&HomeWidget::onTransactionAdded);
    dialog.exec();
}
void HomeWidget::onTransactionAdded()
{   qDebug() << "Slot onTransactionAdded called.";
    QJsonArray transactions = m_dbManager->getUserTransactions(m_userId);
    qDebug() << "Loaded transactions:" << transactions;
    loadUserCategories();
    updateMonthlyTotals();
    createPieCharts();

}

//Methods to work with space of categories
void HomeWidget::setupCategoriesWidget(){
    QScrollArea* categoriesScrollArea=ui->scrollArea;
    QWidget* scrollContent=new QWidget();
    m_categoriesLayout=new QGridLayout(scrollContent);
    m_categoriesLayout->setAlignment(Qt::AlignTop);

    categoriesScrollArea->setWidget(scrollContent);
    categoriesScrollArea->setWidgetResizable(true);
    loadUserCategories();
}

void HomeWidget::loadUserCategories()
{
    // Static list of default categories
    static const QStringList defaultCategories = {"Food", "Transport", "Entertainment", "Salary", "Investment"};


    // Use cashing of categories and their expenses to speed up the process of loading transactions
    static QMap<QString, double> categoryExpensesCache;
    static int lastLoadedUserId = -1;

    // Check fro cash data changing
    if (lastLoadedUserId != m_userId) {
        categoryExpensesCache.clear();
        lastLoadedUserId = m_userId;
    }

    // layout deleting
    QLayoutItem* item;
    while ((item = m_categoriesLayout->takeAt(0)) != nullptr) {
        QWidget* widget = item->widget();
        m_categoriesLayout->removeWidget(widget);
        delete widget;
        delete item;
    }

    QJsonArray categories = m_dbManager->getUserCategories(m_userId);
    if (categories.isEmpty()) return;


    // Add default categories to the list if they don't already exist
    for (const QString& defaultCategory : defaultCategories) {
        bool categoryExists = std::any_of(categories.begin(), categories.end(),
            [&defaultCategory](const QJsonValue& value) {
            return value.toObject()["name"].toString() == defaultCategory;
            });

        if (!categoryExists) {
            QJsonObject defaultCategoryObj;
            defaultCategoryObj["name"] = defaultCategory;
            categories.prepend(defaultCategoryObj);
        }
    }

    if (categories.isEmpty()) return;

    int currentYear = QDate::currentDate().year();
    QString currentMonth = QDate::currentDate().toString("MMMM");
    // Pre-processing expenses to avoid recalculations
    QMap<QString, double> categoryExpenses;
    for (const QJsonValue& categoryValue : categories) {
        QJsonObject category = categoryValue.toObject();
        QString categoryName = category["name"].toString();

        // Перевірка кешу
        if (categoryExpensesCache.contains(categoryName)) {
            categoryExpenses[categoryName] = categoryExpensesCache[categoryName];
        } else {
            double expenses = calculateCategoryExpenses(categoryName, currentYear, currentMonth);
            categoryExpenses[categoryName] = expenses;
            categoryExpensesCache[categoryName] = expenses;
        }
    }

    // Creating of categpry widgets
    QVector<QWidget*> categoryWidgets;
    categoryWidgets.reserve(categories.size());

    for (int i = 0; i < categories.size(); ++i) {
        QJsonObject category = categories[i].toObject();
        QString categoryName = category["name"].toString();

        QWidget* categoryButton = createCategoryButton(
            categoryName,
            categoryExpenses.value(categoryName, 0.0)
            );

        if (categoryButton) {
            categoryWidgets.append(categoryButton);
        }
    }

    // Adding to grid
    for (int row = 0, col = 0, i = 0; i < categoryWidgets.size(); ++i, ++col) {
        if (col > 3) {
            col = 0;
            row++;
        }
        m_categoriesLayout->addWidget(categoryWidgets[i], row, col);
    }

    qDebug() << "Categories loaded successfully";
}
double HomeWidget::calculateCategoryExpenses(const QString& category, int year, const QString& month)
{
    TransactionsList tempList(m_dbManager, m_userId);
    return tempList.calculateCategoryExpenses(category, year, month);
}
QWidget* HomeWidget::createCategoryButton(const QString& category, double expenses)
{
    if (category.isEmpty()) return nullptr;

    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(5);

    // Централізована обробка іконок
    QLabel* iconLabel = createCategoryIcon(category);
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);

    QLabel* nameLabel = new QLabel(category);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    layout->addWidget(nameLabel, 0, Qt::AlignCenter);

    QString expensesText=QString::number(expenses, 'f', 2);;
    QString expensesStyle;

    if (expenses > 0) {
        expensesStyle = "color: green; font-size: 10px;";
    } else if (expenses < 0) {
        expensesStyle = "color: red; font-size: 10px;";
    } else {
        expensesText = "0.00";
        expensesStyle = "color: gray; font-size: 10px;";
    }

    QLabel* expensesLabel = new QLabel(expensesText+" UAH");
    expensesLabel->setStyleSheet(expensesStyle);
    layout->addWidget(expensesLabel, 0, Qt::AlignCenter);

    QPushButton* button = new QPushButton("Details");
    button->setProperty("categoryName", category);
    connect(button, &QPushButton::clicked, this, &HomeWidget::onCategoryButtonClicked);

    layout->addWidget(button);


    return container;
}

QLabel* HomeWidget::createCategoryIcon(const QString& category)
{
    QLabel* iconLabel = new QLabel(this);
    QString iconPath = getIconPathForCategory(category);

    QPixmap pixmap(iconPath);

    if (pixmap.isNull()) {
        qWarning() << "Icon not found for category:" << category << ", using default.";
        pixmap.load(getIconPathForCategory("Default"));
    }

    iconLabel->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    return iconLabel;
}

QString HomeWidget::getIconPathForCategory(const QString& category)
{
    static const QMap<QString, QString> categoryIcons = {
        {"Food", ":/img/img/food.svg"},
        {"Transport", ":/img/img/transport.svg"},
        {"Entertainment", ":/img/img/entertainment.svg"},
        {"Investment", ":/img/img/investment.png"},
        {"Salary", ":/img/img/salary.png"},
        {"Default", ":/img/img/receipt.svg"},
        {"Other", ":/img/img/rectangle-list.png"}
    };

    return categoryIcons.value(category, categoryIcons["Default"]);
}

void HomeWidget::onCategoryButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString category = button->property("categoryName").toString();
    if (category.isEmpty()) return;

    int currentYear = QDate::currentDate().year();
    QString currentMonth = QDate::currentDate().toString("MMMM");


    QDialog* categoryDetailsDialog = new QDialog(this);
    categoryDetailsDialog->setWindowTitle(category + " Transactions");
    categoryDetailsDialog->setMinimumSize(600, 400);

    QVBoxLayout* dialogLayout = new QVBoxLayout(categoryDetailsDialog);

    // Creating QTableWidget for transactions
    QTableWidget* transactionsTable = new QTableWidget();
    transactionsTable->setColumnCount(3);
    transactionsTable->setHorizontalHeaderLabels({"Date", "Amount", "Comment"});
    transactionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //Only read Mode
    transactionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);


    QJsonArray transactions = m_dbManager->getUserTransactions(m_userId);

    // Фільтрація транзакцій
    QVector<QJsonObject> filteredTransactions;
    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        if (transaction["category"].toString() == category &&
            transactionDate.date().year() == currentYear &&
            transactionDate.toString("MMMM") == currentMonth) {
            filteredTransactions.append(transaction);
        }
    }

    // Settings table
    transactionsTable->setRowCount(filteredTransactions.size());
    for (int row = 0; row < filteredTransactions.size(); ++row) {
        const QJsonObject& transaction = filteredTransactions[row];

        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);
        double amount = transaction["amount"].toDouble();
        bool isExpense = transaction["is_expense"].toBool();

        // date
        QTableWidgetItem* dateItem = new QTableWidgetItem(transactionDate.toString("dd.MM.yyyy hh:mm"));
        transactionsTable->setItem(row, 0, dateItem);

        // summary
        QTableWidgetItem* amountItem = new QTableWidgetItem(QString("%1%2").arg(isExpense ? "-" : "+", QString::number(amount, 'f', 2)+ "UAH"));
        amountItem->setForeground(isExpense ? Qt::red : Qt::darkGreen);
        transactionsTable->setItem(row, 1, amountItem);

        // comment
        QTableWidgetItem* commentItem = new QTableWidgetItem(transaction["comment"].toString());
        transactionsTable->setItem(row, 2, commentItem);
    }

    dialogLayout->addWidget(transactionsTable);

    // close button
    QPushButton* closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, categoryDetailsDialog, &QDialog::accept);
    dialogLayout->addWidget(closeButton);

    // setup of layour
    categoryDetailsDialog->setLayout(dialogLayout);
    categoryDetailsDialog->exec();
}

void HomeWidget::refreshHomeCategories(){
    loadUserCategories();
}

void HomeWidget::updateMonthlyTotals() {
    QDate currentDate = QDate::currentDate();
    int currentYear = currentDate.year();
    QString currentMonth = currentDate.toString("MMMM");

    TransactionsList tempList(m_dbManager,m_userId);
    double totalExpenses = tempList.calculateTotalMonthlyExpenses(currentYear, currentMonth);
    ui->expAmount->setText(QString::number(totalExpenses,'f',2)+" UAH");
    ui->expAmount->setStyleSheet("font-weight: bold; font-size: 15px; color: red;");


    double totalIncomings = tempList.calculateTotalMonthlyIncomings(currentYear, currentMonth);
    ui->incAmount->setText(QString::number(totalIncomings,'f',2)+" UAH");
    ui->incAmount->setStyleSheet("font-weight: bold; font-size: 15px; color: green;");
    qDebug()<<"Total +";
}

void HomeWidget::createPieCharts() {
    qDebug() << "createPieCharts called";

    // Детальна перевірка UI компонентів
    if (!ui) {
        qCritical() << "UI is not initialized!";
        return;
    }

    QWidget* chartContainer = ui->widget_2;
    if (!chartContainer) {
        qCritical() << "Chart container widget_2 is NULL!";
        return;
    }

    // Перевірка менеджера бази даних
    if (!m_dbManager) {
        qCritical() << "DatabaseManager is NULL!";
        return;
    }

    // Перевірка user ID


    // Безпечне створення layout
    QLayout* existingLayout = chartContainer->layout();
    if (!existingLayout) {
        existingLayout = new QVBoxLayout(chartContainer);
        qDebug() << "Created new layout for chart container";
    }

    // Очищення попереднього вмісту
    QLayoutItem* item;
    while ((item = existingLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            existingLayout->removeWidget(widget);
            delete widget;
        }
        delete item;
    }

    // Створення графіка
    ExpensePieChart* pieChart = new ExpensePieChart(m_dbManager, m_userId);
    existingLayout->addWidget(pieChart);
    qDebug() << "Pie chart created successfully.";
}

void HomeWidget::pieChartscreation(){
    createPieCharts();
}
