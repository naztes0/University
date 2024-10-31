#include "databasemanager.h"

DatabaseManager::DatabaseManager() {
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("finance_manager");
    db.setUserName("root");
    db.setPassword("qawsujikcvb2850147!");
}


bool DatabaseManager::connectToDatabase(){
    if(!db.open()){
        qDebug()<<"Database error:"<<db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::initializeDatabase(){
    if(!connectToDatabase()){
        return false;
    }
    return createTables();
}

bool DatabaseManager::createTables(){
    QSqlQuery query;

    QString createUsersTable=
    "CREATE TABLE IF NOT EXISTS users("
    "id INT AUTO_INCREMENT PRIMARY KEY,"
    "login VARCHAR(50) UNIQUE NOT NULL,"
    "email VARCHAR(100) UNIQUE NOT NULL,"
    "PASSWORD VARCHAR(64) NOT NULL,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                               ")ENGINE=InnoDB";
    if(!query.exec(createUsersTable)){
        qDebug()<<"Error creating users table:"<<query.lastError().text();
        return false;
    }
    QString createTransactionsTable=
    "CREATE TABLE IF NOT EXISTS transactions("
    "id INT AUTO INCREMENT PRIMARY KEY,"
    "user_id INT NOT NULL,"
    "is_expense BOOLEAN NOT NULL,"
    "category VARCHAR(50) NOT NULL,"
    "amount DECIMAL(10,2) NOT NULL,"
    "transaction_date DATETIME NOT NULL,"
    "comment TEXT,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY (user_id) REFERENCES user(id) ON DELETE CASCADE"
                                      ")ENGINE=InnoDB";

    if(!query.exec(createTransactionsTable)){
        qDebug()<<"Error creating transactions table:"<<query.lastError().text();
        return false;
    }
    return true;
}

//hash pass

// QString DatabaseManager::hashPassword(const QString&password){
//     QByteArray passwordBytes=password.toUft8();
//     QByteArray hashedPassword= QCryptographicHash::hash(passwordBytes,QCryptographicHash::Sha256);
//     return QString(hashedPassword.toHex());
// }


bool DatabaseManager::addUser(const QString&login, const QString&email, const QString&password){
    if(userExists(login)||emailExists(email)){
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO users(login, email,password)"
                  "VALUES(:login,:email,:password)");
    query.bindValue(":login",login);
    query.bindValue(":email",email);
    query.bindValue(":password",password);// additionaly can use hashing of pass
    if(!query.exec()){
        qDebug()<<"Error adding user:"<<query.lastError().text();
        return false;
    }
    return true;
}

int DatabaseManager::validateUser(const QString&email, const QString&password){
    QSqlQuery query;
    query.prepare("SELECT id,password FROM users WHERE email=:email");
    if(!query.exec()||!query.next()){
        qDebug()<<"Not found users data";
        return -1;
    }
    int userId=query.value(0).toInt();
    QString storedPassword=query.value(1).toString();
    if(storedPassword==password){
        return userId;
    }
    return -1;
}

bool DatabaseManager::userExists(const QString& login){
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE login=:login ");
    query.bindValue(":login",login);
    if(!query.exec()||!query.next()){
        return false;
    }
    return query.value(0).toInt()>0;
}

bool DatabaseManager::emailExists(const QString&email){
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email=:email");
    query.bindValue(":email",email);
    if(!query.exec()||!query.next()){
        return false;
    }
    return query.value(0).toInt()>0;
}

bool DatabaseManager::addTransaction(int userId, bool isExpense,const QString&category,double amount,
                                    const QDateTime& date,const QString&comment){
    QSqlQuery query;
    query.prepare("INSERT INTO transactions (user_id,is_expense,category)"
                  "amount, transaction_date,comment)"
                  "VALUES(:user_id, :is_expense, :category, :amount,"
                  ":transaction_date, :comment");
    query.bindValue(":user_id",userId);
    query.bindValue(":is_expense",isExpense);
    query.bindValue(":category",category);
    query.bindValue(":amount",amount);
    query.bindValue(":transaction_date",date);
    query.bindValue(":comment",comment);
    if(!query.exec()){
        qDebug()<<"Error adding transaction:"<<query.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::updateTransaction(int transactionId,bool isExpense, const QString&category,
                                        double amount, const QDateTime&date,const QString&comment){
    QSqlQuery query;
    query.prepare("UPDATE transactions SET is_expense=:is_expense,"
                  "category=:category, amount=:amount, transaction_date=:transaction_date,"
                  "comment=:comment WHERE id=:id");
    query.bindValue(":id",transactionId);
    query.bindValue(":is_expense",isExpense);
    query.bindValue(":category",category);
    query.bindValue(":amount",amount);
    query.bindValue(":transaction_date",date);
    query.bindValue(":comment",comment);

    if(!query.exec()){
        qDebug()<<"Error updating transaction:"<<query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteTransaction(int transactionId){
    QSqlQuery query;
    query.prepare("DELETE FROM transactions WHERE id=:id");
    query.bindValue(":id",transactionId);
    if(!query.exec()){
        qDebug()<<"Error deleting transaction:"<<query.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::getUserTransactions(int userId){
    QSqlQuery query;
    query.prepare("SELECT * FROM transactions WHERE user_id=:user_id"
                  "ORDER BY transaction_date DESC");
    query.bindValue(":user_id", userId);
    if(!query.exec()){
        qDebug()<<"Error getting user transactions:"<<query.lastError().text();
    }
    return query;
}


QSqlQuery DatabaseManager::getTransactionById(int transactionId){
    QSqlQuery query;
    query.prepare("SELECT*FROM transactions WHERE id:=id");
    query.bindValue(":id",transactionId);
    if(!query.exec()){
        qDebug()<<"Error getting transaction data:"<<query.lastError().text();
    }
    return query;
}


