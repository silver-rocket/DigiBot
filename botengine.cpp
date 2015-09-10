#include "botengine.h"

BotEngine::BotEngine(QObject *parent)
    : QObject(parent)
{
    commandList["dong"].fn_ptr = &BotEngine::showDongSize;
    commandList["up"].fn_ptr = &BotEngine::showUptime;
    commandList["delay"].fn_ptr = &BotEngine::showStreamDelay;

    for (QMap<QString, CommandInfo>::iterator it = commandList.begin(); it != commandList.end(); ++it) {

        QString command = it.key();

        commandList[command].isCustom = 0;
        commandList[command].currentDelayMsec = 0;

        update_single_cmd_settings(command);
    }

    for (QMap<QString, QMap<QString, QString> >::const_iterator it = CUSTOM_COMMANDS_SETTINGS.begin(); it != CUSTOM_COMMANDS_SETTINGS.end(); ++it) {

        QString command = it.key();

        if (command.contains(' '))
            continue;

        commandList[command].currentDelayMsec = 0;
        commandList[command].isCustom = 1;
        commandList[command].fn_ptr = &BotEngine::showCustomMessage;

        update_single_cmd_settings(command);
    }

    connect(&repeatMsgTimer, SIGNAL(timeout()), this, SLOT(onRepeatTimerTimeout()));
}

BotEngine::~BotEngine()
{

}

void BotEngine::update_single_cmd_settings(const QString & command)
{
    QMap<QString, QString> single_command_settings;

    if (commandList[command].isCustom)
        single_command_settings = CUSTOM_COMMANDS_SETTINGS[command];
    else
        single_command_settings = COMMANDS_SETTINGS[command];

    bool ok = false;

    commandList[command].minDelayMsec = defaultMinDelayMsec;

    if (single_command_settings["MinDelayMsec"] != "") {
        int delay = single_command_settings["MinDelayMsec"].toInt(&ok);
        if (ok)
            commandList[command].minDelayMsec = delay;
    }

    commandList[command].isOn = false;

    if (single_command_settings["IsOn"] != "") {
        int ison = single_command_settings["IsOn"] == "true" ? 1 : 0;
        qDebug() << command << ' ' << ison;
        if (ok)
            commandList[command].isOn = ison;
    }

    if (commandList[command].isCustom) {
        if (single_command_settings["Message"] != "")
            commandList[command].customCmdMessage = single_command_settings["Message"];
    }

}

void BotEngine::set_cur_channel(const QString & channel)
{
    cur_channel = channel;
}

void BotEngine::set_subs_greeting_state(bool state)
{
    greetSubs = state;
}

void BotEngine::set_subs_greeting_msg(const QString & msg)
{
    if (msg != "")
        greetingMsg = msg;
}

void BotEngine::start_repeating_message(const QString & msg, int repeatTimeSec)
{
    repeatMsg = msg;
    emit botMessageReady(repeatMsg);
    repeatMsgTimer.start(repeatTimeSec * 1000);
}

void BotEngine::stop_repeating_message()
{
    repeatMsgTimer.stop();
}

void BotEngine::parseNewMsg(const IRCMsgParser::IRCMsg & msg)
{
    QString text = msg.text;

    if (text.startsWith(specialSymbol)) {

        QString command = get_command(text);

        if (commandList.contains(command)) {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

            if (commandList[command].isOn) {
                if (currentTime - commandList[command].currentDelayMsec > commandList[command].minDelayMsec) {
                    (this->*commandList[command].fn_ptr)(msg);
                    commandList[command].currentDelayMsec = currentTime;
                }
            }

        }

    }

    if (msg.sender.toLower() == "twitchnotify") {
        processTwitchNotifyMsg(msg);
    }

}

void BotEngine::processTwitchNotifyMsg(const IRCMsgParser::IRCMsg & msg)
{
    if (msg.text.contains("subscribed", Qt::CaseInsensitive)) {

        if (greetSubs) {
            showSubGreetingMsg(msg);
        }
    }

    // process other func here
}

void BotEngine::showSubGreetingMsg(const IRCMsgParser::IRCMsg & msg)
{
    QString sub_name = msg.text.section(" ", 0, 0);
    emit botMessageReady(greetingMsg.replace("@sub", sub_name, Qt::CaseInsensitive));
}

QString BotEngine::processCustomMessage(const IRCMsgParser::IRCMsg & msg)
{
    QString cmd = get_command(msg.text);
    QString customMessage = commandList[cmd].customCmdMessage;

    if (customMessage.contains("@Sender", Qt::CaseInsensitive)) {
        customMessage.replace("@Sender", msg.sender, Qt::CaseInsensitive);
    }

    /* Random numbers feature works fine only if used once per request! */

    QRegExp rand_regex("(.*)rand\\((.+),(.+)\\)(.*)");
    if (customMessage.contains(rand_regex)) {

        rand_regex.exactMatch(customMessage);
        customMessage = rand_regex.cap(1) + QString::number(Random::nextLinear(rand_regex.cap(2).toInt(), rand_regex.cap(3).toInt(), QDateTime::currentMSecsSinceEpoch())) + rand_regex.cap(4);
    }

    QRegExp randn_regex("(.*)randn\\((.+),(.+)\\)(.*)");
    if (customMessage.contains(randn_regex)) {

        randn_regex.exactMatch(customMessage);
        customMessage = randn_regex.cap(1) + QString::number(Random::nextGaussian(randn_regex.cap(2).toInt(), randn_regex.cap(3).toInt(), QDateTime::currentMSecsSinceEpoch())) + randn_regex.cap(4);
    }

    return customMessage;
}

void BotEngine::showCustomMessage(const IRCMsgParser::IRCMsg & msg)
{
    QString new_msg = processCustomMessage(msg);
    emit botMessageReady(new_msg);
}

void BotEngine::showDongSize(const IRCMsgParser::IRCMsg & msg)
{
    long seed_val = 0;
    for (size_t i = 0; i < (size_t)std::min(msg.sender.size(), 8); i++) {\
        seed_val += (msg.sender.at(i).toLatin1() - 'a') * (10<<(i+1));
    }

    size_t l = Random::nextGaussian(14, 5, seed_val);

    QString out_string = msg.sender + ", " + QString::number(l) + "cm";

    if (l > 17)
        out_string += " PogChamp";
    else if (l < 11)
        out_string += " Kappa";

    emit botMessageReady(out_string);
}

QString BotEngine::get_command(const QString & text)
{
    QString cmd = text;
    cmd.remove(0, 1);
    return cmd.section(" ", 0, 0);
}

void BotEngine::showUptime(const IRCMsgParser::IRCMsg & msg)
{
    QTime uptime = TwitchAPI::instance().getStreamUptime(cur_channel);
    if (uptime != QTime())
        emit botMessageReady("Stream is up for " + uptime.toString() + ".");
}

void BotEngine::showStreamDelay(const IRCMsgParser::IRCMsg & msg)
{
    int delay = TwitchAPI::instance().getStreamDelay(cur_channel);
    if (delay != -1)
        emit botMessageReady("Delay: " + QString::number(delay/60) + "min");
}

void BotEngine::onRepeatTimerTimeout()
{
    emit botMessageReady(repeatMsg);
}
