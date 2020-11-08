#ifndef KEEPLIVE_H
#define KEEPLIVE_H

#include <QObject>
#include "app.h"
#include <QUdpSocket>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QCoreApplication>
#include <QLockFile>

class KeepLive:public QObject
{
    Q_OBJECT

public:
    KeepLive(QObject *parent = nullptr);
    ~KeepLive();
    void initLock();
    void initService();

    QTimer *_timerHeart = nullptr;     //心跳定时器
    QUdpSocket *_udp = nullptr;        //UDP通信对象
    int count = 0;              //计数
    bool ok = false;                //是否正常
    void pause();
    void resume();
private slots:
    void killApp();
    void killOther();
    void startApp();
    bool isExistProcess(QString name);
    void runCommand(QString cmd);
};

#endif // LIVE_H
