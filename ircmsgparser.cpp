#include "ircmsgparser.h"

IRCMsgParser::IRCMsgParser()
{

}

IRCMsgParser::~IRCMsgParser()
{

}

IRCMsgParser::IRCMsg IRCMsgParser::parse(QString string) {

    IRCMsgParser::IRCMsg ret_msg;

    ret_msg.type = IRCMsgParser::UNSUPPORTED;

    // System messages //

    if (string.contains("Welcome, GLHF")) {

        ret_msg.type = IRCMsgParser::WELCOME;
        return ret_msg;

    } else if (string.contains("Login unsuccessful")) {

        ret_msg.type = IRCMsgParser::ERR_AUTH_TOKEN;
        return ret_msg;

    } else if (string.contains("Error logging in")) {

        ret_msg.type = IRCMsgParser::ERR_LOGIN;
        return ret_msg;
    }

    // User related messages //

    QRegExp regex_action(":(.+)\!.+tv (.+) #(.+)"); // join, part or msg

    QRegExp regex_ping("PING :(.+)");

    bool ping = regex_ping.exactMatch(string);

    if (ping) {

        ret_msg.type = IRCMsgParser::PING;
        ret_msg.text = regex_ping.cap(1);
        return ret_msg;

    }

    bool action = regex_action.exactMatch(string);

    if (action) {
        ret_msg.sender = regex_action.cap(1);

        if (regex_action.cap(2) == "JOIN") {
            ret_msg.type = IRCMsgParser::JOIN;
        }
        else
        if (regex_action.cap(2) == "PART") {
            ret_msg.type = IRCMsgParser::PART;
        }
        else
        if (regex_action.cap(2) == "PRIVMSG") {
            ret_msg.type = IRCMsgParser::PRIVMSG;

            QRegExp regex_msg(".+ :(.+)");
            bool msg_ok = regex_msg.exactMatch(regex_action.cap(3));
            ret_msg.text = regex_msg.cap(1);

            if (!msg_ok)
                ret_msg.type = IRCMsgParser::UNSUPPORTED;

        }

    }

    return ret_msg;
}
