#include "applive.h"
#include "qmutex.h"
#include "qudpsocket.h"
#include "qstringlist.h"
#include "qapplication.h"
#include "qdatetime.h"
#include "qdebug.h"
#include <QJsonDocument>

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

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
    udpServer  = new QUdpSocket(this);

    QString name = qApp->applicationFilePath();
    QStringList list = name.split("/");
    appName = list.at(list.count() - 1).split(".").at(0);
}

void AppKeepLive::readData()
{
    QByteArray tempData;

    do {
        tempData.resize((int)udpServer->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpServer->readDatagram(tempData.data(), tempData.size(), &sender, &senderPort);
        QString data = QLatin1String(tempData);

        //没有具体分析类容，目前全部通过
        if (!data.isEmpty()) {
            QVariantMap kvs;
            kvs.insert("AppPort",senderPort);
            kvs.insert("AppName",qApp->applicationName());
            kvs.insert("applicationDirPath",qApp->applicationDirPath());
            kvs.insert("DateTime",QDateTime::currentDateTime());
            kvs.insert("ok",true);
            QJsonDocument doc=QJsonDocument::fromVariant(QVariant(kvs));
            auto jba = QJsonDocument(doc).toJson(QJsonDocument::Indented);


            udpServer->writeDatagram(jba, sender, senderPort);
        }
    } while (udpServer->hasPendingDatagrams());
}

bool AppKeepLive::start(int port)
{
    bool ok = udpServer->bind(port);
    if (ok) {
        connect(udpServer, SIGNAL(readyRead()), this, SLOT(readData()));
        qDebug() << TIMEMS << QString("UDP bind(%1)Start AppLive Ok").arg(port);
    }

    return ok;
}

void AppKeepLive::stop()
{
    udpServer->abort();
    disconnect(udpServer, SIGNAL(readyRead()), this, SLOT(readData()));
}
