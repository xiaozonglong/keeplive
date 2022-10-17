#ifndef KEEPLIVE_H
#define KEEPLIVE_H

#include <QObject>
#include "app.h"
#include <QUdpSocket>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QCoreApplication>
#ifdef MACRO_LOCKFILE
#include <QLockFile>
#endif
#include <QFileInfoList>
class KeepLive:public QObject
{
    Q_OBJECT

public:
    KeepLive(QObject *parent = nullptr);
    ~KeepLive();
    void initVar();
    #ifdef MACRO_LOCKFILE
    void initLock();
#endif
    void initService();

    QTimer *_timerHeart = nullptr;     //心跳定时器
    QUdpSocket *_udp = nullptr;        //UDP通信对象
    int count = 0;              //计数
    bool ok = false;                //是否正常

    void pause();
    void resume();
private:
    qlonglong _packetID = 0; //当前发送包ID
    bool _keeplive = true;
    QByteArray send_heart();
    QByteArray send_cmdResponse(QVariantMap recvmap);
    static bool isExistProcess(QString name);
    static QStringList scanFolder(QString path, QString filter);
    static QFileInfoList scanFolderFile(QString path);
    bool _isExistProcess();

    void _killProcess();
    void beginTimeOutTimer();
    
    QString appNamePath();
    
Q_SIGNALS:
    void serviceCmd(QString cmd,bool enable);
private slots:
    void killApp();
    void killOther();
    void startApp();

    void runCommand(QString cmd);
};

#endif // LIVE_H
