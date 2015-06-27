#include "botengine.h"

BotEngine::BotEngine():countMessages(false)
{

}

BotEngine::~BotEngine()
{

}

void BotEngine::parseMsg(IRCMsgParser::IRCMsg msg)
{
    if (countMessages) {

        if (msg.type == IRCMsgParser::PRIVMSG) {
            userMsgMap[msg.sender].count++;
        }

    }

    if (msg.type == IRCMsgParser::PRIVMSG) {
        userMsgMap[msg.sender].count++;
    }

    //msg.text.contains()

}
