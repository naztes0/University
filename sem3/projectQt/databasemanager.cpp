#include "databasemanager.h"


DatabaseManager::DatabaseManager(QObject*parent):QObject(parent)
{
    manager= new QNetworkAccessManager(this);
    projectURL="";
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

    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();

    if(reply->error()==QNetworkReply::NoError){
        QByteArray responseData=reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(responseData);

    }
}


bool DatabaseManager::adduser(const QString &login, const QString &email, const QString &password){
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
