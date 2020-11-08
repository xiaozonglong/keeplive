#ifndef APPLIVE_H
#define APPLIVE_H

#include <QObject>

class QUdpSocket;

class AppKeepLive : public QObject
{
	Q_OBJECT
public:
    static AppKeepLive *Instance();
    explicit AppKeepLive(QObject *parent = nullptr);

private:
    static QScopedPointer<AppKeepLive> self;
	QUdpSocket *udpServer;
    QString appName;

private slots:
    void readData();

public slots:
    bool start(int port);
    void stop();
};

#endif // APPLIVE_H
