#ifndef ISYSLINUX_H
#define ISYSLINUX_H

#include <QObject>

class ISysLinux
{
public:
    ISysLinux();
    static bool IsExistProcess(const char *szProcessName);
    static bool killAll(QString szProcessName);
};

#endif // ISYSLINUX_H
