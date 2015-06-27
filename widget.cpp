#include "widget.h"
#include "ui_widget.h"

const QString Widget::mainWindowTitle = "Digibot v 1.0.0";

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle(mainWindowTitle);

    load_settings();
    load_QA();

    chatConnected = false;
    streamConnected = false;

    streamInfoTimer = new QTimer(this);
    connect(streamInfoTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readIRCData()));

    /*
    ui->webView->load(url);
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(check_url(QUrl)));
    */

}

Widget::~Widget()
{
    delete ui;
}

void Widget::check_url(QUrl url) {

    qDebug() << "URL:\n" << url.toString() << '\n';

    /*
    url = url.toString().replace("#","?");
    QString token = url.queryItemValue("access_token");

    if (token.isEmpty()) {
        qDebug() << "Something went wrong.";
        return;
    }

    qDebug() << "Token: " << token << '\n';
    */
}

void Widget::readIRCData() {

    QString line = socket->readLine();

    if (line.contains("Welcome, GLHF")) {

        _log("Connected to " + ui->channelEdit->text() + "'s chat succesfully!\n", Qt::green);
        ui->chatConnectedLabel->setStyleSheet("QLabel { background-color : rgb(0,255,0); color : blue; font: 10pt \"MS Shell Dlg 2\"; }");
        ui->chatConnectedLabel->setText("Connected");
        chatConnected = true;
        if (ui->channelEdit->text() != currentChatName) {
            ui->chat->clear();
        }
        currentChatName = ui->channelEdit->text();
        ui->chatBox->setTitle("Chat (Connected to " + currentChatName + ")");

    } else if (line.contains("Login unsuccessful")) {
        _log("Login unsuccessful. Possibly wrong oauth token.\n", Qt::red);
    } else if (line.contains("Error logging in")) {
        _log("Login unsuccesfull. Probably wrong username or channel name.\n", Qt::red);
    }

    IRCMsgParser::IRCMsg msg = IRCMsgParser::parse(line);

    qDebug() << line;
    //qDebug() << msg.type << ' ' << msg.sender << ' ' << msg.text;

    if (msg.type == IRCMsgParser::PING)
        socket->write(QString("PONG :" + msg.text).toLatin1());

    if (msg.type == IRCMsgParser::JOIN) {
        //socket->write(QString("PRIVMSG #" + ui->channelEdit->text() + " :" + msg.sender + ", SwiftRage / HELLO!\r\n").toLatin1());
    }
    if (msg.type == IRCMsgParser::PRIVMSG) {

        _chat(msg.sender + ": " + msg.text.remove("\r\n"));

    }
    if(socket->canReadLine())
        readIRCData();
}

void Widget::on_pushButton_clicked()
{
    int n = 10;
    QList<QString> last_followers = twitchAPI.getLastFollowers(currentStreamName, ui->authTokenEdit->text(), n);

    if (last_followers.size() != n) {
        _log("Can't retrieve last followers. Check stream connection.", Qt::red);
    }
    else {
        ui->lastFollowers->clear();
        for (int i=0; i<n; i++) {
            ui->lastFollowers->addItem(last_followers[i]);
        }
    }

    return;

    //QUrl current("https://api.twitch.tv/kraken/streams/followed");
    //QUrl current("https://api.twitch.tv/kraken/user");
    //QUrl current("https://api.twitch.tv/kraken/streams/starladder4");
    QUrl current("https://api.twitch.tv/kraken/channels/Starladder1/follows");
    //QUrl current("https://api.twitch.tv/kraken/channels/thewide001/subscriptions");

    QUrlQuery param;
    param.addQueryItem("limit", "3");
    current.setQuery(param);

    QByteArray answer = twitchAPI.GET(current, ui->authTokenEdit->text());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer);

    QJsonObject jsonObj = jsonResponse.object();

    qDebug() << jsonObj["follows"].toArray();

    for(QJsonObject::const_iterator iter = jsonObj.begin(); iter != jsonObj.end(); ++iter) {

        if (iter.key() == "follows") { // follows subscriptions

            QJsonArray jsonObj2 = iter.value().toArray();

            for(QJsonArray::const_iterator iter2 = jsonObj2.begin(); iter2 != jsonObj2.end(); ++iter2) {

                QJsonObject jsonObj2 = (*iter2).toObject();
                qDebug() << jsonObj2["user"].toObject()["display_name"].toString();
                //qDebug() << '\n';

            }

        }

    }

    //qDebug() << answer;

}

void Widget::on_chatConnectButton_clicked()
{
    connect_to_chat();
}

void Widget::on_chatDisconnectButton_clicked()
{
    disconnect_from_chat();
}

void Widget::onTimerTimeout() {

    //QMap<QString, QString> stream_info = getStreamInfo(currentStreamName);
    QMap<QString, QString> stream_info = twitchAPI.getStreamInfo(currentStreamName, ui->authTokenEdit->text());

    if (stream_info["offline"] == "true") {
        streamInfoTimer->stop();
    }

    ui->viewersLabel->setText(stream_info["viewers"]);
    ui->fpsLabel->setText(stream_info["average_fps"]);

}

void Widget::load_settings()
{
    SimpleXMLReadWrite xml("DigiBotSettings.xml");
    settings = xml.readFile();
    ui->channelEdit->setText(settings["Channel"]);
    ui->usernameEdit->setText(settings["Username"]);
    ui->authTokenEdit->setText(settings["Auth_token"]);
}

void Widget::load_QA()
{
    QFile file(":/data/Answers.txt");

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Can't open QA database");
        return;
    }

    QTextStream stream(&file);

    stream.setCodec("cp-1251");

    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        QRegExp regex("(.+) : (.+)");

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

void Widget::on_getAuthButton_clicked()
{
    QString auth_path = "https://api.twitch.tv/kraken/oauth2/authorize";
    QUrl url(auth_path);

    QString scope = "user_read+user_blocks_edit+user_blocks_read+user_follows_edit+channel_read+"
            "channel_editor+channel_commercial+channel_stream+channel_subscriptions+user_subscriptions+channel_check_subscription+chat_login";

    QUrlQuery query;

    query.addQueryItem("response_type", "token");
    query.addQueryItem("client_id", "jv1zuv3ifjgbt45ubipfk1ng7khpnhi");
    query.addQueryItem("redirect_uri", "http://oauth.abyle.org/");
    query.addQueryItem("scope", scope);

    url.setQuery(query);

    QDesktopServices::openUrl(QUrl(url));
}

void Widget::on_saveButton_clicked()
{
    SimpleXMLReadWrite xml("DigiBotSettings.xml");
    settings["Username"] = ui->usernameEdit->text();
    settings["Channel"] = ui->channelEdit->text().toLower();
    settings["Auth_token"] = ui->authTokenEdit->text();
    xml.writeFile(settings);
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

void Widget::on_reconnectToStreamButton_clicked()
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

    socket->connectToHost(QString("irc.twitch.tv"), 6667);
    socket->write("PASS oauth:" + ui->authTokenEdit->text().toUtf8() + "\r\n");
    socket->write("NICK " + ui->usernameEdit->text().toUtf8() + "\r\n");
    socket->write("JOIN #" + ui->channelEdit->text().toUtf8() + "\r\n");
}

void Widget::connect_to_stream()
{
    _log("Connecting to " + ui->channelEdit->text() + " stream...");

    //QMap<QString, QString> stream_info = getStreamInfo(ui->channelEdit->text());
    QMap<QString, QString> stream_info = twitchAPI.getStreamInfo(ui->channelEdit->text(), ui->authTokenEdit->text());

    if (stream_info["offline"] == "false") {
        ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(0,255,0); color : blue; font: 10pt \"MS Shell Dlg 2\"; }");
        ui->streamOnlineLabel->setText("Online");
        _log("Connected to " + ui->channelEdit->text().toUtf8() +  " stream!", Qt::green);
        streamInfoTimer->start(streamInfoTimerTimeout);
        streamConnected = true;
        currentStreamName = ui->channelEdit->text();
    } else {
        _log("Stream " + ui->channelEdit->text().toUtf8() + " is currently offline.", Qt::yellow);
        ui->streamOnlineLabel->setText("Offline");
        ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
        streamInfoTimer->stop();
        streamConnected = false;
    }

    ui->viewersLabel->setText(stream_info["viewers"]);
    ui->fpsLabel->setText(stream_info["average_fps"]);

}

void Widget::disconnect_from_chat()
{
    socket->write("QUIT\r\n");
    socket->flush();
    socket->disconnectFromHost();
    _log("Disconnected from chat " + currentChatName + "\n");
    ui->chatConnectedLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
    ui->chatConnectedLabel->setText("Disconnected");
    chatConnected = false;
    ui->chatBox->setTitle("Chat");
}

void Widget::disconnect_from_stream()
{
    _log("Disconnected from stream " + currentStreamName + "\n");
    ui->streamOnlineLabel->setText("Offline");
    ui->streamOnlineLabel->setStyleSheet("QLabel { background-color : rgb(212,212,212); color : black; font: 10pt \"MS Shell Dlg 2\"; }");
    streamConnected = false;
    streamInfoTimer->stop();
}

