//
// Created by yichun822 on 2026/3/26.
//
#include "deepseekclient.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QDebug>

DeepSeekClient::DeepSeekClient(QObject *parent) : QObject(parent)
{
    connect(&m_networkManager, &QNetworkAccessManager::finished,
            this, &DeepSeekClient::onReplyFinished);
}

void DeepSeekClient::setApiKey(const QString &key)
{
    m_apiKey = key;
}

void DeepSeekClient::sendMessage(const QString &userMessage)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API Key 未设置");
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(m_apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject payload = buildRequestPayload(userMessage);
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    m_networkManager.post(request, data);
}

QJsonObject DeepSeekClient::buildRequestPayload(const QString &userMessage)
{
    QJsonObject payload;
    payload["model"] = "deepseek-chat";   // 模型名称，根据文档填写
    payload["stream"] = false;             // 非流式响应

    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "You are a helpful assistant,you need to return message like \"time:1921;happened:中国共产党成立;content:中国革命的面貌焕然一新了;EOF;\",the key words you can use only include\"time happened content\",end an envent with\"EOF;\",represent BC with\"-\"in the head of number.if you search unsuccessfully,just return\"EOF;\" directly";
    messages.append(systemMsg);

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    payload["messages"] = messages;
    return payload;
}

void DeepSeekClient::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        handleResponse(reply);
    } else {
        QString errorStr = QString("网络错误: %1\n%2")
                               .arg(reply->errorString())
                               .arg(reply->readAll());
        emit errorOccurred(errorStr);
    }
    reply->deleteLater();
}

void DeepSeekClient::handleResponse(QNetworkReply *reply)
{
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull()) {
        emit errorOccurred("无效的 JSON 响应");
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("error")) {
        QJsonObject errorObj = obj["error"].toObject();
        QString errorMsg = errorObj["message"].toString();
        emit errorOccurred("API 错误: " + errorMsg);
        return;
    }

    QJsonArray choices = obj["choices"].toArray();
    if (choices.isEmpty()) {
        emit errorOccurred("响应中没有 choices 字段");
        return;
    }

    QJsonObject firstChoice = choices[0].toObject();
    QJsonObject message = firstChoice["message"].toObject();
    QString content = message["content"].toString();

    emit responseReceived(content);
}