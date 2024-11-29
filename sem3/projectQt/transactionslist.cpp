#include "transactionslist.h"
#include "ui_transactionslist.h"
#include<QJsonArray>
#include<QScrollArea>
#include<QGroupBox>
#include<QHBoxLayout>
#include<QPushButton>


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

void TransactionsList::loadtransactions(){
    //clear previous transactaions
    QLayoutItem*item;
    while((item=m_mainLayout->takeAt(0))!=nullptr){
        delete item->widget();
        delete item;
    }

    //Get transactions from DB
    QJsonArray transactions= manager->getUserTransactions(m_userId);
    //Group transactions by the date
    QMap <QString,QJsonArray> transactionsByDate;

    for(const QJsonValue& transactionValue:transactions){
        QJsonObject transaction= transactionValue.toObject();
        QDateTime transactionDate= QDateTime::fromString(transaction["transaction_date"].toString(),Qt::ISODate);
        QString dateKey=transactionDate.toString("dd MMMM");
        transactionsByDate[dateKey].prepend(transaction);
    }

    QStringList sortedDates = transactionsByDate.keys();
    std::reverse(sortedDates.begin(), sortedDates.end());
     //Displaying transactions
    for (const QString& dateKey : sortedDates) {
        QLabel* dateLabel = new QLabel(dateKey);
        dateLabel->setStyleSheet("font-weight: bold; font-size: 18px;");
        m_mainLayout->addWidget(dateLabel);

        for (const QJsonValue& transactionValue : transactionsByDate[dateKey]) {
            createTransactionItem(transactionValue.toObject());
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
    QLabel*amountLabel=new QLabel(amountText);
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
        QAction*editAction=contextMenu.addAction("Edit");
        QAction*deleteAction=contextMenu.addAction("Delete");
        QAction*selectedAction=contextMenu.exec(transactionWidget->mapToGlobal(pos));

        if(selectedAction==editAction){

        }

        if(selectedAction==deleteAction){
            if(manager->deleteTransaction(transactionId)){
                loadtransactions();
            }
        }
    });
}

void TransactionsList::refreshTransactionsList(){
    loadtransactions();
}
