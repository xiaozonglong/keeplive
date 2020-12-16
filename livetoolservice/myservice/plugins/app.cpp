#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "app.h"
#include "qsettings.h"
#include "qfile.h"

QString App::ConfigFile ="";
QString App::TargetAppName = "";
QString App::SuffixAppName = "";
quint16 App::TargetAppPort = 0;
bool App::ReStartExplorer = false;
int App::TimeoutCount = 7;
int App::ReStartCount = 0;
int App::TimerHeartInterval = 5000;
QString App::ReStartLastTime = "";
bool App::UIEnable = true;
bool App::DestoryApp = true;
bool App::ExitDestoryApp = true;
void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);
    set.beginGroup("BaseConfig");
    App::TargetAppName = set.value("TargetAppName", App::TargetAppName).toString();
    App::SuffixAppName = set.value("SuffixAppName", App::SuffixAppName).toString();
    App::TargetAppPort = set.value("TargetAppPort", App::TargetAppPort).toInt();
    App::ReStartExplorer = set.value("ReStartExplorer", App::ReStartExplorer).toBool();
    App::TimeoutCount = set.value("TimeoutCount", App::TimeoutCount).toInt();
    App::TimerHeartInterval = set.value("TimerHeartInterval", App::TimerHeartInterval).toInt();
    App::ReStartCount = set.value("ReStartCount", App::ReStartCount).toInt();
    App::ReStartLastTime = set.value("ReStartLastTime", App::ReStartLastTime).toString();
    App::UIEnable = set.value("UIEnable", App::UIEnable).toBool();
    App::DestoryApp = set.value("DestoryApp", App::DestoryApp).toBool();
    App::ExitDestoryApp = set.value("ExitDestoryApp", App::ExitDestoryApp).toBool();
    set.endGroup();
}

void App::writeConfig()
{
    QSettings set(App::ConfigFile, QSettings::IniFormat);
    set.beginGroup("BaseConfig");
    set.setValue("TargetAppName", App::TargetAppName);
    set.setValue("SuffixAppName", App::SuffixAppName);
    set.setValue("TargetAppPort", App::TargetAppPort);
    set.setValue("ReStartExplorer", App::ReStartExplorer);
    set.setValue("TimeoutCount", App::TimeoutCount);
    set.setValue("TimerHeartInterval", App::TimerHeartInterval);
    set.setValue("ReStartCount", App::ReStartCount);
    set.setValue("ReStartLastTime", App::ReStartLastTime);
    set.setValue("UIEnable", App::UIEnable);
    set.setValue("DestoryApp", App::DestoryApp);
    set.setValue("ExitDestoryApp", App::ExitDestoryApp);
    set.endGroup();
}

bool App::checkConfig()
{
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(App::ConfigFile);
    if (file.size() == 0) {
        writeConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            writeConfig();
            return false;
        }
    } else {
        writeConfig();
        return false;
    }

    return true;
}
