#include "databasemanager.h"


DatabaseManager::DatabaseManager(QObject*parent):QObject(parent)
{
    manager= new QNetworkAccessManager(this);
    projectURL="https://transactionsmanager2850456-default-rtdb.firebaseio.com/";
}

QString DatabaseManager::constructUrl(const QString &path){
    return projectURL+path+".json";
}

QJsonDocument DatabaseManager::synchronousRequest(const QString &path, const QString &method, const QJsonDocument &data){
    QNetworkRequest request(constructUrl(path));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QNetworkReply*reply=nullptr;
    if(method=="GET") {
        reply=manager->get(request);
    }
    else if(method=="POST"){
        reply=manager->post(request,data.toJson());
    }
    else if(method=="PUT"){
        reply=manager->put(request,data.toJson());
    }
    else if(method=="DELETE"){
        reply=manager->deleteResource(request);
    }
    else if(method=="PATCH"){
        reply=manager->sendCustomRequest(request,"PATCH",data.toJson());
    }
    if(!reply){
        qDebug()<<"Error: Failed to create network reply for"<<method<<"request to"<<request.url();
        return QJsonDocument();
    }
    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();

    if(reply->error()==QNetworkReply::NoError){
        QByteArray responseData=reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(responseData);
    }
    qDebug() << "Network error:" << reply->errorString();
    reply->deleteLater();
    return QJsonDocument();
}


bool DatabaseManager::addUser(const QString &login, const QString &email, const QString &password){
    if(userExists(login)||emailExists(email)){
        return false;
    }
    QJsonObject userData;
    userData["login"] = login;
    userData["email"] = email;
    userData["password"] = password; //hashPassword(password);
    userData["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(userData);
    QJsonDocument response=synchronousRequest("users","POST",doc);
    return !response.isNull();
}

bool DatabaseManager::userExists(const QString &login){
    QJsonDocument response=synchronousRequest("users","GET");
    if(response.isNull()){
        qDebug()<<"No response in userExist";
        return false;
    }
    QJsonObject users=response.object();
    for(auto it=users.begin();it!=users.end();++it){
        if(it.value().toObject()["login"].toString()==login){
            return true;
        }
    }
    return false;
}

bool DatabaseManager::emailExists(const QString &email){
    QJsonDocument response=synchronousRequest("users","GET");
    if(response.isNull()){
        qDebug()<<"No response in emailEXists";
        return false;
    }
    QJsonObject users=response.object();
    for(auto it=users.begin();it!=users.end();++it){
        if(it.value().toObject()["email"].toString()==email){
            return true;
        }
    }
    return false;
}

QString DatabaseManager::validateUser(const QString &email, const QString &password){
    //QString hashedPassword =hashPassword(password);
    QJsonDocument response= synchronousRequest("users","GET");
    if(response.isNull()){
        qDebug()<<"Error: no response in validateUser";
        return QString();
    }
    QJsonObject users=response.object();
    for(auto it=users.begin();it!=users.end();++it){
        QJsonObject user =it.value().toObject();
        if (user["email"].toString() == email &&
            user["password"].toString() == password/*hashedPassword*/) {
            return it.key();
        }
    }
    return QString();
}

//Methods to work with Transactions

bool DatabaseManager::addTransaction(int userId, bool isExpense, const QString &category, double amount, const QDateTime &date, const QString &comment){
    QJsonObject transactionData;
    transactionData["user_id"]=userId;
    transactionData["is_expense"] = isExpense;
    transactionData["category"] = category;
    transactionData["amount"] = amount;
    transactionData["transaction_date"] = date.toString(Qt::ISODate);
    transactionData["comment"] = comment;
    transactionData["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(transactionData);
    QJsonDocument response=synchronousRequest("transactions","POST",doc);
    return !response.isNull();
}
bool DatabaseManager::deleteTransaction(int transactionId){
    QString path=QString("transactions/%1").arg(transactionId); //ussage of placeholder
    QJsonDocument response =synchronousRequest(path,"DELETE");
    return !response.isNull();
}
//Methods to get all or specific transactions
QJsonArray DatabaseManager::getUserTransactions(int userId){
    QJsonDocument response= synchronousRequest("transactions","GET");
    if(response.isNull()){
        qDebug()<<"Error: reponse is null in getUserTransactions";
        return QJsonArray();
    }
    QJsonArray userTransactions;
    QJsonObject transactions =response.object();

    for(auto it=transactions.begin();it!=transactions.end();++it){
        QJsonObject transaction=it.value().toObject();
        if (transaction["user_id"].toInt() == userId) {
            transaction["id"] = it.key();
            userTransactions.append(transaction);
        }
    }
    return userTransactions;
}

QJsonObject DatabaseManager::getTransactionById(int transactionId){
    QJsonDocument response=synchronousRequest("transactions","GET");
    if(response.isNull()){
        qDebug()<<"Error: reponse is null in getTransactionById";
        return QJsonObject();
    }
    QJsonObject transactionById;
    QJsonObject transactions=response.object();
    for (auto it=transactions.begin();it!=transactions.end();++it){
        if(it.key().toInt() == transactionId){
            transactionById = it.value().toObject();
            return transactionById;
        }
    }
    return QJsonObject();
}

QString DatabaseManager::hashPassword(const QString &password){
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return QString(hashedPassword.toHex());
}
