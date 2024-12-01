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

    delete ui->categoriesComboBox;
    setupCustomCategoryComboBox();

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

}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}

void AddTransactionDialog::setupCustomCategoryComboBox(){
    m_categoriesComboBox= new CustomCategoryComboBox(ui->transactionDetails);
    m_categoriesComboBox->setDBManager(manager,userId);
    //Replace old combo box
    ui->gridLayout->removeWidget(ui->categoriesComboBox);
    m_categoriesComboBox->setGeometry(23, 40, 321, 26);
    // Додаємо новий m_categoriesComboBox у той самий layout
    ui->gridLayout->addWidget(m_categoriesComboBox, 0, 3, 1, 1);

    //connect signals
    connect(m_categoriesComboBox, &CustomCategoryComboBox::addCategoryRequested,
            this, &AddTransactionDialog::onAddCategory);
    connect(m_categoriesComboBox, &CustomCategoryComboBox::deleteCategoryRequested,
            this, &AddTransactionDialog::onDeleteCategory);

    //initialize of categories
    initializeCategories();
}

void AddTransactionDialog::initializeCategories(){
    //clear existing items
    m_categoriesComboBox->clear();
    //add default placeholder
    m_categoriesComboBox->addItem("...");
    //Add predefined categories
    QStringList defaultCategories={"Food", "Transport", "Entertainment", "Salary", "Investment"};
    m_categoriesComboBox->addItems(defaultCategories);

    //Add user custom categories
    QJsonArray userCategories=manager->getUserCategories(userId);
    for(const QJsonValue& categoryValue:userCategories){
        QJsonObject category=categoryValue.toObject();
        m_categoriesComboBox->addItem(category["name"].toString());
    }

}
//Methods to work with categories
void AddTransactionDialog::onAddCategory() {
    QString categoryName = QInputDialog::getText(this, "Add Category", "Enter new category name:");
    if (!categoryName.isEmpty()) {
        if (manager->addUserCategory(userId, categoryName)) {
            initializeCategories();
            // Select the newly added category
            m_categoriesComboBox->setCurrentText(categoryName);
        } else {
            QMessageBox::warning(this, "Error", "Failed to add category or category already exists");
        }
    }
}

void AddTransactionDialog::onDeleteCategory(const QString &category) {
    // Prevent deleting default categories
    QStringList protectedCategories = {"Food", "Transport", "Entertainment", "Salary", "Investment"};
    if (protectedCategories.contains(category)) {
        QMessageBox::warning(this, "Error", "Cannot delete default category");
        return;
    }
    // Confirm deletion
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Category",
                                                              QString("Are you sure you want to delete the category '%1'? Transactions refered to this category will be moved to category \"Other\".").arg(category),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (manager->deleteUserCategory(userId, category)) {
            initializeCategories();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete category");
        }
    }
}

// New methods to setup dialog window for transac editing
void AddTransactionDialog::setTransactionData(const QJsonObject& transaction)
{
    m_transactionId = transaction["id"].toString();
    m_originalDateTime=QDateTime::fromString(transaction["transaction_date"].toString(),Qt::ISODate);

    ui->sumLineEdit->setText(QString::number(transaction["amount"].toDouble()));
    m_categoriesComboBox->setCurrentText(transaction["category"].toString());
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
    QString category=m_categoriesComboBox->currentText();
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
    QString category = m_categoriesComboBox->currentText();
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


