#ifndef ISYSWIN_H
#define ISYSWIN_H
//#pragma once
#include <QDebug>
#include <QString>
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include "isysinfo.h"
//#include <atlconv.h>
#include <string.h>
#include <math.h>

//在WINDOWS平台上实现
class ISysWin: public ISysInfo
{
public:
    ISysWin(void);
    virtual ~ISysWin(void);

    bool GetSysCpu(int& nCpuRate) ;

    bool GetSysMemory(int& nMemTotal,int& nMemUsed);

    bool GetSysDisk(int& nDiskTotal,int& nDiskUsed) ;

    QMap<int,QString> GetAllProcess() ;

    static BOOL IsExistProcess(const char*  szProcessName);
protected:
    //时间转换
    __int64 Filetime2Int64(const FILETIME* ftime);
    //两个时间相减运算
    __int64 CompareFileTime(FILETIME preTime,FILETIME nowTime);
};
#endif
