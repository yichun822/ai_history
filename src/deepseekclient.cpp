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
extern QString system_prompt;
QJsonObject DeepSeekClient::buildRequestPayload(const QString &userMessage)
{
    QJsonObject payload;
    payload["model"] = "deepseek-v4-flash";   // 模型名称，根据文档填写
    payload["stream"] = false;             // 非流式响应
    payload["temperature"] = 0.0;

    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    //systemMsg["content"] = "return message like \"time:xxx;happened:xxx;content:xxx;EOF;\",the key words you can use only include\"time happened content\",end an envent with\"EOF;\".Don't use\\n.Do not generate time ranges, only generate time points.Represent BC with\"-\"in the head of number.if you search unsuccessfully,just return\"EOF;\" directly";
    systemMsg["content"] = system_prompt;
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

QString system_prompt="The user will provide some historical question,and you need to return answers. Please only parse the \"time\" \"happened\" and \"content\" and output them in JSON format in Chinese. \n EXAMPLE INPUT: \n xxx \n EXAMPLE JSON OUTPUT:\n[{\"time\": \"xxx\",\"happened\": \"xxx\",\"content\": \"xxx\"},{\"time\": \"xxx\",\"happened\": \"xxx\",\"content\": \"xxx\"}].Do not generate time ranges, only generate time points.Represent BC with\"-\"in the head of number.";