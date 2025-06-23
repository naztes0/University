#include "transactionslist.h"
#include "ui_transactionslist.h"
#include<QJsonArray>
#include<QScrollArea>
#include<QGroupBox>
#include<QHBoxLayout>
#include<QPushButton>
#include<QTimer>
#include"addtransactiondialog.h"


TransactionsList::TransactionsList(DatabaseManager*dbManager, int userId, QWidget *parent)
    : QWidget(parent),manager(dbManager),m_userId(userId)
    , ui(new Ui::TransactionsList)
{
    setupUI();
    loadtransactions();
}

TransactionsList::~TransactionsList()
{
    delete ui;
}

void TransactionsList::setupUI(){
    QVBoxLayout*mainLayout=new QVBoxLayout(this);

    //creating scroll-area for transactions
    QScrollArea*scrollArea=new QScrollArea();
    QWidget*scrollContent =new QWidget();
    m_mainLayout=new QVBoxLayout(scrollContent);
    m_mainLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
}



void TransactionsList::loadtransactions() {
    // Очистка попередніх транзакцій
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Отримання транзакцій з бази даних
    QJsonArray transactions = manager->getUserTransactions(m_userId);

    // Групування транзакцій за роком та місяцем
    QMap<int, QMap<QString, QJsonArray>> transactionsByYearAndMonth;

    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        int year = transactionDate.date().year();
        QString monthKey = transactionDate.toString("dd MMMM");

        transactionsByYearAndMonth[year][monthKey].prepend(transaction);
    }

    // Сортування років (від найновіших до найстаріших)
    QVector<int> sortedYears = transactionsByYearAndMonth.keys().toVector();
    std::sort(sortedYears.begin(), sortedYears.end(), std::greater<int>());

    // Відображення транзакцій
    for (int year : sortedYears) {
        // Додавання року як заголовка
        QLabel* yearLabel = new QLabel(QString::number(year));
        yearLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #757575 ; margin-top: 15px;");
        m_mainLayout->addWidget(yearLabel);

        // Отримання місяців для поточного року
        QVector<QString> sortedMonths = transactionsByYearAndMonth[year].keys().toVector();
        std::sort(sortedMonths.begin(), sortedMonths.end(), [this](const QString& date1, const QString& date2) {
            // Функція порівняння місяців з урахуванням поточного року
            QDateTime dt1 = QDateTime::fromString(date1 + " " + QString::number(QDate::currentDate().year()), "dd MMMM yyyy");
            QDateTime dt2 = QDateTime::fromString(date2 + " " + QString::number(QDate::currentDate().year()), "dd MMMM yyyy");

            // Сортування від найновіших до найстаріших
            return dt1 > dt2;
        });

        // Відображення місяців та транзакцій
        for (const QString& monthKey : sortedMonths) {
            QLabel* monthLabel = new QLabel(monthKey);
            monthLabel->setStyleSheet("font-weight: bold; font-size: 18px; margin-top: 10px;");
            m_mainLayout->addWidget(monthLabel);

            for (const QJsonValue& transactionValue : transactionsByYearAndMonth[year][monthKey]) {
                createTransactionItem(transactionValue.toObject());
            }
        }
    }
}

void TransactionsList::createTransactionItem(const QJsonObject &transaction){
    QWidget* transactionWidget= new QWidget();
    QGridLayout*layout= new QGridLayout(transactionWidget);
    layout->setContentsMargins(5,5,5,5);
    layout->setHorizontalSpacing(10);
    double amount=transaction["amount"].toDouble();
    bool isExp=transaction["is_expense"].toBool();
    QString amountText=!isExp?QString("+%1").arg(amount,0,'f',2):QString("-%1").arg(amount,0,'f',2);
    QLabel*amountLabel=new QLabel(amountText+" UAH");
    amountLabel->setStyleSheet(isExp?"color: red; font-size: 14px;":"color: green; font-size: 14px;");

    QString comment=transaction["comment"].toString();
    if(comment.length()>60){
        comment=comment.left(57)+"...";
    }

    QLabel*categoryLabel=new QLabel(transaction["category"].toString());
    QLabel* commentLabel=new QLabel(comment);
    QDateTime transactioDate= QDateTime::fromString(transaction["transaction_date"].toString(),Qt::ISODate);

    QLabel*timeLabel=new QLabel(transactioDate.toString("hh:mm"));
    layout->addWidget(amountLabel,0,0,1,1);
    layout->addWidget(categoryLabel,0,1,1,1);
    layout->addWidget(commentLabel,0,2,1,3);
    layout->addWidget(timeLabel,0,5,1,1);

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 4);
    layout->setColumnStretch(5, 1);

    QString transactionId=transaction["id"].toString();
    setupContextMenu(transactionWidget,transactionId);

    m_mainLayout->addWidget(transactionWidget);

}

void TransactionsList::setupContextMenu(QWidget *transactionWidget, const QString& transactionId){
    transactionWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(transactionWidget,&QWidget::customContextMenuRequested,[this,transactionWidget,transactionId](const QPoint& pos)
        {
        QMenu contextMenu(tr("Transaction Actions"));
        QAction*editAction=contextMenu.addAction(QIcon(":/img/img/pencil.svg"), "Edit");
        QAction*deleteAction=contextMenu.addAction(QIcon(":/img/img/trash.svg"),"Delete");
        QAction*selectedAction=contextMenu.exec(transactionWidget->mapToGlobal(pos));

        if(selectedAction==editAction){
            QJsonObject transaction = manager->getTransactionById(transactionId);

            AddTransactionDialog* editDialog = new AddTransactionDialog(manager, m_userId, this);
            editDialog->setTransactionData(transaction);

            connect(editDialog, &AddTransactionDialog::transactionAdded, this, &TransactionsList::refreshTransactionsList);
            editDialog->exec();
        }

        if(selectedAction==deleteAction){
            QTimer::singleShot(100,this,[this](){
                loadtransactions();
            });
            manager->deleteTransaction(transactionId);
        }
    });
}

void TransactionsList::refreshTransactionsList(){
    loadtransactions();
}

//Methods to work with cateogries lists of transactions
void TransactionsList::loadTransactionsByCategory(const QString& category, int year, const QString& month) {
    // Очистка попередніх транзакцій
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Отримання транзакцій з бази даних
    QJsonArray transactions = manager->getUserTransactions(m_userId);

    // Заголовок з назвою категорії та місяця
    QLabel* titleLabel = new QLabel(QString("%1 - %2 %3").arg(category, month, QString::number(year)));
    titleLabel->setStyleSheet("font-weight: bold; font-size: 18px; margin-bottom: 15px;");
    m_mainLayout->addWidget(titleLabel);

    // Фільтрація та відображення транзакцій
    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        // Перевірка відповідності категорії, року та місяця
        if (transaction["category"].toString() == category &&
            transactionDate.date().year() == year &&
            transactionDate.toString("dd MMMM") == month) {
            createTransactionItem(transaction);
        }
    }
}

double TransactionsList::calculateCategoryExpenses(const QString& category, int year, const QString& month) {
    double totalExpenses = 0.0;
    QJsonArray transactions = manager->getUserTransactions(m_userId);
    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        // Check if the transaction matches the specified category, year, and month
        if (transaction["category"].toString() == category &&
            transactionDate.date().year() == year &&
            transactionDate.toString("MMMM") == month) {

            double amount = transaction["amount"].toDouble();
            if (transaction["is_expense"].toBool()) {
                totalExpenses -= amount;
            } else {
                totalExpenses += amount;
            }
        }
    }
    qDebug() << category << ": " << totalExpenses;
    return totalExpenses;
}

double TransactionsList::calculateTotalMonthlyExpenses(int year, const QString& month) {
    double totalExpenses = 0.0;
    QJsonArray transactions = manager->getUserTransactions(m_userId);

    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        // Check if the transaction is an expense and matches the specified year and month
        if (transactionDate.date().year() == year &&
            transactionDate.toString("MMMM") == month &&
            transaction["is_expense"].toBool()) {

            double amount = transaction["amount"].toDouble();
            totalExpenses -= amount;
        }
    }

    return totalExpenses;
}
double TransactionsList::calculateTotalMonthlyIncomings(int year, const QString& month) {
    double totalIncomings = 0.0;
    QJsonArray transactions = manager->getUserTransactions(m_userId);

    for (const QJsonValue& transactionValue : transactions) {
        QJsonObject transaction = transactionValue.toObject();
        QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);

        // Check if the transaction is not an expense and matches the specified year and month
        if (transactionDate.date().year() == year &&
            transactionDate.toString("MMMM") == month &&
            !transaction["is_expense"].toBool()) {

            double amount = transaction["amount"].toDouble();
            totalIncomings += amount;
        }
    }

    return totalIncomings;
}
