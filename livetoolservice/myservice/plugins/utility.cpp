#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "utility.h"
#include <QDir>
#include <QUrl>

QString utility::platformKey()
{
    QString m_platform;
#if defined Q_OS_WIN
    m_platform = "windows";
#elif defined Q_OS_MAC
    m_platform = "osx";
#elif defined Q_OS_LINUX
#ifdef MACRO_linux_mipsle
    m_platform = "linux_mipsle";
#elif defined MACRO_linux_arm
    m_platform = "linux_arm";
#else
    m_platform = "linux";
#endif
#elif defined Q_OS_ANDROID
    m_platform = "android";
#elif defined Q_OS_IOS
    m_platform = "ios";
#endif
    return m_platform;
}
//[0,2) 取值0，1
int utility::rand(int begin, int end)
{
    if(begin == end)return 0 ;
    if(begin>end)
    {
        int tmp = begin;
        begin = end;
        end = tmp;
    }
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
    int value = qrand()%(end-begin);
    return value+begin;
}
//"0x00"-->0
qlonglong utility::xreg(QString regstr)
{
    qlonglong ret = -1;
    //                            startadd = startaddstr.toInt();
    QRegExp rxsub("0x([0-9A-Fa-f]+)");

    if(rxsub.isValid())
    {
        if(rxsub.indexIn(regstr) != -1)
        {
            ret=rxsub.cap(1).toLongLong(nullptr,16);
        }else
        {
            ret = regstr.toLongLong();
        }
    }else
    {
        qDebug()<<"ssssss"<<regstr;
    }

    return ret;

}
void utility::insertM(QHash<int, QVariantHash > &src,QHash<int, QVariantHash > dest_store)
{
    foreach (int pos, dest_store.keys()) {
        QVariantHash  prevalue,aftervalue;
        prevalue = src.value(pos);
        aftervalue = dest_store.value(pos);
        foreach (QString key, aftervalue.keys()) {
            QVariant value;
            value = aftervalue.value(key);
            if(!value.isValid() || value == "")
            {

            }else
            {
                prevalue.insert(key,value);
            }
        }
        src.insert(pos,prevalue);
    }
}
//url正则解析，判断是否含有url
bool RegExpUtils::existUrl(const QString &strText)
{
    bool bResult = false;
    QString strTempText = strText;
    //可以识别www.baidu.com、http://abc123.com等(遇到中文、空格、特殊字符则结束)
    QString strUrlExp = "((http|https|ftp)://|(www)\\.)(\\w+)(\\.?[\\.a-z0-9/:?%&=\\-_+#;]*)"; //url正则
    QRegExp urlRegExp(strUrlExp,Qt::CaseInsensitive); //Url正则表达式，不区分大小写
    while(urlRegExp.indexIn(strTempText) != -1)
    {
        bResult = true;
        QString strWebUrl = urlRegExp.cap(0);//匹配到的url

        qDebug() << strWebUrl; //输出url
        int nIndex = strTempText.indexOf(strWebUrl); //索引位置
        strTempText.remove(0,nIndex+strWebUrl.size()); //删除已遍历过的内容
    }
    return bResult; //返回是否包含url
}


bool RegExpUtils::isRegexMatch(QString context, QString pattern)
{
    QRegExp rx(pattern);
    bool match = rx.exactMatch(context);
    return match;
}
QStringList RegExpUtils::stringExactMatch(QStringList keys, QRegExp regExp)
{
    QStringList list;
    foreach(auto key,keys)
    {
        bool ok = regExp.exactMatch(key);
        if(ok){
            list<<key;
        }
    }
    return list ;
}

bool RegExpUtils::stringExactMatch(QString key, QRegExp regExp)
{
    bool ok = regExp.exactMatch(key);
    return ok;
}
QVariant RegExpUtils::_find_attrValue(QVariantMap attribute,QString kkey)
{
    QVariant c_attrkvs;
    QVariantMap product_attribute_wildcard;
    foreach (auto key, attribute.keys()) {
        if(key.contains("*"))
        {
            product_attribute_wildcard.insert(key,attribute.value(key));
        }
    }
    //通配符识别
    foreach (auto key, product_attribute_wildcard.keys()) {
        QString keytmp = key;
        QRegExp regExp(keytmp.replace("*","(.*)"));

        if(regExp.indexIn(kkey) != -1)
        {
            //                sn = "000000000000";
            auto id = regExp.cap(1);
            //            auto beforestr = regExp.cap(2);

            auto v = product_attribute_wildcard.value(key);
            if(v.type() == QVariant::String)
            {
                c_attrkvs = v.toString().replace("*",id);
            }else
            {
                c_attrkvs =  v;
            }
            break;

        }else
        {
            //            qDebug()<<regExp.errorString();
        }

        //        bool ok = regExp.exactMatch(kkey);

        //        if(ok)
        //        {
        //            auto v = product_attribute_wildcard.value(key);
        //            if(v.type() == QVariant::String)
        //            {
        //                c_attrkvs = v;
        //            }else
        //            {
        //                c_attrkvs =  v;
        //            }
        //            break;
        //        }
    }
    return c_attrkvs;
}
QVariant RegExpUtils::_find_attrKey(QVariantMap attribute,QString kkey)
{
    QVariant c_attrkvs;
    QVariantMap product_attribute_wildcard;
    foreach (auto keyvariant, attribute.values()) {
        QString key =keyvariant.toString();
        if(key.contains("*"))
        {
            product_attribute_wildcard.insert(key,attribute.key(key));
        }
    }
    //通配符识别
    foreach (auto key, product_attribute_wildcard.keys()) {
        QString keytmp = key;
        QRegExp regExp(keytmp.replace("*","(.*)"));

        if(regExp.indexIn(kkey) != -1)
        {
            //                sn = "000000000000";
            auto id = regExp.cap(1);
            //            auto beforestr = regExp.cap(2);

            auto v = product_attribute_wildcard.value(key);
            if(v.type() == QVariant::String)
            {
                c_attrkvs = v.toString().replace("*",id);
            }else
            {
                c_attrkvs =  v;
            }
            break;

        }else
        {
            //            qDebug()<<regExp.errorString();
        }
    }
    return c_attrkvs;
}
void FileUtils::openCatalog(QString path)
{
#ifdef Q_OS_ANDROID
    QString application_path = QApplication::applicationFilePath();
    //    messageDlg(this,application_path);
#elif defined Q_OS_LINUX
    //    QDesktopServices::openUrl(QUrl::fromLocalFile("http://www.baidu.com"));
    QProcess::execute("nautilus",QStringList(path));
#elif defined Q_OS_WIN

#else

#endif
}
void FileUtils::copyDefaultFile(QString defaultpath,QString destpath)
{
    if(QFile::exists(defaultpath))
    {
        if(destpath != defaultpath)
        {
            if(!QFile::exists(destpath))
            {
                qDebug()<<"正在copy"<<defaultpath<<"-->"<<destpath;
                auto success = QFile::copy(defaultpath,destpath);
                if(success)qDebug()<<"已经copy配置文件"<<destpath;
            }
        }
    }
}

bool FileUtils::isFileExist(QString fullFilePath,bool iscreate)
{
    bool ret = false;
    QFile file(fullFilePath);
    ret = file.exists();
    return ret;
}

bool FileUtils::isDirExist(QString fullPath,bool iscreate)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
        return true;
    }
    else
    {
        if(iscreate == true)
        {
            bool ok = dir.mkpath(fullPath);//创建多级目录
            return ok;
        }
    }
    return false;
}
QByteArray FileUtils::getFileHash(QString filePath,QCryptographicHash::Algorithm method)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(method);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    localFile.close();
    QByteArray md5 = ch.result();
    return md5;
}
bool IpUtils::checkIp(QString ip)
{
    QRegExp rx2("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    if( !rx2.exactMatch(ip) )
    {
        return false;
    } else {
        QStringList pieces = ip.split(".");
        int b_subIpNum[4];
        int b_cnt255 = 0;
        bool ok = true;
        for (int i = 0;i < 4;i++) {
            b_subIpNum[i] = pieces.value(i).toInt(&ok,10);//合理性判断
            if(!ok) {
                return false;
            }
            if (b_subIpNum[i] > 255) {
                return false;
            }
            if (b_subIpNum[i] == 255) {
                b_cnt255 += 1;
            }
        }
        if (b_subIpNum[0] == 0 || b_cnt255 == 4){
            b_cnt255 = 0;
            return false;
        }
    }
    return true;
}
bool IpUtils::checkip(QString ip)
{
    QRegExp rx2("^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$");
    if( !rx2.exactMatch(ip) )
    {
        return false;
    }
    return true;
}
bool IpUtils::checkMask(QString mask)
{
    QRegExp rx2("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    if( !rx2.exactMatch(mask) )
    {
        return false;
    } else {
        QStringList pieces = mask.split(".");
        int b_subMaskNum[4];
        int b_cnt255 = 0;
        int b_cnt0 = 0;
        bool ok = true;
        int imask = 0;
        int tmask = 255;

        for (int i = 0;i < 4;i++) {
            b_subMaskNum[i] = pieces.value(i).toInt(&ok,10);//合理性判断
            if(!ok) {
                return false;
            }
            imask =  b_subMaskNum[i];
            if(!(imask==128 || imask==192 || imask==224 || imask==240 || imask==248 || imask==252 || imask==254 || imask==255 || imask==0)) {
                return false;
            } else if (imask > tmask) {
                return false;
            }

            if (b_subMaskNum[i] == 0) {
                b_cnt0 += 1;
            }
            if (b_subMaskNum[i] == 255) {
                b_cnt255 += 1;
            }
            tmask = imask;
        }
        if((b_cnt255 + b_cnt0) < 3) {
            return false;
        }
        if (b_subMaskNum[0] == 0 || b_cnt255 == 4){
            b_cnt255 = 0;
            return false;
        }
    }
    return true;
}
bool IpUtils::checkmask(QString mask)
{
    QRegExp rx2("^(254|252|248|240|224|192|128|0)\\.0\\.0\\.0|255\\.(254|252|248|240|224|192|128|0)\\.0\\.0|255\\.255\\.(254|252|248|240|224|192|128|0)\\.0|255\\.255\\.255\\.(254|252|248|240|224|192|128|0)$");
    if( !rx2.exactMatch(mask) )
    {
        return false;
    }
    return true;
}
bool IpUtils::isSameNetwork(QString ip, QString mask, QString gateway)
{
    QStringList ipList = ip.split('.',QString::SkipEmptyParts);
    QStringList maskList = mask.split('.',QString::SkipEmptyParts);
    QStringList gatewayList = gateway.split('.',QString::SkipEmptyParts);
    bool ok = true;
    for(int i = 0;i < 4;i++) {
        if ((ipList[i].toInt(&ok,10) & maskList[i].toInt(&ok,10)) !=
                (gatewayList[i].toInt(&ok,10) & maskList[i].toInt(&ok,10))) {
            return false;
        }
    }
    return true;
}

QString utility::formatMACString(const QString &hex)
{
    QString hexStr(hex);
    QRegExp rx("([0-9A-Fa-f]{1,2})");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(hexStr, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }
    return list.join(":");
}
//A0AFBDD80AF4-->0xa0 0xaf 0xbd 0xb8 0x0a 0xf4
QByteArray utility::formatMACString2ByteArray(const QString &hex)
{
    QByteArray ret;
    QString hexStr(hex);
    QRegExp rx("([0-9A-Fa-f]{1,2})");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(hexStr, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }

    foreach (QString var, list) {
        ret.append(var.toUInt(nullptr,16));
    }

    return ret;
}

/***
  *判断一个字符串是否为纯数字
  */
int utility::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return -1;
    }
    else
    { //纯数字
        return 0;
    }
}

QString URLUtils::uDecodeSymbol(const QByteArray &resStr)
{
    // 如何把16进制的unicode编码音标显示在窗口中？
    // 比如: \u07a0[\u045an\u02c8ta\u076at(\u0251)lm\u0249nt]
    // 解码思路如下： 先把16进制unicode编码转换为 10进制unicode编码
    QString desStr;
    for (int i = 0; i< resStr.length();)
    {
        if (resStr.at(i) == '\\' )
        {
            QChar sz=  resStr.mid(i+2, 4).toUShort (0,16);
            desStr.append(sz);
            i += 6;
        }
        else
        {
            desStr.append(resStr.at(i));
            ++i;
        }
    }
    return desStr;
}

QString URLUtils::uDecodeSymbol(const QString &resStr)
{
    //    QString filename = "\\u6211\\u662f\\u4e2d\\u6587";
    QString filename = resStr;

    do{
        filename.replace("\\\\","\\");
    } while (filename.indexOf("\\\\") != -1);

    do {
        int idx = filename.indexOf("\\u");
        QString strHex = filename.mid(idx, 6);
        strHex = strHex.replace("\\u", QString());
        int nHex = strHex.toInt(nullptr, 16);
        filename.replace(idx, 6, QChar(nHex));
    } while (filename.indexOf("\\u") != -1);
    return filename;
}

QString URLUtils::uEncodeSymbol(const QByteArray &resStr)
{
    // 如何把16进制的unicode编码音标显示在窗口中？
    // 比如: \u07a0[\u045an\u02c8ta\u076at(\u0251)lm\u0249nt]
    // 解码思路如下： 先把16进制unicode编码转换为 10进制unicode编码
    QString desStr;
    for (int i = 0; i< resStr.length();)
    {
        if (resStr.at(i) == '\\' )
        {
            desStr.append(resStr.at(i));
            desStr.append('\\');
            ++i;
        }
        else
        {
            desStr.append(resStr.at(i));
            ++i;
        }
    }
    return desStr;
}

QByteArray utility::process(QString cmd, QStringList args)
{
    QProcess process;
    process.start (cmd,args);
    process.waitForStarted ();
    process.waitForFinished ();

    QByteArray ba = process.readAllStandardOutput ();
    QString strResult = QString::fromLocal8Bit (ba);
    process.close();
    return ba;
}

QString ModbusUtils::translate_mac_port_str(QString mac, uint port)
{
    //        qDebug()<<product;
    return QString("%1-%2").arg(mac).arg(port);
}
QString ModbusUtils::translate_sn_str(QString macport, uint addr)
{
    //        qDebug()<<product;
    return QString("%1:%2").arg(macport).arg(addr);
}
QString ModbusUtils::translate_product_addr_str(QString product, uint addr)
{
    //        qDebug()<<product;
    return QString("%1-%2").arg(product).arg(addr);
}

uint ModbusUtils::translate2addr(QString slave_addr_str)
{
    QStringList salvelist = slave_addr_str.split("-");
    uint slave_addr = 0;
    QString product;
    if(salvelist.size()>=2)
    {
        product = salvelist.at(0);
        slave_addr = QString(salvelist.at(1)).toUInt();
    }else{
        qWarning()<<"translate2addr error !!"<<slave_addr_str;
    }
    return slave_addr;
}
QString ModbusUtils::translate2product(QString slave_addr_str)
{
    QStringList salvelist = slave_addr_str.split("-");
    //    int slave_addr;
    QString product;
    if(salvelist.size()>=2)
    {
        product = salvelist.at(0);
        //        slave_addr = QString(salvelist.at(1)).toUInt();
    }else{
        qWarning()<<"translate2product error !!"<<slave_addr_str;
    }
    return product;
}
#ifdef MACRO_APP_LibModbus
#include "libmodbus/src/modbus-rtu.h"
#include "libmodbus/src/modbus-tcp.h"
#include "libmodbus/src/modbus.h"
#include "libmodbus/src/modbus-version.h"
QByteArray ModbusUtils::modbusRTUValidWriteByte(QByteArray &data, QByteArray &modbus_rtu_head)
{
    QByteArray valid_byte;
    if(data.isEmpty()) return valid_byte;

    if(modbus_rtu_head.size()<2)return valid_byte;

    if(modbus_rtu_head.at(0) == 0)
    {
        qWarning()<<"modbusRTUValidWriteByte"<<modbus_rtu_head.toHex();
        return valid_byte;
    }

    char slaveaddr = modbus_rtu_head.at(0) ;
    char writefuncode = modbus_rtu_head.at(1);


    //检查帧头
    QByteArray startheader;

    startheader.append(slaveaddr);
    if(data.size() < startheader.size() )  return "";
    //    startheader.append(send_frame.readfunctioncode);
    int pos = data.indexOf(startheader);
    if(pos >=0)
    {
        //去掉帧头前没用的数据
        if(pos!=0)
        {
            data.remove(0,pos);
            //            if(isDebug)qDebug()<<"去掉了没用的帧头前数据个数"<<pos<<"个";
        }
    }else
    {
        qDebug()<<"没检查到帧头,"<<data.size()<<"个数据将要清空";
        data.clear();
    }
    //提取可能对的数据
    pos = data.indexOf(startheader);
    if(pos >= 0)
    {

        if(data.size()>1)
        {
            char fun = data.at(1);
            if(fun == writefuncode)//正确的
            {
                if(data.size()>2)
                {
                    switch (writefuncode)
                    {
                    case MODBUS_FC_WRITE_SINGLE_REGISTER:
                    {
                        int datalen = 8;
                        if((pos + datalen) > data.size())
                        {
                            if(0)qDebug()<<"缓存数据长度不够，等下次再处理.当前长度"<<data.size()<<"个";
                        }else
                        {
                            valid_byte = data.mid(pos,datalen);
                            data.remove(0,pos+datalen);
                        }

                    }break;
                    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
                    {
                        int datalen = 8;
                        if((pos + datalen) > data.size())
                        {
                            if(0)qDebug()<<"缓存数据长度不够，等下次再处理.当前长度"<<data.size()<<"个";
                        }else
                        {
                            valid_byte = data.mid(pos,datalen);
                            data.remove(0,pos+datalen);
                        }

                    }
                        break;

                    default:
                        qWarning()<<"modbusRTUValidWriteByte 正在研发编写。。。writefuncode:"<<QString::number(writefuncode);
                        break;
                    }


                }
            }else if(fun == static_cast<char>(writefuncode|0x80))      //错误的
            {
                qDebug()<<"未开发"<<QString::number(fun)<<"数据返回可能错误"<<valid_byte.toHex();
                //                if(data.size()>2)
                //                {
                //                    int datalen =  2+1+2;
                //                    //缓存数据长度不够，等下次再处理.
                //                    if(datalen > data.size())
                //                    {
                //                        if(0)qDebug()<<"缓存数据长度不够，等下次再处理.当前长度"<<data.size()<<"个";
                //                    }else
                //                    {
                //                        valid_byte = data.mid(pos,datalen);
                //                        data.remove(0,pos+datalen);
                //                    }
                //                }
            }
        }
    }
    return valid_byte;
}
QByteArray ModbusUtils::non_modbusRTUValidWriteByte(QByteArray &data, QByteArray &modbus_rtu_head)
{
    QByteArray valid_byte;
    if(data.isEmpty()) return valid_byte;

    if(modbus_rtu_head.size()<2)return valid_byte;

    if(modbus_rtu_head.at(0) == 0)
    {
        qWarning()<<"non_modbusRTUValidWriteByte"<<modbus_rtu_head.toHex();
        return valid_byte;
    }

    char slaveaddr = modbus_rtu_head.at(0) ;
    char writefuncode = modbus_rtu_head.at(1);


    //检查帧头
    QByteArray startheader;

    startheader.append(slaveaddr);
    //    if(data.size() < startheader.size() )  return "";
    //    startheader.append(send_frame.readfunctioncode);
    int pos = data.indexOf(startheader);
    if(pos >=0)
    {
        //去掉帧头前没用的数据
        if(pos!=0)
        {
            data.remove(0,pos);
            //            if(isDebug)qDebug()<<"去掉了没用的帧头前数据个数"<<pos<<"个";
        }
    }else
    {
        qDebug()<<"没检查到帧头,"<<data.size()<<"个数据将要清空";
        data.clear();
    }
    //提取可能对的数据
    pos = data.indexOf(startheader);
    if(pos >= 0)
    {
        if(data.size()>1)
        {
            char fun = data.at(1);
            if(fun == writefuncode)//正确的
            {
                valid_byte = data.mid(pos,-1);
                data.clear();

            }else if(fun == static_cast<char>(writefuncode|0x80))      //错误的
            {
                valid_byte = data.mid(pos,-1);
                data.clear();
            }else
            {
                qDebug()<<"其他"<<""<<fun<<"writefuncode"<<data.toHex();
            }
        }
    }
    return valid_byte;
}

QByteArray ModbusUtils::modbusRTUValidReadByte(QByteArray &data,QByteArray &modbus_rtu_head)
{
    QByteArray valid_byte;
    if(data.isEmpty()) return valid_byte;

    if(modbus_rtu_head.size()<2)return valid_byte;
    if(modbus_rtu_head.at(0) == 0)
    {
        qWarning()<<"modbusRTUValidReadByte"<<modbus_rtu_head.toHex();
        return valid_byte;
    }

    char slaveaddr = modbus_rtu_head.at(0) ;
    char readfuncode = modbus_rtu_head.at(1);

    //检查帧头
    QByteArray startheader;

    startheader.append(slaveaddr);
    //    if(data.size() < startheader.size() )  return "";
    //    startheader.append(send_frame.readfunctioncode);
    int pos = data.indexOf(startheader);
    if(pos >=0)
    {
        //去掉帧头前没用的数据
        if(pos!=0)
        {
            data.remove(0,pos);
            //            if(isDebug)qDebug()<<"去掉了没用的帧头前数据个数"<<pos<<"个";
        }
    }else
    {
        if(0)qDebug()<<"没检查到帧头,"<<data.size()<<"个数据将要清空";
        data.clear();
    }
    //提取可能对的数据
    pos = data.indexOf(startheader);
    if(pos >= 0)
    {

        if(data.size()>1)
        {
            char fun = data.at(1);
            if(fun == readfuncode)//正确的
            {
                if(data.size()>2)
                {
                    int payloadlen = data.at(2);
                    int datalen =  2+1+payloadlen+2;
                    //缓存数据长度不够，等下次再处理.
                    if((pos + datalen) > data.size())
                    {
                        if(0)qDebug()<<"缓存数据长度不够，等下次再处理.当前长度"<<data.size()<<"个";
                    }else
                    {
                        valid_byte = data.mid(pos,datalen);
                        data.remove(0,pos+datalen);
                    }
                }
            }else if(fun == static_cast<char>(readfuncode|0x80))      //错误的
            {
                if(data.size()>2)
                {
                    int datalen =  2+1+2;
                    //缓存数据长度不够，等下次再处理.
                    if((pos + datalen) > data.size())
                    {
                        if(0)qDebug()<<"缓存数据长度不够，等下次再处理.当前长度"<<data.size()<<"个";
                    }else
                    {
                        valid_byte = data.mid(pos,datalen);
                        data.remove(0,pos+datalen);
                    }
                }
            }
        }
    }

    //    qDebug()<<data.size()<<valid_byte.size();
    return valid_byte;
}
#endif
QList<QVariant> ProtocolUtils::getArrayDataFromBase(QString key,QVariantMap basekv)
{
    QList<QVariant> ret;
    {
        QString protocol = basekv.value ("protocol").toString();

//        if(! (protocol.contains(protocolstr)))
//        {
//            qWarning()<<"协议不支持";
//            return ret;
//        }
        //            qDebug()<<document.isObject();
        QVariant value = basekv.value (key);
        if(value.type() == QVariant::List)
        {
            ret = value.toList();
        }
    }

    return ret;
}

bool ProtocolUtils::getKVDataFromFile(QString path, QVariantMap &basekv)
{
    return JsonUtils::getVariantFromJsonFile(path,basekv);

}
bool ProtocolUtils::getKVDataFromBase(QString key, QVariantMap basekv, QVariantMap &objkv, QVariantMap &otherkv)
{
    bool ret = false;

    if(basekv.isEmpty())
    {
        qWarning()<<"basekv.isEmpty()";
        return ret;
    }

    {
        {
            QString protocol = basekv.value ("protocol").toString();

            otherkv.insert("protocol",protocol);
//            if(!protocol.contains(protocolstr))
//            {
//                qWarning()<<"不是modbus协议"<<protocol;
//                return ret;
//            }
            //            qDebug()<<document.isObject();
            {
                // 遍历 jsonarray 数组，把每一个对象转成 json 对象
                QVariant value = basekv.value(key);
                if(value.type() == QVariant::Map)
                {
                    ret = true;
                    objkv = (value.toMap());
                }

            }
        }
    }
    return ret;
}
void FileUtils::record(QString context, QString path)
{
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Append | QFile::Text))
    {
        QTextStream ts(&file);
        ts.setCodec("utf-8");
        ts << context<<endl;
        file.close();
    }else
    {
        QString errcontext = QString("%1 is not exist!").arg(path);
        qWarning()<<errcontext;
    }
}
bool FileUtils::writeToFile(QString context,QString path)
{
    bool ret = false;
    QString Codec = "UTF-8";
    QFile fileOut(path);

    ret = fileOut.open(QIODevice::WriteOnly);

    if(!ret)
    {
        QString context = QString("%1 is not exist!").arg(path);
        qWarning()<<context;
        return ret;
    }
    QTextStream streamFileOut(&fileOut);
    streamFileOut.setCodec(Codec.toLocal8Bit());
    //   streamFileOut << contextlist.join("\r\n");
    streamFileOut <<  context;
    streamFileOut.flush();

    // ZC: 查到说，下面的参数是true的话 就是保存成"UTF-8 + BOM"，false的话 就是保存成"UTF-8"
    //  但是，我尝试了一下，都是"UTF-8"... 不知为啥...
    //    streamFileOut.setGenerateByteOrderMark(true);

    fileOut.close();
    return ret;
}

QString FileUtils::GBK2UTF8(const QString &str)
{
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    return utf8->toUnicode(str.toUtf8());
}

QString FileUtils::UTF82GBK(const QString &str,QString encoding)
{
    QTextCodec *gbk = QTextCodec::codecForName(encoding.toStdString().data());
    return gbk->toUnicode(str.toLocal8Bit());

    //    QTextStream instream;
    //    QString str1 = str;
    //    instream.setString(&str1, QIODevice::ReadWrite);
    //    instream.setCodec(encoding.toStdString().data());
    //    QString context = instream.readAll();
    //    return context;

}

std::string FileUtils::GBK2UTF8(std::string &str)
{
    QString temp = QString::fromLocal8Bit(str.c_str());
    std::string ret = temp.toUtf8().data();
    return ret;
}

std::string FileUtils::UTF82GBK(std::string &str)
{
    QString temp = QString::fromUtf8(str.c_str());
    std::string ret = temp.toLocal8Bit().data();
    return ret;
}


QJsonObject JsonUtils::convertQStringToQJsonObject(const QString jsonString)
{
    QTextCodec *tc = QTextCodec::codecForName("UTF-8");//防止中文乱码
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
    if (jsonDocument.isNull()){
        //		g_pInterface->iDataPrintf("数据解析有误"); //判断是否解析出问题
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

QString JsonUtils::convertQJsonObjectToQString(const QJsonObject& jsonObject)
{
    return QJsonDocument(jsonObject).toJson(QJsonDocument::Indented);
}
QString JsonUtils::convertQJsonDocumentToQString(const QJsonDocument& document)
{
    return document.toJson(QJsonDocument::Indented);
}

QTimer *TimerUtils::initThreadTimer(QObject *obj, int interval)
{
    bool isdebug = true;
    //定时器
    auto m_pTimer_send = new QTimer();//


    //线程
    auto m_pThread = new QThread(obj);
    QObject::connect(m_pTimer_send, &QTimer::destroyed, obj,[=]()
    {
        if(isdebug)qDebug()<<"_m_pTimer_send destroyed";
        m_pThread->requestInterruption();
        m_pThread->quit();
        m_pThread->wait();
    });
    QObject::connect(m_pThread, &QThread::started, obj,[=]()
    {
        if(isdebug)qDebug()<<"m_pThread started";
    });
    QObject::connect(m_pThread, &QThread::finished, obj,[=]()
    {
        if(isdebug)qDebug()<<"m_pThread finished";
        m_pThread->deleteLater();
    });
    QObject::connect(m_pThread, &QThread::destroyed, obj,[=]()
    {
        if(isdebug)qDebug()<<"m_pThread destroyed";
        //        qDebug()<<m_pTimer_send->isActive()<<m_pTimer_send->interval();
        //        delete  m_pTimer_send;
    });

    m_pTimer_send->moveToThread(m_pThread);
    m_pTimer_send->setInterval(interval);
    m_pTimer_send->setTimerType(Qt::PreciseTimer);


    QObject::connect(m_pThread, SIGNAL(started()), m_pTimer_send, SLOT(start()));
    QObject::connect(m_pThread, SIGNAL(finished()), m_pTimer_send, SLOT(stop()));

    QObject::connect(m_pTimer_send, &QTimer::timeout,obj, [=]()
    {

        //       auto private_interval = m_pTimer_send->property("private_interval");
        //       m_pTimer_send->setProperty("interval",private_interval);
        //        if(isdebug)qDebug()<<"hello do it"<<m_pTimer_send->interval();

        //在析构函数中终止进程
        //        if(m_pTimer_send->isSingleShot())
        //        {

        //        }
    });

    m_pThread->start();
    return m_pTimer_send;
}

bool JsonUtils::getVariantFromJsonFile(QString filepath,QVariantMap &kvs)
{

//    QByteArray filecontext = FileSys::readFile(filepath);

//    return getVariantFromJsonByteArray(filecontext,kvs);
    return true;

}

bool JsonUtils::getVariantFromJsonByteArray(QByteArray ba, QVariantMap &kvs)
{
    bool ret = false;
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(ba, &error);
    if (error.error == QJsonParseError::NoError) {
        ret = jsonDocument.isObject();
        if(ret)
        {
            QJsonObject object = jsonDocument.object();

            kvs = object.toVariantMap();
        }else
        {
            qWarning()<<"！jsonDocument.isObject()";
        }

    }
    else
    {
        if(0)qWarning()<<"Json格式有错"<<error.errorString();
    }

    return ret;
}

QByteArray JsonUtils::convertVariantToQString(QVariant var,bool sw)
{
    QJsonDocument doc=QJsonDocument::fromVariant(var);
    QJsonDocument::JsonFormat compac ;
    if(sw == true)
    {
        compac = QJsonDocument::JsonFormat::Indented ;
    }else
    {
        compac = QJsonDocument::JsonFormat::Compact ;
    }

    return (doc.toJson(compac));
}
QString DebugUtils::showElapsedTime(QString title,QElapsedTimer &mstimer,bool isdebug)
{
    float timefloat=(double)mstimer.nsecsElapsed() / (double)1000000;
    QString context = QString("%1 此函数花了%3ms时间")
            .arg(title)
            .arg(timefloat);
    if(isdebug == true)
    {
        qDebug()<<context;
    }
    return context;
}
