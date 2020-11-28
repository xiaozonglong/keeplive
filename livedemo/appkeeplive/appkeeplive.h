#ifndef APPKEEPLIVE_H
#define APPKEEPLIVE_H

#include <QObject>
#include <QUdpSocket>

class AppKeepLive : public QObject
{
    Q_OBJECT
public:
    static AppKeepLive *Instance();
    explicit AppKeepLive(QObject *parent = nullptr);
    quint16 getBindPort();
private:
    static QScopedPointer<AppKeepLive> self;
    QUdpSocket *_udpServer = nullptr;
    QString appName;
    QHostAddress _senderAddress;
    int _senderPort ;
    void sendRemoteCMD(QString cmdobject, bool enable);
    bool RemotePause = false;
    bool RemoteStart = true;
Q_SIGNALS:
    void heartbeat(QString context);
    void keeplive(bool enable);
private slots:
    void readData();
    void setRemotePause(bool enable);
    bool getRemotePause();
#ifdef MACRO_DEBUG
    void setRemoteStart(bool enable);
    bool getRemoteStart();

    void RemoteRestart();
#endif
public slots:
    bool start(int port);
    void stop();
};

#endif // APPKEEPLIVE_H
