#include "twitchapi.h"

TwitchAPI::TwitchAPI(QWidget *parent) : QWidget(parent)
{

}

TwitchAPI::~TwitchAPI()
{

}


QMap<QString, QString> TwitchAPI::getStreamInfo(const QString & channel, const QString & auth_token)
{
    QUrl current("https://api.twitch.tv/kraken/streams/" + channel);

    QByteArray answer = GET(current, auth_token);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    QString viewers = "0";
    QString fps = "0";

    QMap<QString, QString> ret_map;

    ret_map["offline"] = "false";

    if (!jsonObj["stream"].toObject().isEmpty()) {
        viewers = QString::number(jsonObj["stream"].toObject()["viewers"].toInt());
        fps = QString::number(jsonObj["stream"].toObject()["average_fps"].toDouble());
    } else {
        ret_map["offline"] = "true";
    }

    ret_map["viewers"] = viewers;
    ret_map["average_fps"] = fps;

    return ret_map;
}

QByteArray TwitchAPI::GET(QUrl u, const QString & auth_token)
{
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

QList<QString> TwitchAPI::getLastFollowers(const QString & channel, const QString & auth_token, int nFollowers)
{
    QUrl current("https://api.twitch.tv/kraken/channels/" + channel + "/follows");

    if (nFollowers > 100)
        nFollowers = 100;

    QUrlQuery param;
    param.addQueryItem("limit", QString::number(nFollowers));
    current.setQuery(param);

    QByteArray answer = GET(current, auth_token);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    QList<QString> out_list;

    QJsonArray jsonArray = jsonObj["follows"].toArray();

    for(QJsonArray::const_iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter) {

        out_list.push_back((*iter).toObject()["user"].toObject()["display_name"].toString());

    }

    return out_list;
}
