#include "ISysWin.h"


ISysWin::ISysWin(void)
{
}


ISysWin::~ISysWin(void)
{
}

__int64 ISysWin::Filetime2Int64(const FILETIME* ftime)
{
    LARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

__int64 ISysWin::CompareFileTime(FILETIME preTime,FILETIME nowTime)
{
    return this->Filetime2Int64(&nowTime) - this->Filetime2Int64(&preTime);
}

//将单字节char*转化为宽字节wchar_t*
wchar_t* AnsiToUnicode( const char* szStr )
{
    int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
    if (nLen == 0)
    {
        return nullptr;
    }
    wchar_t* pResult = new wchar_t[nLen];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
    return pResult;
}

//将宽字节wchar_t*转化为单字节char*
inline char* UnicodeToAnsi( const wchar_t* szStr )
{
    int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );
    if (nLen == 0)
    {
        return nullptr;
    }
    char* pResult = new char[nLen];
    WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );
    return pResult;
}

bool ISysWin::GetSysCpu(int& nCpuRate)
{
    HANDLE hEvent;
    bool res;
    static FILETIME preIdleTime;
    static FILETIME preKernelTime;
    static FILETIME preUserTime;

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;

    res = GetSystemTimes(&idleTime,&kernelTime,&userTime);

    preIdleTime = idleTime;
    preKernelTime = kernelTime;
    preUserTime = userTime;

    hEvent = CreateEvent(nullptr,FALSE,FALSE,nullptr);//初始值为nonsignaled

    WaitForSingleObject(hEvent,500);//等待500毫秒

    res = GetSystemTimes(&idleTime,&kernelTime,&userTime);

    int idle = CompareFileTime(preIdleTime,idleTime);
    int kernel = CompareFileTime(preKernelTime,kernelTime);
    int user = CompareFileTime(preUserTime,userTime);

    nCpuRate =(int)ceil( 100.0*( kernel + user - idle ) / ( kernel + user ) );

    return res;
}

bool ISysWin::GetSysMemory(int& nMemTotal,int& nMemUsed)
{
    MEMORYSTATUSEX memsStat;
    memsStat.dwLength = sizeof(memsStat);
    if(!GlobalMemoryStatusEx(&memsStat))//如果获取系统内存信息不成功，就直接返回
    {
        nMemTotal = -1;
        nMemUsed  = -1;
        return false;
    }
    int nMemFree = memsStat.ullAvailPhys/( 1024.0*1024.0 );
    nMemTotal = memsStat.ullTotalPhys/( 1024.0*1024.0 );
    nMemUsed= nMemTotal- nMemFree;
    return true;
}

bool ISysWin::GetSysDisk(int& nDiskTotal,int& nDiskUsed)
{
    static char path[_MAX_PATH];//存储当前系统存在的盘符
    int curdrive = _getdrive();
    unsigned long lFreeAll = 0UL;
    unsigned long lTotalAll = 0UL;
    for(int drive = 1; drive <= 26; drive++ )//遍历所有盘符
    {
        if( !_chdrive( drive ) )
        {
            sprintf(path, "%c:\\", drive + 'A' - 1 );
            ULARGE_INTEGER caller, total, free;
            WCHAR wszClassName[_MAX_PATH];
            memset(wszClassName,0,sizeof(wszClassName));
            MultiByteToWideChar(CP_ACP,0,path,strlen(path)+1,wszClassName,
                sizeof(wszClassName)/sizeof(wszClassName[0]));
            if (GetDiskFreeSpaceEx(wszClassName, &caller, &total, &free) == 0)
            {
                qDebug()<<"GetDiskFreeSpaceEx Filed!";
                return false;
            }

            double dTepFree = free.QuadPart/( 1024.0*1024.0 );
            double dTepTotal = total.QuadPart/( 1024.0*1024.0 );
            //qDebug()<<"Get Windows Disk Information:"<<path<<"--free:"<<dTepFree<<"--total:"<<dTepTotal;
            lFreeAll += (unsigned long)ceil(dTepFree);
            lTotalAll += (unsigned long)ceil(dTepTotal);
        }
    }
    nDiskUsed = lFreeAll;
    nDiskTotal= lTotalAll;
    return true;
}

QMap<int,QString> ISysWin::GetAllProcess()
{
    HANDLE hProcessSnap;	//进程快照的句柄
    HANDLE hProcess;	//用于获取进程的句柄
    PROCESSENTRY32 pe32;//进程信息的结构体
    QMap<int,QString> pidMap;//保存所有进程的PID
    // 获取系统进程信息的快照
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        qDebug()<<"CreateToolhelp32Snapshot Failed!";// 打印错误信息
        if(nullptr !=hProcessSnap)
        {
            CloseHandle( hProcessSnap );          // 关闭进程快照信息
            hProcessSnap = nullptr;
        }
        return pidMap;
    }
    // 在使用之前设置PROCESSENTRY32结构体的初始大小值,如果不初始化dwSize, Process32First 调用会失败.
    pe32.dwSize = sizeof( PROCESSENTRY32 );
    if( !Process32First( hProcessSnap, &pe32 ) )// 开始获取第一个进程的信息，如果获取失败就返回
    {
        qDebug()<<"Process32First Failed!"; // 打印错误信息
        if(nullptr !=hProcessSnap)
        {
            CloseHandle( hProcessSnap );          // 关闭进程快照信息
            hProcessSnap = nullptr;
        }
        return pidMap;
    }
    //开始遍历所有进程
    pidMap.clear();
    do
    {
        //加入PID
        if(!pidMap.contains((int)pe32.th32ProcessID))
        {
            QString strProcessName =QString::fromLocal8Bit(UnicodeToAnsi(pe32.szExeFile));
            pidMap.insert((int)pe32.th32ProcessID,strProcessName);
        }

        //当然还可以获取到很多其他信息，例如进程名字(szExeFile[MAX_PATH])、父进程PPID(th32ParentProcessID)。。。
        /* 附上该结构体信息
        typedef struct tagPROCESSENTRY32 {
        DWORD     dwSize;
        DWORD     cntUsage;
        DWORD     th32ProcessID;
        ULONG_PTR th32DefaultHeapID;
        DWORD     th32ModuleID;
        DWORD     cntThreads;
        DWORD     th32ParentProcessID;
        LONG      pcPriClassBase;
        DWORD     dwFlags;
        TCHAR     szExeFile[MAX_PATH];
        } PROCESSENTRY32, *PPROCESSENTRY32;
        */
    }
    while( Process32Next( hProcessSnap, &pe32 ) );// 获取下一个进程的信息
    if(nullptr !=hProcessSnap)//最后关闭快照句柄
    {
        CloseHandle( hProcessSnap );
        hProcessSnap = nullptr;
    }
    return pidMap;
}

BOOL ISysWin::IsExistProcess(const char*  szProcessName)
{
    PROCESSENTRY32 processEntry32;
    HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (((int)toolHelp32Snapshot) != -1)
    {
        processEntry32.dwSize = sizeof(processEntry32);
        if (Process32First(toolHelp32Snapshot, &processEntry32))
        {
            do
            {
                int iLen = 2 * wcslen(processEntry32.szExeFile);
                char* chRtn = new char[iLen + 1];
                //转换成功返回为非负值
                wcstombs(chRtn, processEntry32.szExeFile, iLen + 1);
                if (strcmp(szProcessName, chRtn) == 0)
                {
                    delete[]  chRtn;
                    return TRUE;
                }
                 delete[]  chRtn;
            } while (Process32Next(toolHelp32Snapshot, &processEntry32));
        }
        CloseHandle(toolHelp32Snapshot);
    }
    //
    return FALSE;
}
QList<int> ISysWin::GetProcessIDs(char *FileName)
{
    QList<int> pids;
    HANDLE myhProcess;
    PROCESSENTRY32 mype;
    mype.dwSize = sizeof(PROCESSENTRY32);
    BOOL mybRet;
    //进行进程快照
    myhProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); //TH32CS_SNAPPROCESS快照所有进程
    //开始进程查找
    mybRet=Process32First(myhProcess,&mype);
    //循环比较，得出ProcessID
    while(mybRet)
    {
        QString temp=QString::fromWCharArray(mype.szExeFile);
        if(strcmp(FileName,temp.toLocal8Bit())==0)
            pids.append(mype.th32ProcessID);
        mybRet=Process32Next(myhProcess,&mype);
    }
    return pids;
}


