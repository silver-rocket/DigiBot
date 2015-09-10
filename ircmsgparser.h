#ifndef IRCMSGPARSER_H
#define IRCMSGPARSER_H

#include <QString>
#include <QRegExp>
#include <QDebug>

class IRCMsgParser
{
public:

    enum IRCMsgType { WELCOME, JOIN, PART, PRIVMSG, PING, UNSUPPORTED, ERR_AUTH_TOKEN, ERR_LOGIN };

    struct IRCMsg {
        IRCMsgType type;
        QString sender;
        QString text;
    };

    IRCMsgParser();
    ~IRCMsgParser();

    static IRCMsg parse(QString string);
};

#endif // IRCMSGPARSER_H
