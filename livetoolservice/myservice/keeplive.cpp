#include "keeplive.h"
#include <QJsonDocument>
#include <QDir>
#include "process_loader.h"
#include "ISysWin.h"

KeepLive::KeepLive(QObject *parent):QObject(parent)
{
    QString applicationDirPath = qApp->applicationDirPath();
    QString dirPath = QDir::currentPath();

    if(applicationDirPath != dirPath)
    {
        QDir::setCurrent(applicationDirPath);
    }

    App::ConfigFile = applicationDirPath + "/config.ini";
    App::readConfig();

    QString applicationName = QCoreApplication::applicationName();
    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")<<endl<<"-------------------------------服务启动中------------------------------------------"<<endl
           <<"ServiceName()-->"<<applicationName<<endl
          <<"LiveProcessName()-->"<<App::TargetAppName<<endl
         <<"applicationDirPath()-->"<<applicationDirPath<<endl
           ;

    initLock();
    initService();
}

KeepLive::~KeepLive()
{
    killApp();
}

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

void KeepLive::initService()
{
    //每秒钟定时询问心跳
    _timerHeart = new QTimer(this);
    connect(_timerHeart,&QTimer::destroyed,this,[=](){
        qDebug()<<"QTimer::destroyed";
        _timerHeart = nullptr;
    });
    _timerHeart->setInterval(2000);
    connect(_timerHeart, &QTimer::timeout, this, [=](){

        if(_udp == nullptr)
        {
            qWarning()<<"_udp == nullptr";
            return;
        }

        QVariantMap kvs;
        kvs.insert("timerHeart",_timerHeart->interval());
        kvs.insert("DateTime",QDateTime::currentDateTime());
        kvs.insert("TargetAppPort",App::TargetAppPort);
        kvs.insert("TargetAppName",App::TargetAppName);
        kvs.insert("ConfigFile",App::ConfigFile);

        kvs.insert("TimeoutCount",App::TimeoutCount);
        kvs.insert("ReStartCount",App::ReStartCount);
        kvs.insert("ReStartLastTime",App::ReStartLastTime);
        kvs.insert("ReStartExplorer",App::ReStartExplorer);
        kvs.insert("applicationDirPath",qApp->applicationDirPath());


        kvs.insert("count",count);
        kvs.insert("ok",ok);
        QJsonDocument doc=QJsonDocument::fromVariant(QVariant(kvs));
        auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);




        auto sendsize = _udp->writeDatagram(jba, QHostAddress::LocalHost, App::TargetAppPort);

        Q_UNUSED(sendsize)
        //        qDebug()<<sendsize<<jba;
        //判断当前是否没有回复
        if (!ok) {
            count++;
        } else {
            count = 0;
            ok = false;
        }

        //如果超过规定次数没有收到心跳回复,则超时重启
        if (count >= App::TimeoutCount) {
            _timerHeart->stop();

            killApp();


            //killother
            QTimer::singleShot(1000 , this, SLOT(killOther()));
            QTimer::singleShot(3000 , this, SLOT(startApp()));
            //            QTimer::singleShot(4000 , this, SLOT(startExplorer()));
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
            QByteArray tempData;
            do {
                tempData.resize((int)_udp->pendingDatagramSize());
                _udp->readDatagram(tempData.data(), tempData.size());
                //没有具体分析类容，目前全部通过
                if(!tempData.isEmpty())
                {
                    count = 0;
                    ok = true;
                }

            } while (_udp->hasPendingDatagrams());
        });
    }

    if (App::TargetAppName.isEmpty()) {

        _timerHeart->stop();
    } else {

        _timerHeart->start();
    }

}

void KeepLive::pause()
{
    if(_timerHeart != nullptr)
    {
        _timerHeart->stop();
    }
}

void KeepLive::resume()
{
    if(_timerHeart != nullptr)
    {
        _timerHeart->start();
    }
}
void KeepLive::killApp()
{  
    if(isExistProcess(App::TargetAppName + ".exe"))
    {
        QString cmd = QString("taskkill /im %1.exe /f").arg(App::TargetAppName);
        runCommand(cmd);
    }
}

void KeepLive::killOther()
{
    if(isExistProcess("WerFault.exe"))
    {
        QString cmd = QString("taskkill /im %1.exe /f").arg("WerFault");
        runCommand(cmd);
    }


    //重建缓存,彻底清除托盘图标
    if (App::ReStartExplorer) {
        if(isExistProcess("explorer.exe"))
        {
            QString cmd = QString("taskkill /f /im explorer.exe");
            runCommand(cmd);
        }
    }
}

void KeepLive::startApp()
{

    QString appname = QString("\"%1/%2.exe\"").arg(qApp->applicationDirPath()).arg(App::TargetAppName);
    if(/* DISABLES CODE */ (0)){
        runCommand(appname);
    }else if(/* DISABLES CODE */ (0)){//启动不带UI
        auto ret = QProcess::startDetached(appname);
        qDebug()<<"QProcess::startDetached "<<appname<<ret;
    }else//启动带UI
    {
#ifdef Q_OS_WIN
        //    std::wstring command = L"notepad.exe";
        std::wstring command = appname.toStdWString();
        if (ProcessLoader::loadWindowsApplication(command) == false) {
            qWarning() <<appname<< "Failed to launch " << command.c_str()<<"请在服务中启动程序";
        }
#endif
    }
    count = 0;
    ok = true;
    _timerHeart->start();

    App::ReStartCount++;
    App::ReStartLastTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    App::writeConfig();

}

bool KeepLive::isExistProcess(QString name)
{
    bool isexist = false;
#ifdef Q_OS_WIN
    //    QTimer::singleShot(1000,this,[=]{
    isexist = ISysWin::IsExistProcess(name.toStdString().c_str());
    //    });
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


