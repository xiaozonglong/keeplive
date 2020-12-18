#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "keeplive.h"
#include <QJsonDocument>
#include <QNetworkDatagram>
#include <QDir>
#include "process_loader.h"

#ifdef Q_OS_LINUX
#include "isyslinux.h"
#else
#include "ISysWin.h"
#endif

QFileInfoList KeepLive::scanFolderFile(QString path)
{
    //====需要查找的路径
    QDir fileDir(path);

    fileDir.setFilter(QDir::Files); //===设置过滤配置,接受文件

    QFileInfoList fileInfoList = fileDir.entryInfoList();
    return fileInfoList;
}
QStringList KeepLive::scanFolder(QString path,QString filter)
{
    QFileInfoList fileInfoList = scanFolderFile(path);

    QStringList fileList;

    foreach (auto info , fileInfoList) {
        //====如果需要筛选指定文件可以在这里添加判断

        if(!filter.isEmpty())
        {
            QString suffix = info.suffix();
            if(suffix == filter)
            {
                QString baseName = info.baseName();

                fileList<<baseName;
            }
        }else
        {
            QString fileName = info.fileName();


            fileList<<fileName;
        }

    }
    return fileList;
}
KeepLive::KeepLive(QObject *parent):QObject(parent)
{
    initVar();
#ifdef MACRO_LOCKFILE
    if(/* DISABLES CODE */ (0))initLock();
#endif
    initService();
    startApp();
}

KeepLive::~KeepLive()
{

    bool ExitDestoryApp = App::ExitDestoryApp;
    if(ExitDestoryApp == true)
    {
        killApp();
    }

    if(_udp)
    {
        _udp->deleteLater();
    }
}

void KeepLive::initVar()
{
    QString applicationDirPath = qApp->applicationDirPath();
    QString dirPath = QDir::currentPath();

    if(applicationDirPath != dirPath)
    {
        QDir::setCurrent(applicationDirPath);
    }

    //预加载自身配置
    QString applicationName = QCoreApplication::applicationName();
    App::ConfigFile = applicationDirPath + "/"+applicationName+"_config.ini";
    App::ReStartLastTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    App::TargetAppPort = 60001;
    App::AppStartupTime = 7;
    App::TimeoutCount = 10000;
    App::ReStartExplorer = false;
    App::ReStartCount = 0;
    App::TimerHeartInterval = 5000;
    App::UIEnable = true;
    App::DestoryApp = true;
    App::ExitDestoryApp = true;
    App::SuffixAppName = "exe";
    App::TargetAppName = "";
    App::OperateParameter = "";
    App::readConfig();

    //默认扫描当前目录下的守护exe
    if(App::TargetAppName.isEmpty())
    {
        QStringList flist = scanFolder("./",App::SuffixAppName);
        {
            foreach (auto appfile, flist) {
                if(appfile == applicationName)continue;
                App::TargetAppName =appfile;
                break;
            }
        }
    }
    //扫描不到，默认守护livedemo.exe
    if(App::TargetAppName.isEmpty())
    {
        App::TargetAppName = "livedemo";
    }

    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")<<endl<<"-------------------------------服务启动中------------------------------------------"<<endl
           <<"ServiceName()-->"<<applicationName<<endl
          <<"LiveProcessName()-->"<<App::TargetAppName<<App::SuffixAppName<<endl
         <<"applicationDirPath()-->"<<applicationDirPath<<endl
           ;

}
#ifdef MACRO_LOCKFILE
/*
 * APP锁功能，会在本地生产.lock文件
*/
void KeepLive::initLock()
{
    QString applicationName = QCoreApplication::applicationName();
    QString path = QDir::currentPath()+"/"+applicationName+(".lock");
    // path = C:/Users/yu/AppData/Local/Temp/SingleAppTest.lock
    QLockFile *plockfile = new QLockFile(path);

    connect(this,&KeepLive::destroyed,this,[=](){
        delete plockfile;
    });

    // bool QLockFile::tryLock(int timeout = 0)
    // tryLock尝试创建锁定文件。此函数如果获得锁，则返回true; 否则返回false。
    // 如果另一个进程（或另一个线程）已经创建了锁文件，则此函数将最多等待timeout毫秒


    if (!plockfile->tryLock(100))
    {
        qDebug()<<("The application is already running.\n"
                   "Allowed to run only one instance of the application.");
        QTimer::singleShot(5000,this,[=](){
            exit(-4);
        });
    }else
    {
        qDebug()<<("The application is locked.\n");
    }

}
#endif
QByteArray KeepLive::send_heart()
{
    QByteArray retstr;
    if(_packetID == 0)
    {
        _packetID = QDateTime::currentDateTime().toTime_t();
    }
    QVariantMap mainkvs;
    {
        {

            QVariantMap kvs;
            kvs.insert("object",this->metaObject()->className());
            kvs.insert("value",_keeplive);
            kvs.insert("DateTime",QDateTime::currentDateTime());
            kvs.insert("TargetAppPort",App::TargetAppPort);
            kvs.insert("TargetAppName",App::TargetAppName);
            kvs.insert("SuffixAppName",App::SuffixAppName);
            kvs.insert("OperateParameter",App::OperateParameter);
            kvs.insert("ConfigFile",App::ConfigFile);

            kvs.insert("TimeoutCount",App::TimeoutCount);
            kvs.insert("ReStartCount",App::ReStartCount);
            kvs.insert("ReStartLastTime",App::ReStartLastTime);
            kvs.insert("ReStartExplorer",App::ReStartExplorer);
            kvs.insert("UIEnable",App::UIEnable);
            kvs.insert("DestoryApp",App::DestoryApp);
            kvs.insert("TimerHeartInterval",App::TimerHeartInterval);
            kvs.insert("AppDirPath",qApp->applicationDirPath());
            kvs.insert("AppPid",qApp->applicationPid());


            kvs.insert("count",count);
            kvs.insert("ok",ok);

            mainkvs.insert("payload",kvs);
        }

    }
    mainkvs.insert("messagetype","update");



    {

        mainkvs.insert("packetID",_packetID);
        mainkvs.insert("soft_version","1.0");


        QJsonDocument doc=QJsonDocument::fromVariant(QVariant(mainkvs));
        auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);
        retstr = jba;

    }
    return retstr;
}

QByteArray KeepLive::send_cmdResponse(QVariantMap recvmap)
{
    QByteArray retstr;
    {
        auto packetID = recvmap.value("packetID").toUInt();
        auto object = recvmap.value("object").toString();
        auto payloadmap = recvmap.value("payload").toMap();
        if(object == "service_status" )
        {
            auto command = recvmap.value("command").toString();
            if(command == "write")
            {
                if(!payloadmap.contains("object"))return retstr;


                //消息回复
                QVariantMap mainkvs;
                mainkvs.insert("packetID",packetID);
                mainkvs.insert("soft_version","1.0");
                mainkvs.insert("messagetype","response");

                {
                    QVariantMap kvs;
                    kvs.insert("object",payloadmap.value("object"));
                    kvs.insert("value",payloadmap.value("value"));
                    mainkvs.insert("payload",kvs);
                }
                QJsonDocument doc=QJsonDocument::fromVariant(QVariant(mainkvs));
                auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);


                retstr = jba;

                //发送信号
                if(payloadmap.contains("object"))
                {
                    auto payloadobject = payloadmap.value("object").toString();
                    auto payloadobjectvalue = payloadmap.value("value").toBool();

                    emit serviceCmd(payloadobject,payloadobjectvalue);
                }
                else
                {

                }

            }
        }else
        {
            qWarning() <<"解析数据有错误,未识别的相应的字段 object"<<object;
        }


    }
    return retstr;
}

void KeepLive::initService()
{
    //每秒钟定时询问心跳
    _timerHeart = new QTimer(this);
    connect(_timerHeart,&QTimer::destroyed,this,[=](){
        qDebug()<<"QTimer::destroyed";
        _timerHeart = nullptr;
    });
    auto TimerHeartInterval = App::TimerHeartInterval;
    _timerHeart->setInterval(TimerHeartInterval);
    connect(_timerHeart, &QTimer::timeout, this, [=](){

        if(_udp == nullptr)
        {
            qWarning()<<"_udp == nullptr";
            return;
        }


        auto jba =  send_heart();
        if(!jba.isEmpty()){
            auto sendsize = _udp->writeDatagram(jba, QHostAddress::LocalHost, App::TargetAppPort);
        }

        //判断当前是否没有回复
        if (!ok) {
            count++;
        } else {
            count = 0;
            ok = false;
        }

        if(_keeplive == true)
        {
            //如果超过规定次数没有收到心跳回复,则超时重启
            if (count >= App::TimeoutCount) {
                _timerHeart->stop();
                if(_packetID != 0)
                {
                    qWarning()<<"没有响应数据包 packetID="<<_packetID<<count<<App::TimeoutCount;
                    _packetID = 0;
                }

                killApp();

                //killother
                QTimer::singleShot(1000 , this, SLOT(killOther()));
                QTimer::singleShot(3000 , this, SLOT(startApp()));
                //            QTimer::singleShot(4000 , this, SLOT(startExplorer()));
            }
        }
    });

    if(_udp == nullptr)
    {
        _udp = new QUdpSocket(this);
        connect(_udp,&QUdpSocket::destroyed,this,[=](){
            qDebug()<<"QUdpSocket::destroyed";
            _udp = nullptr;
        });


        connect(_udp, &QUdpSocket::readyRead, this, [=](){

            while(_udp->hasPendingDatagrams())//拥有等待的数据报
            {

                QNetworkDatagram datagram = _udp->receiveDatagram();

                //        _ser->readDatagram(datagramba.data(), datagramba.size(),
                //                           &senderAddress, &senderPort);
                QByteArray datagramba = datagram.data();
                auto senderAddress = datagram.senderAddress();
                auto senderPort = datagram.senderPort();

                if (!datagramba.isEmpty())
                {
                    QJsonDocument recvdata = QJsonDocument::fromJson(datagramba);

                    if(recvdata.isNull())break;

                    auto recvmap = recvdata.toVariant().toMap();

                    auto messagetype = recvmap.value("messagetype").toString();

                    if(messagetype == "response")
                    {
                        auto packetID = recvmap.value("packetID").toUInt();

                        if(packetID == _packetID)
                        {
                            _packetID = 0;
                            count = 0;
                            ok = true;
                        }else
                        {
                            qWarning() <<"回复数据有错误,相应的字段参数不符 packetID"<<packetID<<_packetID;
                        }
                    }else if(messagetype == "update")
                    {
                        auto jba = send_cmdResponse(recvmap);
                        if(!jba.isEmpty())
                        {
                            auto sendsize = _udp->writeDatagram(jba, senderAddress, senderPort);
                        }

                    }
                    else
                    {
                        qWarning() <<"回复数据有错误,未识别的相应的字段 messagetype"<<messagetype;
                    }

                }

            }
        });
    }



}

void KeepLive::pause()
{
#if 1
    _keeplive = false;
#else
    if(_timerHeart != nullptr)
    {
        _timerHeart->stop();
    }
#endif
}

void KeepLive::resume()
{
#if 1
    _keeplive = true;
#else
    if(_timerHeart != nullptr)
    {
        _timerHeart->start();
    }
#endif
}
void KeepLive::killApp()
{  
    bool DestoryApp = App::DestoryApp;
    if(DestoryApp == true)
    {

        if(_isExistProcess())
        {
            _killProcess();
        }
    }
}

void KeepLive::killOther()
{
#ifdef Q_OS_LINUX
#else
    {
        QString tappname = QString("WerFault")+ "."+App::SuffixAppName;
        if(isExistProcess(tappname))
        {
            QString cmd = QString("taskkill /im %1 /f").arg(tappname);
            runCommand(cmd);
        }
    }


    //重建缓存,彻底清除托盘图标
    if (App::ReStartExplorer) {
        QString tappname = QString("explorer")+ "."+App::SuffixAppName;
        if(isExistProcess(tappname))
        {
            QString cmd = QString("taskkill /f /im %1").arg(tappname);
            runCommand(cmd);
        }
    }
#endif
}
void KeepLive::_killProcess()
{
    QString appname;
    if(App::SuffixAppName.isEmpty())
    {
        appname = QString("%1").arg(App::TargetAppName);
    }else
    {
        appname = QString("%1.%2").arg(App::TargetAppName).arg(App::SuffixAppName);
    }
#ifdef Q_OS_LINUX
    ISysLinux::killAll(appname);
#elif defined (Q_OS_WIN)
    QString cmd = QString("taskkill /im %1 /f").arg(appname);
    runCommand(cmd);
#endif

}
bool KeepLive::_isExistProcess()
{
    QString appname;
    if(App::SuffixAppName.isEmpty())
    {
        appname = QString("%1").arg(App::TargetAppName);
    }else
    {
        appname = QString("%1.%2").arg(App::TargetAppName).arg(App::SuffixAppName);
    }
    return isExistProcess(appname);

}

void KeepLive::beginTimeOutTimer()
{
    QTimer::singleShot(App::TimeoutCount,this,[=](){
        if (App::TargetAppName.isEmpty()) {

            _timerHeart->stop();
        } else {

            _timerHeart->start();
        }
    });
}

void KeepLive::startApp()
{
    if(_isExistProcess())
    {
        qDebug()<<"ExistProcess";
    }else
    {
        bool uienable = App::UIEnable;
        QString apppath;
        if(App::SuffixAppName.isEmpty())
        {
            apppath = QString("%1/%2").arg(qApp->applicationDirPath()).arg(App::TargetAppName);
        }else
        {
            apppath = QString("%1/%2.%3").arg(qApp->applicationDirPath()).arg(App::TargetAppName).arg(App::SuffixAppName);
        }


        if(QFile::exists (apppath))
        {
            QStringList arguments;

            if(!App::OperateParameter.isEmpty())
            {

                arguments = App::OperateParameter.split(' ');
            }

            QString appname1 = QString("\"%1\"").arg(apppath);
            if(/* DISABLES CODE */ (0)){
                runCommand(appname1);
            }else if(/* DISABLES CODE */ (!uienable)){//启动不带UI
                auto ret = QProcess::startDetached(appname1,arguments);
                qDebug()<<"QProcess::startDetached "<<appname1<<arguments<<ret;
            }else//启动带UI
            {
#ifdef Q_OS_WIN
                std::wstring command = appname1.toStdWString();
                if (ProcessLoader::loadWindowsApplication(command) == false) {
                    qWarning() <<apppath<< "Failed to launch " << command.c_str()<<"1.请在“管理服务”中启动程序；2检查指定程序不能设置为管理员权限！";
                }
#else
                auto ret = QProcess::startDetached(apppath,arguments);
                qDebug()<<"QProcess::startDetached "<<apppath<<arguments<<ret;
#endif
            }
            App::ReStartCount++;
            App::ReStartLastTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            App::writeConfig();
        }else{
            qWarning() <<apppath<<"不存在 Failed to launch ";
        }

    }


    count = 0;
    ok = true;

    beginTimeOutTimer();
}

bool KeepLive::isExistProcess(QString name)
{
    bool isexist = false;
#ifdef Q_OS_WIN
    //    QTimer::singleShot(1000,this,[=]{
    isexist = ISysWin::IsExistProcess(name.toStdString().c_str());
    //    });
#else
    isexist = ISysLinux::IsExistProcess(name.toStdString().c_str());
#endif
    return isexist;
}

void KeepLive::runCommand(QString cmd)
{
    QProcess *p = new QProcess;
    connect(p,&QProcess::destroyed,this,[=](){
        qDebug()<<"QProcess::destroyed"<<cmd<<endl;
    });
    connect(p,&QProcess::started,this,[=](){
        qDebug()<<"QProcess::started"<<cmd;
    });
    connect(p,&QProcess::errorOccurred,this,[=](QProcess::ProcessError error){
        qDebug()<<"QProcess::errorOccurred"<<error;
    });
    connect(p,&QProcess::stateChanged,this,[=](QProcess::ProcessState state){
        qDebug()<<state;
    });
    connect(p,&QProcess::readyReadStandardOutput,this,[=](){
        qDebug()<<"QProcess::readyReadStandardOutput-->"<<p->readAllStandardOutput();
    });
    connect(p,&QProcess::readyReadStandardError,this,[=](){
        qDebug()<<"QProcess::readyReadStandardError-->"<<p->readAllStandardError();
    });
    connect(p,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),this,[=](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitCode)
        qDebug()<<"QProcess::exitStatus = "<<exitStatus
                  //               <<p->program()
                  //              <<p->arguments()
                  ;
        delete p;
    },Qt::UniqueConnection);
    p->start(cmd);
}


