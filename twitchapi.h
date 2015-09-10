#ifndef TWITCHAPI_H
#define TWITCHAPI_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QRegExp>
#include <QTimeZone>


class TwitchAPI : public QWidget
{
    Q_OBJECT

private:

    QString auth_token;

    explicit TwitchAPI(QWidget *parent = 0): QWidget(parent) {}

    QByteArray GET(QUrl u) {
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);

        QNetworkRequest req(u);
        req.setRawHeader("Accept", "application/vnd.twitchtv.v3+json");
        req.setRawHeader("Authorization", "OAuth " + auth_token.toLatin1());
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );

        QNetworkReply* reply = manager->get(req);
        QEventLoop wait;
        connect(manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
        QTimer::singleShot(10000, &wait, SLOT(quit()));
        wait.exec();
        QByteArray answer = reply->readAll();
        reply->deleteLater();
        return answer;
    }

public:
    static TwitchAPI & instance();

    void set_auth_token(const QString& tok);

    QMap<QString, QString> getStreamInfo(const QString & channel);

    QTime getStreamUptime(const QString & channel);

    int getStreamDelay(const QString & channel);

    QList<QString> getLastFollowers(const QString & channel, int nFollowers = 10);
};

#endif // TWITCHAPI_H
