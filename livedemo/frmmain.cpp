#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "frmmain.h"
#include "ui_frmmain.h"

#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QLockFile>
frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    initLock();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initLock()
{
    QString applicationName = QCoreApplication::applicationName();
    QString path = QDir::currentPath()+"/"+applicationName+(".lock");
    // path = C:/Users/yu/AppData/Local/Temp/SingleAppTest.lock
    //    QLockFile lockFile(path);
    QLockFile *plockfile = new QLockFile(path);

    connect(this,&frmMain::destroyed,this,[=](){
        delete plockfile;
    });
    // bool QLockFile::tryLock(int timeout = 0)
    // tryLock尝试创建锁定文件。此函数如果获得锁，则返回true; 否则返回false。
    // 如果另一个进程（或另一个线程）已经创建了锁文件，则此函数将最多等待timeout毫秒

    QString context;

    if (!plockfile->tryLock(100))
    {
        int time =5000;
        context = QString("The application is already running.\n"
                   "Allowed to run only one instance of the application.\n"
                   "%1 ms to exit").arg(time);
        ui->label->setText(context);
        QTimer::singleShot(time,this,[=](){
            exit(-4);
        });
    }else
    {
        context = ("The application is not locked ,now to lock.\n");
    }
    ui->label->setText(context);
}

void frmMain::on_pushButton_released()
{
    //    QTimer *timer = nullptr;
    QTimer *timer;
    //    timer = new QTimer(this);
    timer->setInterval(2000);
    timer->start();
}

void frmMain::on_pushButton_2_released()
{
    while(1);
}
