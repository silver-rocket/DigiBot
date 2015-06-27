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
#include <QList>

class TwitchAPI : public QWidget
{
    Q_OBJECT

public:
    explicit TwitchAPI(QWidget *parent = 0);
    ~TwitchAPI();

    QMap<QString, QString> getStreamInfo(const QString & channel, const QString & auth_token);
    QList<QString> getLastFollowers(const QString & channel, const QString & auth_token, int nFollowers = 10);

    QByteArray GET(QUrl r, const QString & auth_token);

signals:

public slots:
};

#endif // TWITCHAPI_H
