#include "chatengine.h"

ChatEngine::ChatEngine(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

ChatEngine::~ChatEngine()
{
    delete socket;
}

bool ChatEngine::join(const QString & auth_token, const QString & nick, const QString channel)
{

    socket->connectToHost(QString("irc.twitch.tv"), 6667);
    socket->write("PASS oauth:" + auth_token.toUtf8() + "\r\n");
    socket->write("NICK " + nick.toUtf8() + "\r\n");
    socket->write("JOIN #" + channel.toUtf8() + "\r\n");

/*
            socket->connectToHost(QString("192.16.64.180"), 6667);
            socket->write("PASS oauth:" + auth_token.toUtf8() + "\r\n");
            socket->write("NICK " + nick.toUtf8() + "\r\n");
            socket->write("JOIN #_thelakmus_1440494034233\r\n");
*/

    cur_channel = channel;

    return 0;
}

bool ChatEngine::quit()
{
    socket->write("QUIT\r\n");
    socket->flush();
    socket->disconnectFromHost();
    return 0;
}

bool ChatEngine::sendToChat(const QString & msg)
{
    socket->write(QString("PRIVMSG #" + cur_channel + " :" + msg + "\r\n").toLatin1());
    return 0;
}

void ChatEngine::readyRead()
{
    QString line = socket->readLine();

    //qDebug() << line;

    IRCMsgParser::IRCMsg msg = IRCMsgParser::parse(line);

    if (msg.type == IRCMsgParser::PING)
        socket->write(QString("PONG :" + msg.text).toLatin1());
    else
        emit newMessageReady(msg);

    if(socket->canReadLine())
        this->readyRead();
}
