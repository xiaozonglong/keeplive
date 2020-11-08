#ifndef __ISYSINFO_H
#define __ISYSINFO_H
//#pragma once
//#pragma comment(lib,"Kernel32.lib")
//#pragma comment(lib,"Psapi.lib")
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <setupapi.h>
    #include <dbt.h>
#endif /*Q_OS_WIN*/

#include <TlHelp32.h>
#include <direct.h>
#include <winternl.h>
#include <Psapi.h>
#include <QMap>
//获取系统信息的接口
class ISysInfo
{
public:
    virtual ~ISysInfo(){}
    /************************************************************************/
    /*函数说明：获取系统CPU当前使用率                                           */
    /*函数参数：nCpuRate->CPU使用率                                           */
    /*返回结果：返回结果状态                                                   */
    /*作者时间：Bevis 2014-07-19                                             */
    /************************************************************************/
    virtual bool GetSysCpu(int& nCpuRate) = 0;
    /************************************************************************/
    /*函数说明：获取系统内存信息                                                 */
    /*函数参数：nMemTotal->内存总共大小,(MB)；nMemUsed->内存已使用大小,(MB)        */
    /*返回结果：返回结果状态                                                   */
    /*作者时间：Bevis 2014-07-19                                           */
    /************************************************************************/
    virtual bool GetSysMemory(int& nMemTotal,int& nMemUsed) = 0;
    /************************************************************************/
    /*函数说明：获取系统磁盘信息                                             */
    /*函数参数：nDiskTotal->磁盘总共大小,(MB)；nDiskUsed->磁盘已使用大小,(MB) */
    /*返回结果：返回结果状态                                               */
    /*作者时间：Bevis 2014-07-19                                             */
    /************************************************************************/
    virtual bool GetSysDisk(int& nDiskTotal,int& nDiskUsed) = 0;
    /************************************************************************/
    /*函数说明：遍历系统所有进程                                               */
    /*函数参数：无                                                           */
    /*返回结果：进程PID和进程名称的映射集合                                     */
    /*作者时间：Bevis 2014-07-19                                             */
    /************************************************************************/
    virtual QMap<int,QString> GetAllProcess() = 0;
    /************************************************************************/
    /*函数说明：获取单个进程的内存使用率                                         */
    /*函数参数：nPid->进程唯一标识号；nProcessMemRate->进程的内存使用率           */
    /*返回结果：返回结果状态                                                   */
    /*作者时间：Bevis 2014-07-19                                             */
    /************************************************************************/
    virtual bool GetProcessMemory(int nPid,int& nProcessMemRate ,int& nProcessMemUsed) = 0;
};
#endif
