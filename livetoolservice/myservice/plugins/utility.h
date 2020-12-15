#ifndef UTILITY_H
#define UTILITY_H
#include <QProcess>
#include <QObject>
#include <QTime>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QJsonObject>
#include <QJsonDocument>
#else
#include "json-forwards.h"
#include "json/json.h"
#endif

namespace utility
{
int rand(int begin,int end);
void insertM(QHash<int, QVariantHash  > &src, QHash<int, QVariantHash  > dest_store);
QString platformKey();
qlonglong xreg(QString regstr);
QString formatMACString(const QString &hex);
QByteArray formatMACString2ByteArray(const QString &hex);
int isDigitStr(QString src);
QByteArray process(QString cmd, QStringList args = QStringList(""));

}
namespace URLUtils
{
QString uDecodeSymbol(const QByteArray &resStr);
QString uDecodeSymbol(const QString &resStr);
QString uEncodeSymbol(const QByteArray &resStr);
}
namespace RegExpUtils
{
    bool existUrl(const QString &strText);
    bool isRegexMatch(QString context, QString pattern);

    QStringList stringExactMatch(QStringList keys, QRegExp regExp);
    bool stringExactMatch(QString key, QRegExp regExp);
    QVariant _find_attrValue(QVariantMap attribute, QString kkey);
    QVariant _find_attrKey(QVariantMap attribute, QString kkey);

}
namespace FileUtils
{
    QByteArray getFileHash(QString filePath,QCryptographicHash::Algorithm method = QCryptographicHash::Md5);
    void record(QString context, QString path);
    bool writeToFile(QString context, QString path);

    bool isFileExist(QString fullFilePath, bool iscreate = true);
    bool isDirExist(QString fullPath, bool iscreate = false);
    QString GBK2UTF8(const QString &str);

    QString UTF82GBK(const QString &str, QString encoding = "GB18030");

    std::string GBK2UTF8(std::string &str);


    std::string UTF82GBK(std::string &str);
    void openCatalog(QString path);
    void copyDefaultFile(QString defaultpath,QString destpath);
}

namespace IpUtils {
    bool checkIp(QString ip);
    bool checkip(QString ip);
    bool checkMask(QString mask);
    bool checkmask(QString mask);
    bool isSameNetwork(QString ip, QString mask, QString gateway);
}
//针对是MC公司的板子
namespace GPIOUtils {
    QString wpa_cli_scan(QString card);
    QString wpa_cli_scan_result(QString card);
    QString wpa_passphrase(QString apppath, QString account, QString passwd);
    bool isExist(QString iofile);
    void userLedTrun(QString gpiostr,bool sw);
    void watchdogTrun(int time = 30);
}

namespace ModbusUtils {
QString translate_mac_port_str(QString mac, uint port);
QString translate_sn_str(QString macport, uint addr);
QString translate_product_addr_str(QString product, uint addr);
uint translate2addr(QString slave_addr_str);
QString translate2product(QString slave_addr_str);
#ifdef MACRO_APP_LibModbus
QByteArray modbusRTUValidWriteByte(QByteArray &data, QByteArray &modbus_rtu_head);
QByteArray non_modbusRTUValidWriteByte(QByteArray &data, QByteArray &modbus_rtu_head);
QByteArray modbusRTUValidReadByte(QByteArray &data, QByteArray &modbus_rtu_head);
#endif

}
namespace ProtocolUtils {
QList<QVariant> getArrayDataFromBase(QString key, QVariantMap basekv);
bool getKVDataFromBase(QString key, QVariantMap basekv, QVariantMap &objkv, QVariantMap &otherkv);
bool getKVDataFromFile(QString path, QVariantMap &basekv);
}
namespace JsonUtils
{
bool getVariantFromJsonFile(QString filepath, QVariantMap &kvs);
bool getVariantFromJsonByteArray(QByteArray ba, QVariantMap &kvs);
QJsonObject convertQStringToQJsonObject(const QString jsonString);
QString convertQJsonObjectToQString(const QJsonObject& jsonObject);
QString convertQJsonDocumentToQString(const QJsonDocument& document);
QByteArray convertVariantToQString(QVariant var, bool sw = false);
}
#include <QThread>
#include <QTimer>
namespace TimerUtils
{
    QTimer* initThreadTimer(QObject *obj, int interval);
}
#include <QElapsedTimer>
namespace DebugUtils
{
    QString showElapsedTime(QString title, QElapsedTimer &mstimer, bool isdebug = false);
}

#endif // UTILITY_H
