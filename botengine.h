#ifndef BOTENGINE_H
#define BOTENGINE_H

#include <QMap>
#include <QString>
#include <QDebug>

#include "ircmsgparser.h"

class BotEngine
{

public:

    struct msgMapValue {
        int count;
        int swearCount;
    };

private:

    bool countMessages;
    QMap<QString, msgMapValue> userMsgMap;

public:

    BotEngine();
    ~BotEngine();

    void parseMsg(IRCMsgParser::IRCMsg msg);

};

#endif // BOTENGINE_H
