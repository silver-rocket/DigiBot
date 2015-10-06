#include "twitchapi.h"

TwitchAPI & TwitchAPI::instance() {
    static TwitchAPI inst;
    return inst;
}

void TwitchAPI::set_auth_token(const QString& tok) {
   auth_token = tok;
}

QMap<QString, QString> TwitchAPI::getStreamInfo(const QString & channel)
{
    QUrl current("https://api.twitch.tv/kraken/streams/" + channel);

    QByteArray answer = GET(current);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    QString viewers = "0";
    QString fps = "0";
    QString created_at = "0";
    QString delay = "0";

    QMap<QString, QString> ret_map;

    ret_map["offline"] = "false";

    if (!jsonObj["stream"].toObject().isEmpty()) {
        viewers = QString::number(jsonObj["stream"].toObject()["viewers"].toInt());
        fps = QString::number(jsonObj["stream"].toObject()["average_fps"].toDouble());
        created_at = jsonObj["stream"].toObject()["created_at"].toString();
        delay = QString::number(jsonObj["stream"].toObject()["channel"].toObject()["delay"].toDouble());
    } else {
        ret_map["offline"] = "true";
    }

    ret_map["viewers"] = viewers;
    ret_map["average_fps"] = fps;
    ret_map["created_at"] = created_at;
    ret_map["delay"] = delay;

    return ret_map;
}

QTime TwitchAPI::getStreamUptime(const QString & channel)
{
    QMap<QString, QString> info = getStreamInfo(channel);

    if (info["offline"] == "true")
        return QTime();

    QRegExp time_regex("(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):(\\d{2})Z");

    bool ok = time_regex.exactMatch(info["created_at"]);

    if (!ok)
        return QTime();

    QDateTime start_stream_time(QDate(time_regex.cap(1).toInt(),time_regex.cap(2).toInt(), time_regex.cap(3).toInt()), QTime(time_regex.cap(4).toInt(),time_regex.cap(5).toInt(), time_regex.cap(6).toInt()), QTimeZone::utc());

    qint64 elapsed_ms = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - start_stream_time.toMSecsSinceEpoch();

    int hours = int(elapsed_ms/1000./60./60); // % 24 ?
    int mins = int(elapsed_ms/1000./60) % 60;
    int secs = (elapsed_ms/1000) % 60;

    QTime elapsed(hours, mins, secs);

    return elapsed;
}

QTime TwitchAPI::getFollowTime(const QString & user, const QString & channel)
{
    QUrl current("https://api.twitch.tv/kraken/users/"+user+"/follows/channels/"+channel);

    QByteArray answer = GET(current);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    QString created_at = jsonObj["created_at"].toString();

    if (created_at == "")
        return QTime();

    QRegExp time_regex("(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):(\\d{2}).*");

    bool ok = time_regex.exactMatch(created_at);

    qDebug() << created_at;

    if (!ok)
        return QTime();

    QDateTime start_follow_time(QDate(time_regex.cap(1).toInt(),time_regex.cap(2).toInt(), time_regex.cap(3).toInt()), QTime(time_regex.cap(4).toInt(),time_regex.cap(5).toInt(), time_regex.cap(6).toInt()), QTimeZone::utc());

    qint64 elapsed_ms = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - start_follow_time.toMSecsSinceEpoch();

    int days = int(elapsed_ms/1000./60./60./24) % 30;
    int months = int(elapsed_ms/1000./60./60./24./30.41666) % 12;
    int years = int(elapsed_ms/1000./60./60./24./30.41666/12);

    return QTime(years, months, days);
}

int TwitchAPI::getStreamDelay(const QString & channel)
{
    QMap<QString, QString> info = getStreamInfo(channel);

    if (info["offline"] == "true")
        return -1;

    return info["delay"].toInt();
}

QList<QString> TwitchAPI::getLastFollowers(const QString & channel, int nFollowers)
{
    QUrl current("https://api.twitch.tv/kraken/channels/" + channel + "/follows");

    if (nFollowers > 100)
        nFollowers = 100;

    QUrlQuery param;
    param.addQueryItem("limit", QString::number(nFollowers));
    current.setQuery(param);

    QByteArray answer = GET(current);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    QList<QString> out_list;

    QJsonArray jsonArray = jsonObj["follows"].toArray();

    for(QJsonArray::const_iterator iter = jsonArray.begin(); iter != jsonArray.end(); ++iter) {

        out_list.push_back((*iter).toObject()["user"].toObject()["display_name"].toString());

    }

    return out_list;
}
