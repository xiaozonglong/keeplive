#include <QCoreApplication>
#include "main.h"
#include "myservice/myservice.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
    MyService a( argc, argv );
    setCodeFormat ();

    //改变默认的消息处理输出
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qSetMessagePattern("%{time h:mm:ss.zzz }%{function}(%{line}):\t%{message}");
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qInstallMessageHandler (customMessageHandler);
#else
    qInstallMsgHandler(customMessageHandler);
#endif
//    Live b;

    return a.exec();
}
