#include <QCoreApplication>
#include "main.h"
#include "myservice/myservice.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

    setCodeFormat ();

#ifdef MACRO_NLoger
    QString name = QCoreApplication::applicationName();
    name = name.append("_run");
    QString path = PREPATH+"log";
    FileSys::DeleteFileOrFolder(QString("%1/%2.%3").arg(path).arg(name).arg("log"));
    LogManager logInstance;
    logInstance.InitLog(QString("%1").arg(path),name,true);
    //    qDebug()<<logInstance.getLogFilePath();
#else
    //改变默认的消息处理输出
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qSetMessagePattern("%{time h:mm:ss.zzz }%{function}(%{line}):\t%{message}");
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qInstallMessageHandler (customMessageHandler);
#else
    qInstallMsgHandler(customMessageHandler);
#endif

#endif
//    Live b;
    MyService a( argc, argv );
    return a.exec();
}
