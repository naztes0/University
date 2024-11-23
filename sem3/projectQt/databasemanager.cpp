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
