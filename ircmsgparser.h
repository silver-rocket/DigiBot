#ifndef IRCMSGPARSER_H
#define IRCMSGPARSER_H

#include <QString>
#include <QRegExp>
#include <QDebug>

class IRCMsgParser
{
public:

    enum IRCMsgType { JOIN, PART, PRIVMSG, PING, UNSUPPORTED };

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
