//
// Created by yichun822 on 2026/3/26.
//

#ifndef AI_HISTORY_DEEPSEEKCLIENT_H
#define AI_HISTORY_DEEPSEEKCLIENT_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class DeepSeekClient : public QObject
{
    Q_OBJECT
public:
    explicit DeepSeekClient(QObject *parent = nullptr);
    void setApiKey(const QString &key);
    void sendMessage(const QString &userMessage);

signals:
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_networkManager;
    QString m_apiKey;
    QString m_apiUrl = "https://api.deepseek.com/v1/chat/completions";
    QJsonObject buildRequestPayload(const QString &userMessage);
    void handleResponse(QNetworkReply *reply);
};


#endif //AI_HISTORY_DEEPSEEKCLIENT_H