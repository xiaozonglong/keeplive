#ifndef MYSERVICE_H
#define MYSERVICE_H

#include "qtservice.h"
#include <QDebug>
#include "keeplive.h"
#include "qtsinglecoreapplication.h"

//class MyService : public QtService<QCoreApplication>
class MyService : public QtService<QtSingleCoreApplication>
{
public:
    explicit MyService(int argc, char **argv);
    ~MyService();
protected:

    /* 开启服务 */
    virtual void start() override;

    /* 停止服务 */
    virtual void stop() override;

    /* 暂停服务 */
    virtual void pause() override;

    /* 重启服务 */
    virtual void resume() override;
private:
    KeepLive *_live = nullptr;

};

#endif // MYSERVICE_H
