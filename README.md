# keeplive
QT Keep APP Alive for windows and linux

2020-11-26 配置文件新增UIEnable功能
2020-11-28
1.配置文件新增TimerHeartInterval功能
2.修改log和ini名字
3.开启json传输协议解析

原理
使用UDP通信方式，发送json格式心跳数据包，询问应用状态。

基础特性
1.当被守护应用死机，或者意外退出。守护进程杀死并重启被守护应用
高级特性
1.被守护应用，可以远程配置守护进程，快速配置。

windows 系统服务方式启动应用 ---- linux 不支持，只会在/ect/xdg/QtSoftware.conf生产配置信息



未来完善方向，多应用同时守护。
