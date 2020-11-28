#ifndef MAIN_H
#define MAIN_H
//#include "BSP/application.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <QDateTime>
#include <QFile>
#include <QTextCodec>
#include <qlogging.h>
#ifdef MACRO_STATIC
#include <QtPlugin>
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
//Q_IMPORT_PLUGIN(qico)
//Q_IMPORT_PLUGIN(qcncodecs)
//Q_IMPORT_PLUGIN(qjpeg)
//Q_IMPORT_PLUGIN(qgif)
//Q_IMPORT_PLUGIN(qsql)
#else
//Q_IMPORT_PLUGIN(qvnc)

#endif
#endif

using namespace std;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
void customMessageHandler(  QtMsgType type,  const char *msg)
#endif

{
    QString txt;
    switch (type)
    {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }


    //输出到控制台
    QDateTime now = QDateTime::currentDateTime();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    cout << now.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString() << " "<< codec->fromUnicode(txt).data()<<endl;

    QFile outFile(QCoreApplication::applicationName()+"_debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append|QIODevice::Text);
    QTextStream ts(&outFile);
    ts <<now.toString("yyyy-MM-dd hh:mm:ss.zzz") << " " << txt << endl;
    outFile.close();
}
bool setCodeVersion(int argc, char *argv[])
{
    QString datastr = __DATE__;
    QDate buildDate = QLocale( QLocale::English ).toDate( datastr.replace( "  ", " 0" ) , "MMM dd yyyy");
    QTime buildTime = QTime::fromString(__TIME__,"hh:mm:ss");
    QDateTime buildDateTime;
    buildDateTime.setDate(buildDate);
    buildDateTime.setTime(buildTime);
    QString ver ;
    ver= QString("%1.%2.%3").arg("1").arg("0").arg(buildDateTime.date().toString("yyyyMMdd"));
    QCoreApplication::setApplicationVersion(ver);

    QStringList qarglist;

    for   (int i   =   1;   i<argc;   i++)
    {
        qarglist<<argv[i];
    }
    //    qDebug ()<<qarglist;

    if(qarglist.contains ("-v"))
    {
        qDebug ()<<QCoreApplication::applicationVersion ();

        return 1;
    }
    return 0;
}
void setCodeFormat()
{
    /*编码格式设置*/
    QTextCodec *codec = QTextCodec::codecForLocale();
    qDebug()<<codec->name();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if(QSysInfo::kernelType() == "winnt")
    {
#ifdef MACRO_DEBUG
//        codec = QTextCodec::codecForName("GBK");
        codec = QTextCodec::codecForName("UTF-8");
#else
        codec = QTextCodec::codecForName("UTF-8");
#endif
    }else if(QSysInfo::kernelType() == "linux")
    {
        codec = QTextCodec::codecForName("UTF-8");
    }
    else
    {
        codec = QTextCodec::codecForName("UTF-8");
    }
#else
    codec = QTextCodec::codecForName("UTF-8");
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    QTextCodec::setCodecForLocale( codec );      /*    qDebug可输出中文    */

#else
    QTextCodec::setCodecForTr( codec);              /*    输入:  tr( "字符串" )    */
    QTextCodec::setCodecForLocale( codec );      /*    qDebug可输出中文    */
    QTextCodec::setCodecForCStrings( codec );   /*    输入:  "字符串"           */
#endif
}

QStringList readConditionParameter(int argc, char *argv[])
{
    QStringList code;

    for(int i=1; i<argc; i++)
    {
        QString str;
        str = QString("%1").arg(argv[i]);
        code<<str;
    }


    return code;

}
QString loadFontFamilyFromFiles(const QString &fontFile);
#endif // MAIN_H
