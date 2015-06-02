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
#include <QList>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QVariant>
#include <QFile>
#include <QXmlStreamWriter>
#include <QMap>
#include <QMessageBox>
#include <QListWidget>

#include "ircmsgparser.h"
#include "simplexmlreadwrite.h"

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

    QTimer * streamInfoTimer;

    QTcpSocket * socket;

    QMap<QString, QString> settings;
    QMap<QString, QString> streamInfo;

    QMap<QString, QString> QADataBase;

    bool chatConnected;
    bool streamConnected;

    QString currentStreamName;
    QString currentChatName;

private:

    QByteArray GET(QUrl r);

    void load_settings();
    void load_QA();

    void _log(QString string, Qt::GlobalColor color = Qt::white);
    void _chat(QString string);

    QMap<QString, QString> getStreamInfo(const QString & channel);

    void connect_to_chat();
    void connect_to_stream();

    void disconnect_from_chat();
    void disconnect_from_stream();

private slots:
    void on_pushButton_clicked();
    void check_url(QUrl url);
    void onTimerTimeout();
    void readIRCData();

    void on_chatConnectButton_clicked();
    void on_chatDisconnectButton_clicked();
    void on_getAuthButton_clicked();
    void on_saveButton_clicked();
    void on_reconnectToStreamButton_clicked();
    void on_connectButton_clicked();
};


#endif // WIDGET_H
