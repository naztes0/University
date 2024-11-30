#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"
#include <QMessageBox>
#include <QPushButton>

AddTransactionDialog::AddTransactionDialog(DatabaseManager*dbManager,int userId,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTransactionDialog)
    ,manager(dbManager)
    ,userId(userId)
    ,m_transactionId("")

{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->disconnect();
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),&QPushButton::clicked,
            this,&AddTransactionDialog::validateAndAccept);
    setWindowTitle("Add Transaction");
    ui->transactionDetails->setTitle("Transaction Details");
    //current date
    ui->dateEdit->setDate(QDate::currentDate());
    initializeCategories();
    //as default:expense
    ui->expenseRadio->setChecked(true);

    QPushButton*addCategoryButton=new QPushButton("Add category", this);
    QPushButton* deleteCategoryButton=new QPushButton("Delete category", this);
    connect(addCategoryButton,&QPushButton::clicked,this,&AddTransactionDialog::onAddCategoryClicked);
    connect(deleteCategoryButton,&QPushButton::clicked,this,&AddTransactionDialog::onDeleteCategoryClicked);
}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}

void AddTransactionDialog::initializeCategories(){
    ui->categoriesComboBox->clear();
    ui->categoriesComboBox->addItem("...");
    ui->categoriesComboBox->addItem("Food");
    ui->categoriesComboBox->addItem("Transport");
    ui->categoriesComboBox->addItem("Entertainment");
    ui->categoriesComboBox->addItem("Salary");
    ui->categoriesComboBox->addItem("Investment");

    QJsonArray userCategories=manager->getUserCategories(userId);
    for (const QJsonValue&categoryValue:userCategories){
        QJsonObject category=categoryValue.toObject();
        ui->categoriesComboBox->addItem(category["name"].toString());
    }
}

// Новий метод для налаштування діалогу при редагуванні
void AddTransactionDialog::setTransactionData(const QJsonObject& transaction)
{
    m_transactionId = transaction["id"].toString();
    m_originalDateTime=QDateTime::fromString(transaction["transaction_date"].toString(),Qt::ISODate);

    ui->sumLineEdit->setText(QString::number(transaction["amount"].toDouble()));
    ui->categoriesComboBox->setCurrentText(transaction["category"].toString());
    ui->commentLineEdit->setText(transaction["comment"].toString());


    QDateTime transactionDate = QDateTime::fromString(transaction["transaction_date"].toString(), Qt::ISODate);
    ui->dateEdit->setDate(m_originalDateTime.date());


    bool isExpense = transaction["is_expense"].toBool();
    ui->expenseRadio->setChecked(isExpense);
    ui->incomeRadio->setChecked(!isExpense);
    setWindowTitle("Edit Transaction");
    setWindowIcon(QIcon(":/img/img/rotate-square.svg"));
}
Transaction*AddTransactionDialog::createTransaction(){
    double amount=ui->sumLineEdit->text().toDouble();
    QString category=ui->categoriesComboBox->currentText();
    QString description=ui->commentLineEdit->text();
    QDate date=ui->dateEdit->date();

    Transaction*transaction;
    if(ui->incomeRadio->isChecked()){
        transaction=new IncomeTransaction(0,amount,category,description,date);
    }
    else{
        transaction=new ExpenseTransaction(0,amount,category,description,date);
    }
    return transaction;
}

void AddTransactionDialog::validateAndAccept()
{
    // Попередня валідація (як було раніше)
    if(ui->sumLineEdit->text().isEmpty()){
        QMessageBox::warning(this, "Warning", "Please enter the amount");
        return;
    }

    bool ok;
    double amount = ui->sumLineEdit->text().toDouble(&ok);
    QString category = ui->categoriesComboBox->currentText();
    if(category == "..."){
        QMessageBox::warning(this, "Warning", "Please choose category");
        return;
    }
    if(!ok || amount <= 0){
        QMessageBox::warning(this, "Warning", "Please enter a valid positive amount");
        return;
    }

    bool isExpense = ui->expenseRadio->isChecked();
    QString comment = ui->commentLineEdit->text();
    QDateTime dateTimeAdd(ui->dateEdit->date(),QTime::currentTime());
    QDateTime dateTimeUpd(ui->dateEdit->date(), m_originalDateTime.time());

    // Перевірка, чи це редагування існуючої транзакції
    if(!m_transactionId.isEmpty()){
        // Оновлення існуючої транзакції
        if(manager->updateTransaction(m_transactionId, isExpense, category, amount, dateTimeUpd, comment)){
            emit transactionAdded();
            accept();
        }
        else{
            QMessageBox::warning(this, "Error", "Failed to update transaction");
        }
    }
    else{
        // Додавання нової транзакції (як було раніше)
        if(manager->addTransaction(userId, isExpense, category, amount, dateTimeAdd, comment)){
            emit transactionAdded();
            accept();
        }
        else{
            QMessageBox::warning(this, "Error", "Failed to add transaction");
        }
    }
}

void AddTransactionDialog::on_buttonBox_rejected(){
    reject();
}

//Methods to work with categories
void AddTransactionDialog::onAddCategoryClicked(){
    QString categoryName=QInputDialog::getText(this,"Add category","Enter new category name:");
    if(!categoryName.isEmpty()){
        if(manager->addUserCategory(userId,categoryName));
        initializeCategories();
    }
    else{
        QMessageBox::warning(this,"Error","Failed to add category or category already exists");
    }
}

void AddTransactionDialog::onDeleteCategoryClicked(){
    QJsonArray userCategories=manager->getUserCategories(userId);
    QStringList categoryNames;
    for (const QJsonValue& categoryValue: userCategories){
        QJsonObject category=categoryValue.toObject();
        categoryNames<<category["name"].toString();
    }
    bool ok;
    QString categoryToDelete = QInputDialog::getItem(
        this, "Delete Category", "Select category to delete:",
        categoryNames, 0, false, &ok
        );

    if (ok && !categoryToDelete.isEmpty()) {
        if (manager->deleteUserCategory(userId, categoryToDelete)) {
            // Оновити список категорій
            initializeCategories();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete category");
        }
    }
}
