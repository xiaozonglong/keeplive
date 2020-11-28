#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "myservice.h"

/*
 *window 请使用管理员权限，在命令行里输入
 * 安装服务./app.exe -i
 * 卸载安装./app.exe -u
 * 直接运行./app.exe -e
 * 可执行程序路径下，生成日志文件
*/

MyService::MyService(int argc, char **argv)
    : QtService<QtSingleCoreApplication>(argc, argv, "KeepLiveService")
{
    setServiceDescription(QString("龙创公司App %1保证应用长期运行").arg(serviceName()));
    setStartupType(QtServiceController::StartupType::AutoStartup);
    setServiceFlags(QtServiceBase::CanBeSuspended);


}

MyService::~MyService()
{
//    qDebug()<<"~KeepLiveService()";
}

void MyService::start()
{
    auto app = application();
    Q_UNUSED(app)

    if(_live == nullptr)
    {
        _live = new KeepLive(nullptr);
        QObject::connect(_live,&KeepLive::serviceCmd,[=](QString cmd,bool enable){
            if(cmd == "start")
            {
                if(enable)
                {
                    start();
                }else
                {
                    stop();
                }
            }
            else if(cmd == "pause")
            {
                if(enable)
                {
                    pause();
                }else
                {
                    resume();
                }
            }
        });


    }
    qDebug()<<"start()";
}

void MyService::stop()
{

    if(_live != nullptr)
    {
        delete  _live;
        _live = nullptr;
    }
    qDebug()<<"stop()";
}

void MyService::pause()
{
    qDebug()<<"pause()";
    if(_live != nullptr)
    {
        _live->pause();
    }
}

void MyService::resume()
{
    qDebug()<<"resume()";
    if(_live != nullptr)
    {
        _live->resume();
    }
}
