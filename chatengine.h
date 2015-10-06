#ifndef CHATENGINE_H
#define CHATENGINE_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QDebug>

#include "ircmsgparser.h"

class ChatEngine : public QObject
{
    Q_OBJECT

    QString cur_channel;

public:
    explicit ChatEngine(QObject *parent = 0);

    ~ChatEngine();

    bool join(const QString & auth_token, const QString & nick, const QString channel);

    bool quit();

    bool sendToChat(const QString & msg);

    void enableWhispers() {
        socket->write(QString("CAP REQ :twitch.tv/commands\r\n").toLatin1());
    }

private:
    QTcpSocket * socket;

signals:
    void newMessageReady(const IRCMsgParser::IRCMsg & msg);

public slots:

    void readyRead();

};

#endif // CHATENGINE_H
