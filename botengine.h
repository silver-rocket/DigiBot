#ifndef BOTENGINE_H
#define BOTENGINE_H

#include <random>

#include <QObject>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QMap>
#include <QRegExp>
#include <QList>
#include <QCryptographicHash>

#include "ircmsgparser.h"
#include "simplexmlreadwrite.h"
#include "twitchapi.h"
#include "random.h"
#include "settings.h"

class BotEngine : public QObject
{
    Q_OBJECT

    typedef void (BotEngine::* fnPtr)(const IRCMsgParser::IRCMsg & msg);

private:

    struct CommandInfo {
        fnPtr fn_ptr;
        size_t minDelayMsec;
        qint64 currentDelayMsec;
        bool isOn;
        bool isCustom;
        QString customCmdMessage;
        int repeatDelay;
    };

    static const char specialSymbol = '!';
    static const int defaultMinDelayMsec = 5000;

    QMap<QString, CommandInfo> commandList;

    QTimer repeatMsgTimer;
    QString repeatMsg;

    bool greetSubs;
    QString greetingMsg;

    QString cur_channel;

public:

    explicit BotEngine(QObject *parent = 0);

    ~BotEngine();

    void update_single_cmd_settings(const QString & command);

    void set_cur_channel(const QString & channel);

    void set_subs_greeting_state(bool state);

    void set_subs_greeting_msg(const QString & msg);

    void start_repeating_message(const QString & msg, int repeatTimeSec);

    void stop_repeating_message();

    void parseNewMsg(const IRCMsgParser::IRCMsg & msg);

private:


    void processTwitchNotifyMsg(const IRCMsgParser::IRCMsg & msg);

    void showSubGreetingMsg(const IRCMsgParser::IRCMsg & msg);

    QString get_command(const QString & text);

    QString processCustomMessage(const IRCMsgParser::IRCMsg & msg);

    void showCustomMessage(const IRCMsgParser::IRCMsg & msg);

    /* Build-in Commands */

    void showDongSize(const IRCMsgParser::IRCMsg & msg);

    void showUptime(const IRCMsgParser::IRCMsg & msg);

    void showStreamDelay(const IRCMsgParser::IRCMsg & msg);

    void showFollowTime(const IRCMsgParser::IRCMsg & msg);

signals:

    void botMessageReady(const QString & msg);

public slots:

    void onRepeatTimerTimeout();
};

#endif // BOTENGINE_H
