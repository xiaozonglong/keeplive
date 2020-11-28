#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "appkeeplive.h"
#include "qmutex.h"
#include "qudpsocket.h"
#include "qstringlist.h"
#include "qapplication.h"
#include "qdatetime.h"
#include "qdebug.h"
#include <QJsonDocument>
#include <QNetworkDatagram>

QScopedPointer<AppKeepLive> AppKeepLive::self;
AppKeepLive *AppKeepLive::Instance()
{
    if (self.isNull()) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppKeepLive);
        }
    }

    return self.data();
}

AppKeepLive::AppKeepLive(QObject *parent) : QObject(parent)
{
    if(_udpServer == nullptr)
    {
        _udpServer  = new QUdpSocket(this);
        connect(_udpServer,&QUdpSocket::stateChanged,this,[=](QAbstractSocket::SocketState status){
            qDebug()<<"status-->"<<status;
        },Qt::UniqueConnection);
        connect(_udpServer,QOverload<QAbstractSocket::SocketError>::of(&QUdpSocket::error),this,[=](QAbstractSocket::SocketError error){
            qDebug()<<"error-->"<<error;
        },Qt::UniqueConnection);

        QString name = qApp->applicationFilePath();
        QStringList list = name.split("/");
        appName = list.at(list.count() - 1).split(".").at(0);
    }
}

quint16 AppKeepLive::getBindPort()
{
    int retbindport = 0;
    if(_udpServer != nullptr)
    {
        retbindport = _udpServer->localPort();
    }
}

void AppKeepLive::readData()
{
    while(_udpServer->hasPendingDatagrams())//拥有等待的数据报
    {

        QNetworkDatagram datagram = _udpServer->receiveDatagram();

        //        _ser->readDatagram(datagramba.data(), datagramba.size(),
        //                           &senderAddress, &senderPort);
        QByteArray datagramba = datagram.data();
        _senderAddress = datagram.senderAddress();
        _senderPort = datagram.senderPort();

        if (!datagramba.isEmpty()) {


            QJsonDocument recvdata = QJsonDocument::fromJson(datagramba);

            if(recvdata.isNull())break;



            auto recvmap = recvdata.toVariant().toMap();

            auto messagetype = recvmap.value("messagetype").toString();
            auto packetID = recvmap.value("packetID").toUInt();
            //控制命令回复
            if(messagetype == "response")
            {
                auto object = recvmap.value("object").toString();
                auto payloadmap = recvmap.value("payload").toMap();
                auto payloadObject = payloadmap.value("object").toString();
                auto payloadObjectValue = payloadmap.value("value").toBool();
                if(payloadObject == "start")
                {
                    RemoteStart = payloadObjectValue;
                }else if(payloadObject == "pause")
                {
                    RemotePause = payloadObjectValue;
                }else
                {
                    qWarning()<<"未送别的payloadObject"<<payloadObject;
                }


            }else  if(messagetype == "update")//心跳包回复
            {
                auto payloadmap = recvmap.value("payload").toMap();
                auto payloadObject = payloadmap.value("object").toString();
                auto payloadObjectValue = payloadmap.value("value").toBool();
                auto payloadTargetAppName = payloadmap.value("TargetAppName").toString();
                if(payloadObject == "keeplive")
                {
                    if(payloadTargetAppName == qApp->applicationName())
                    {
                        emit keeplive(payloadObjectValue);
                    }
                }

                emit heartbeat(datagramba);

                QVariantMap mainkvs;
                {
                    QVariantMap kvstmp;
                    {

                        kvstmp.insert("AppPort",_senderPort);
                        kvstmp.insert("AppName",qApp->applicationName());
                        kvstmp.insert("applicationDirPath",qApp->applicationDirPath());
                        kvstmp.insert("DateTime",QDateTime::currentDateTime());

                    }
                    mainkvs.insert("payload",kvstmp);
                }
                mainkvs.insert("messagetype","response");
                mainkvs.insert("packetID",packetID);
                mainkvs.insert("soft_version","1.0");




                QJsonDocument doc=QJsonDocument::fromVariant(QVariant(mainkvs));
                auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);
                _udpServer->writeDatagram(jba, _senderAddress, _senderPort);
            }else
            {
                qWarning()<<"未开发messagetype"<<messagetype;
            }




        }
    }
}

void AppKeepLive::sendRemoteCMD(QString cmdobject,bool enable)
{
    QVariantMap mainkvs;

    {
        QVariantMap kvstmp;
        kvstmp.insert("AppName",qApp->applicationName());
        kvstmp.insert("applicationDirPath",qApp->applicationDirPath());
        kvstmp.insert("DateTime",QDateTime::currentDateTime());
        kvstmp.insert("object",cmdobject);
        kvstmp.insert("value",enable);
        mainkvs.insert("payload",kvstmp);
    }
    mainkvs.insert("object","service_status");
    mainkvs.insert("packetID",QDateTime::currentDateTime().toTime_t());
    mainkvs.insert("messagetype","update");
    mainkvs.insert("command","write");
    QJsonDocument doc=QJsonDocument::fromVariant(QVariant(mainkvs));
    auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);

    _udpServer->writeDatagram(jba, _senderAddress, _senderPort);
}

void AppKeepLive::setRemotePause(bool enable)
{
    sendRemoteCMD("pause",enable);
}


bool AppKeepLive::getRemotePause()
{
    return RemotePause;
}
#ifdef MACRO_DEBUG
void AppKeepLive::setRemoteStart(bool enable)
{
    sendRemoteCMD("start",enable);
}

bool AppKeepLive::getRemoteStart()
{
    return RemoteStart;
}

void AppKeepLive::RemoteRestart()
{
    sendRemoteCMD("restart",true);
    qDebug()<<"RemoteRestart";
}
#endif
bool AppKeepLive::start(int port)
{
    bool ok = false;
    if(_udpServer != nullptr)
    {
        ok = _udpServer->bind(port);
        if (ok) {
            connect(_udpServer, SIGNAL(readyRead()), this, SLOT(readData()),Qt::UniqueConnection);
            qDebug() << "Start AppLive Ok! udp bind "<<port;
        }
    }

    return ok;
}

void AppKeepLive::stop()
{
    if(_udpServer != nullptr)
    {
        _udpServer->abort();
        disconnect(_udpServer);
    }
}
