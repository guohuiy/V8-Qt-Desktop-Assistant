#include "ai.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>

AIModule::AIModule(QObject *parent) : QObject(parent) {}

AIModule::~AIModule() {}

QString AIModule::processText(const QString &text) {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://localhost:11434/api/chat"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["model"] = "gpt-oss:120b-cloud";  // 使用用户指定的模型
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = text;
    messages.append(message);
    json["messages"] = messages;
    json["stream"] = false;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager.post(request, data);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // 设置超时10秒
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        reply->abort();
        loop.quit();
    });
    timer.start(10000);  // 10秒超时

    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return "AI Error: " + reply->errorString();
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
    if (responseDoc.isNull()) {
        return "AI Error: Invalid JSON response";
    }

    QJsonObject responseObj = responseDoc.object();
    QJsonObject messageObj = responseObj["message"].toObject();
    QString response = messageObj["content"].toString();

    return response.trimmed();
}
