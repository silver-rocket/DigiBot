#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QDesktopServices>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QTime>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QVariant>
#include <QFile>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QMessageBox>
#include <QListWidget>
#include <QTableWidget>
#include <QThread>

#include "ircmsgparser.h"
#include "simplexmlreadwrite.h"
#include "twitchapi.h"
#include "botengine.h"
#include "chatengine.h"
#include "settings.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

    static const size_t streamInfoTimerTimeout = 5000;
    static const QString mainWindowTitle;
    static const QString appID;
    static const QString settingsFile;
    static const QString commandsSettingsFile;
    static const QString customCommandsFile;

    QTimer * streamInfoTimer;

    QMap<QString, QString> streamInfo;
    QMap<QString, QString> QADataBase;

    bool chatConnected;
    bool streamConnected;

    QString currentStreamName;
    QString currentChatName;

    ChatEngine * chat;
    BotEngine * bot;

private:

    void load_common_settings();
    void load_commands_settings();
    void load_custom_commands_settings();
    void load_QA();

    void save_common_settings();
    void save_commands_settings();
    void save_custom_commands_settings();

    void _log(QString string, Qt::GlobalColor color = Qt::white);
    void _chat(QString string);

    void connect_to_chat();
    void connect_to_stream();

    void disconnect_from_chat();
    void disconnect_from_stream();

    void add_row_to_table(QTableWidget * table,
                          const QString & command,
                          const QString & isOn,
                          const QString & minDelayMsec,
                          const QString & message = "");

private slots:

    void on_pushButton_clicked();
    void onTimerTimeout();

    void on_getAuthButton_clicked();
    void on_saveButton_clicked();
    void on_connectButton_clicked();
    void on_updateStreamStateButton_clicked();

    void onNewChatMsg(const IRCMsgParser::IRCMsg & msg);
    void onNewBotMsg(const QString & msg);
    void on_authTokenEdit_editingFinished();
    void on_pushButton_2_clicked();
    void on_customCommandsTable_clicked(const QModelIndex &index);
    void on_sendStopAnnouncementButton_clicked();
    void on_greetNewSubCheckBox_stateChanged(int arg1);
    void on_greetingMsgTextEdit_textChanged();
    void on_customCommandsTable_cellChanged(int row, int column);
    void on_greetNewSubCheckBox_clicked();
};

#endif // WIDGET_H
