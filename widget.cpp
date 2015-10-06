#include "widget.h"
#include "ui_widget.h"

const QString Widget::mainWindowTitle = "Digibot v 1.0.1";
const QString Widget::appID = "jv1zuv3ifjgbt45ubipfk1ng7khpnhi";
const QString Widget::settingsFile = "DigiBotSettings.xml";
const QString Widget::commandsSettingsFile = "CommandsSettings.xml";
const QString Widget::customCommandsFile = "CustomCommandsSettings.xml";

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle(mainWindowTitle);

    load_common_settings();
    load_commands_settings();
    load_custom_commands_settings();
    //load_QA();

    chatConnected = false;
    streamConnected = false;

    streamInfoTimer = new QTimer(this);
    connect(streamInfoTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    chat = new ChatEngine();

    connect(chat, SIGNAL(newMessageReady(const IRCMsgParser::IRCMsg &)), this, SLOT(onNewChatMsg(const IRCMsgParser::IRCMsg &)));

    bot = new BotEngine();

    connect(bot, SIGNAL(botMessageReady(const QString &)), this, SLOT(onNewBotMsg(const QString &)));

    TwitchAPI::instance().set_auth_token(ui->authTokenEdit->text());

    bot->set_subs_greeting_state(ui->greetNewSubCheckBox->isChecked());
    bot->set_subs_greeting_msg(ui->greetingMsgTextEdit->toPlainText());

    //connect_to_chat();
}

Widget::~Widget()
{
    delete streamInfoTimer;
    delete bot;
    delete chat;
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    int n = 10;
    QList<QString> last_followers = TwitchAPI::instance().getLastFollowers(currentStreamName, n);

    if (last_followers.size() != n) {
        _log("Can't retrieve last followers. Check stream connection.", Qt::red);
    }
    else {
        ui->lastFollowers->clear();
        for (int i=0; i<n; i++) {
            ui->lastFollowers->addItem(last_followers[i]);
        }
    }

}

void Widget::onTimerTimeout()
{
    QMap<QString, QString> stream_info = TwitchAPI::instance().getStreamInfo(currentStreamName);

    if (stream_info["offline"] == "true") {
        streamInfoTimer->stop();
    }

    ui->viewersLabel->setText(stream_info["viewers"]);
    ui->fpsLabel->setText(stream_info["average_fps"]);

    //bot.showUptime(IRCMsgParser::IRCMsg());
}

void Widget::load_common_settings()
{
    COMMON_SETTINGS = SimpleXMLReadWrite::readNonIerarchy(settingsFile);

    ui->channelEdit->setText(COMMON_SETTINGS["Channel"]);
    ui->usernameEdit->setText(COMMON_SETTINGS["Username"]);
    ui->authTokenEdit->setText(COMMON_SETTINGS["Auth_token"]);
}

void Widget::load_commands_settings()
{
    COMMANDS_SETTINGS = SimpleXMLReadWrite::readIerarchy(commandsSettingsFile);

    // visualize in table:
    for (QMap<QString, QMap<QString, QString> >::iterator it = COMMANDS_SETTINGS.begin(); it != COMMANDS_SETTINGS.end(); ++it) {

        QString command = it.key();
        QMap<QString, QString> single_command_settings = it.value();
        add_row_to_table(ui->commandsTable,
                         command,
                         single_command_settings["IsOn"],
                         single_command_settings["MinDelayMsec"]);

    }
}

void Widget::load_custom_commands_settings()
{
    CUSTOM_COMMANDS_SETTINGS = SimpleXMLReadWrite::readIerarchy(customCommandsFile);

    // visualize in table:
    for (QMap<QString, QMap<QString, QString> >::iterator it = CUSTOM_COMMANDS_SETTINGS.begin(); it != CUSTOM_COMMANDS_SETTINGS.end(); ++it) {

        QString command = it.key();
        QMap<QString, QString> single_command_settings = it.value();
        add_row_to_table(ui->customCommandsTable,
                         command,
                         single_command_settings["IsOn"],
                         single_command_settings["MinDelayMsec"],
                         single_command_settings["Message"]);

    }
}

void Widget::load_QA()
{
    /* Deprecated */

    QFile file("");

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Can't open QA database");
        return;
    }

    QTextStream stream(&file);

    stream.setCodec("cp-1251"); // rus support

    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        QRegExp regex("(.+): (.+)");

        bool match = regex.exactMatch(line);

        qDebug() << line << ' ' << match;

        if (!match) {
            QMessageBox::warning(this, "Error", "Parsing QA data went wrong");
            return;
        }

        QADataBase[regex.cap(1)] = regex.cap(2);
    }

    qDebug() << QADataBase;

    file.close();
}

void Widget::save_common_settings()
{
    COMMON_SETTINGS["Username"] = ui->usernameEdit->text();
    COMMON_SETTINGS["Channel"] = ui->channelEdit->text().toLower();
    COMMON_SETTINGS["Auth_token"] = ui->authTokenEdit->text();

    SimpleXMLReadWrite::writeNonIerarchy(COMMON_SETTINGS, settingsFile);
}

void Widget::save_commands_settings()
{

}

void Widget::save_custom_commands_settings()
{
    SimpleXMLReadWrite::writeIerarchy(CUSTOM_COMMANDS_SETTINGS, customCommandsFile);
}

void Widget::on_getAuthButton_clicked()
{
    QString auth_path = "https://api.twitch.tv/kraken/oauth2/authorize";
    QUrl url(auth_path);

    QString scope = "user_read+user_blocks_edit+user_blocks_read+user_follows_edit+channel_read+"
            "channel_editor+channel_commercial+channel_stream+channel_subscriptions+user_subscriptions+channel_check_subscription+chat_login";

    QUrlQuery query;

    query.addQueryItem("response_type", "token");
    query.addQueryItem("client_id", appID);
    query.addQueryItem("redirect_uri", "http://oauth.abyle.org/");
    query.addQueryItem("scope", scope);

    url.setQuery(query);

    QDesktopServices::openUrl(QUrl(url));
}

void Widget::on_saveButton_clicked()
{
    save_common_settings();

    save_custom_commands_settings();
}

void Widget::_log(QString string, Qt::GlobalColor color)
{
    if (!string.endsWith('\n'))
        string += "\n";

    QListWidgetItem * item = new QListWidgetItem(string);

    switch(color) {
        case Qt::red:
            item->setBackgroundColor(Qt::red);
            item->setTextColor(Qt::white);
            break;
        case Qt::yellow:
            item->setBackgroundColor(Qt::yellow);
            item->setTextColor(Qt::black);
            break;
        case Qt::green:
            item->setBackgroundColor(Qt::green);
            item->setTextColor(Qt::blue);
            break;
        default:
            item->setBackgroundColor(Qt::lightGray);
            item->setTextColor(Qt::black);
    }

    ui->log->addItem(item);
    ui->log->setCurrentItem(item);
}

void Widget::_chat(QString string)
{
    QListWidgetItem * item = new QListWidgetItem(string);
    ui->chat->addItem(item);
    ui->chat->setCurrentItem(item);
}

void Widget::on_updateStreamStateButton_clicked()
{
    disconnect_from_stream();
    connect_to_stream();
}

void Widget::on_connectButton_clicked()
{
    if (chatConnected && !ui->forceReconnectBox->isChecked()) {
        _log("Already connected to chat " + currentChatName, Qt::yellow);
    } else {
        if (chatConnected) {
            disconnect_from_chat();
        }
        connect_to_chat();
    }

    if (streamConnected && !ui->forceReconnectBox->isChecked()) {
        _log("Already connected to stream " + currentStreamName, Qt::yellow);
    } else {
        if (streamConnected) {
            disconnect_from_stream();
        }
        connect_to_stream();
    }
}

void Widget::connect_to_chat()
{
    _log("Connecting to " + ui->channelEdit->text() + " chat...");
    chat->join(ui->authTokenEdit->text(), ui->usernameEdit->text(), ui->channelEdit->text());
    chatConnected = true;
}

void Widget::connect_to_stream()
{
    _log("Checking " + ui->channelEdit->text() + " stream...");

    TwitchAPI::instance().set_auth_token(ui->authTokenEdit->text());
    QMap<QString, QString> stream_info = TwitchAPI::instance().getStreamInfo(ui->channelEdit->text());

    if (stream_info["offline"] == "false") {
        ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(0,255,0); color : blue; font: 10pt \"MS Shell Dlg 2\"; }");
        ui->streamOnlineLabel->setText("Online");
        _log("Connected to " + ui->channelEdit->text().toUtf8() +  " stream!", Qt::green);
        streamInfoTimer->start(streamInfoTimerTimeout);
        streamConnected = true;
    } else {
        _log("Stream " + ui->channelEdit->text().toUtf8() + " is currently offline.", Qt::yellow);
        ui->streamOnlineLabel->setText("Offline");
        ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
        streamInfoTimer->stop();
        streamConnected = false;
    }
    currentStreamName = ui->channelEdit->text();\
    bot->set_cur_channel(currentStreamName);

    ui->viewersLabel->setText(stream_info["viewers"]);
    ui->fpsLabel->setText(stream_info["average_fps"]);
}

void Widget::disconnect_from_chat()
{
    chat->quit();
    _log("Disconnected from chat " + currentChatName + "\n");
    ui->chatConnectedLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
    ui->chatConnectedLabel->setText("Disconnected");
    chatConnected = false;
    ui->chatBox->setTitle("Chat");
}

void Widget::disconnect_from_stream()
{
    //_log("Disconnected from stream " + currentStreamName + "\n");
    ui->streamOnlineLabel->setText("Offline");
    ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
    streamConnected = false;
    streamInfoTimer->stop();
}

void Widget::onNewChatMsg(const IRCMsgParser::IRCMsg & msg)
{
    if (msg.type == IRCMsgParser::WELCOME) {

        _log("Connected to " + ui->channelEdit->text() + "'s chat succesfully!\n", Qt::green);
        ui->chatConnectedLabel->setStyleSheet("QLabel { background-color : rgb(0,255,0); color : blue; font: 10pt \"MS Shell Dlg 2\"; }");
        ui->chatConnectedLabel->setText("Connected");
        chatConnected = true;
        if (ui->channelEdit->text() != currentChatName) {
            ui->chat->clear();
        }
        currentChatName = ui->channelEdit->text();
        ui->chatBox->setTitle("Chat (Connected to " + currentChatName + ")");

    } else if (msg.type == IRCMsgParser::ERR_AUTH_TOKEN) {
        _log("Login unsuccessful. Possibly wrong oauth token.\n", Qt::red);
    } else if (msg.type == IRCMsgParser::ERR_LOGIN) {
        _log("Login unsuccesfull. Probably wrong username or channel name.\n", Qt::red);
    }

    if (msg.type == IRCMsgParser::JOIN) {
        //socket->write(QString("PRIVMSG #" + ui->channelEdit->text() + " :" + msg.sender + ", SwiftRage / HELLO!\r\n").toLatin1());
    }
    if (msg.type == IRCMsgParser::PRIVMSG) {

        IRCMsgParser::IRCMsg msg_no_newline = msg;
        msg_no_newline.text = QString(msg.text).remove("\r\n");

        _chat(msg.sender + ": " + msg_no_newline.text);
        bot->parseNewMsg(msg_no_newline);

    }
}

void Widget::onNewBotMsg(const QString & msg)
{
    qDebug() << msg;
    chat->sendToChat(msg);
}

void Widget::on_authTokenEdit_editingFinished()
{
    TwitchAPI::instance().set_auth_token(ui->authTokenEdit->text());
}

void Widget::on_customCommandsTable_clicked(const QModelIndex &index)
{
    if (index.column() == 1) {

        QString cmd = ui->customCommandsTable->item(index.row(), 0)->text();

        if (ui->customCommandsTable->item(index.row(), index.column())->text() == "On") {
            CUSTOM_COMMANDS_SETTINGS[cmd]["IsOn"] = "false";
            ui->customCommandsTable->item(index.row(), index.column())->setText("Off");
            ui->customCommandsTable->item(index.row(), index.column())->setTextColor(QColor(Qt::black));
            ui->customCommandsTable->item(index.row(), index.column())->setBackgroundColor(QColor(212,212,212));
        } else {
            CUSTOM_COMMANDS_SETTINGS[cmd]["IsOn"] = "true";
            ui->customCommandsTable->item(index.row(), index.column())->setText("On");
            ui->customCommandsTable->item(index.row(), index.column())->setTextColor(QColor(Qt::blue));
            ui->customCommandsTable->item(index.row(), index.column())->setBackgroundColor(QColor(Qt::green));
        }

        bot->update_single_cmd_settings(cmd);

    }
}

void Widget::on_pushButton_2_clicked()
{
    add_row_to_table(ui->customCommandsTable, "", "false", "5000", "");
    ui->customCommandsTable->editItem(ui->customCommandsTable->item(0,0));
}

void Widget::add_row_to_table(QTableWidget * table, const QString & command, const QString & isOn, const QString & minDelayMsec, const QString & message)
{
    //qDebug() << minDelayMsec;
    table->insertRow(0);

    QTableWidgetItem * itemCmd = new QTableWidgetItem(command);
    table->setItem(0, 0, itemCmd);

    QTableWidgetItem * itemIsOn = new QTableWidgetItem("");
    if (isOn == "true") {
        itemIsOn->setText("On");
        itemIsOn->setTextColor(QColor(Qt::blue));
        itemIsOn->setBackgroundColor(QColor(Qt::green));
    } else {
        itemIsOn->setText("Off");
        itemIsOn->setTextColor(QColor(Qt::black));
        itemIsOn->setBackgroundColor(QColor(212,212,212));
    }

    Qt::ItemFlags eFlags = itemIsOn->flags();
    eFlags &= ~Qt::ItemIsEditable;
    itemIsOn->setFlags(eFlags);
    itemIsOn->setTextAlignment(Qt::AlignCenter);
    table->setItem(0, 1, itemIsOn);

    QTableWidgetItem * itemDelay = new QTableWidgetItem(minDelayMsec);
    table->setItem(0, 2, itemDelay);

    if (table == ui->customCommandsTable) {
        QTableWidgetItem * itemMsg = new QTableWidgetItem(message);
        table->setItem(0, 3, itemMsg);
    }
}

void Widget::on_sendStopAnnouncementButton_clicked()
{
    if ((ui->sendStopAnnouncementButton->text() == "Send") && (ui->announcementTextEdit->toPlainText() != "")) {
        if (!ui->repeatAnnouncementCheckBox->isChecked()) {
            chat->sendToChat(ui->announcementTextEdit->toPlainText());
        } else {
            if (ui->announcementRepeatTime->text().toInt() == 0) {
                _log("Please set announcement repeat time.");
            } else {
                bot->start_repeating_message(ui->announcementTextEdit->toPlainText(), ui->announcementRepeatTime->text().toInt());
                ui->sendStopAnnouncementButton->setText("Stop");
            }
        }

    } else {

        bot->stop_repeating_message();
        ui->sendStopAnnouncementButton->setText("Send");
    }
}

void Widget::on_greetNewSubCheckBox_stateChanged(int state)
{
    bot->set_subs_greeting_state(state);
}

void Widget::on_greetingMsgTextEdit_textChanged()
{
    bot->set_subs_greeting_msg(ui->greetingMsgTextEdit->toPlainText());
}

void Widget::on_customCommandsTable_cellChanged(int row, int column)
{
    QString cmd = ui->customCommandsTable->item(row, 0)->text();

    if (column == 0) {
        // ...
    }

    if (column == 2) {
        CUSTOM_COMMANDS_SETTINGS[cmd]["MinDelayMsec"] = ui->customCommandsTable->item(row, column)->text();
    }

    if (column == 3) {
        CUSTOM_COMMANDS_SETTINGS[cmd]["Message"] = ui->customCommandsTable->item(row, column)->text();
    }
}

void Widget::on_greetNewSubCheckBox_clicked()
{
    bot->set_subs_greeting_state(ui->greetNewSubCheckBox->isChecked());
}

void Widget::on_pushButton_3_clicked()
{
    //chat->enableWhispers();
    //chat->sendToChat("/w Inferna1e 123");
}
