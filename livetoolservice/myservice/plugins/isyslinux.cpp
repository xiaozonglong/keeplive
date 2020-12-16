#include "isyslinux.h"
#include "utility.h"
ISysLinux::ISysLinux()
{

}

bool ISysLinux::IsExistProcess(const char*  szProcessName)
{
    bool ret = false;

    QStringList args;

        args<<QString("-auxhr");
//        args<<QString("| grep %1").arg(szProcessName);

//        args<<QString(szProcessName);


        QString context ;


        context =    utility::process("ps",args);



        if(context.contains(szProcessName))
        {
            ret = true;
        }

    return ret;
}

bool ISysLinux::killAll(QString szProcessName)
{
    bool ret = false;

    QStringList args;


        args<<QString(szProcessName);


        QString context ;


        context =    utility::process("killall",args);



        if(!context.contains(szProcessName))
        {
            ret = true;
        }

    return ret;
}
