/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       网络调试助手
*******************************************************************************/
#include "appsocket.h"

AppSocket::AppSocket(QWidget *parent) : QMainWindow(parent)
{
    initUI();
}

void AppSocket::initUI()
{
    initTitle();
    initLayout();
    initRecvText();
    initRecvPort();
    initSendView();
}

void AppSocket::initTitle()
{
    char s_month[5];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    int month, day, year;
    sscanf((__DATE__), "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month)-month_names)/3+1;

    static const QDate makeDate(year, month, day);
    static const QTime makeTime = QTime::fromString(__TIME__, "hh:mm:ss");
    static const QDateTime mDateTime(makeDate, makeTime);
    QString ver = QString("V-0.2.%1").arg(mDateTime.toString("yyMMdd-hhmm"));
    this->setWindowTitle(tr("网络调试助手%1 by link").arg(ver));
    qDebug() << "app show:" << ver;
}

void AppSocket::initLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;

    toplayout = new QHBoxLayout;
    layout->addLayout(toplayout);

    lowlayout = new QHBoxLayout;
    layout->addLayout(lowlayout);

    QFrame *frame = new QFrame(this);
    frame->setLayout(layout);

    this->setCentralWidget(frame);
    this->resize(800, 600);
}

void AppSocket::initRecvText()
{
    textRecv = new QTextEdit(this);
    toplayout->addWidget(textRecv);
}

void AppSocket::initRecvPort()
{
    QGridLayout *layout = new QGridLayout;

    textAddr = new QLineEdit(this);
    textAddr->setFixedSize(97, 32);
    layout->addWidget(new QLabel(tr("设备地址"), this), 0, 0);
    layout->addWidget(textAddr, 0, 1);

    textPort = new QLineEdit(this);
    textPort->setFixedSize(97, 32);
    layout->addWidget(new QLabel(tr("设备端口"), this), 1, 0);
    layout->addWidget(textPort, 1, 1);

    boxSpace = new QCheckBox(tr("空格"), this);
    layout->addWidget(boxSpace,  2, 0);
    boxHex16 = new QCheckBox(tr("十六进制"), this);
    layout->addWidget(boxHex16,  2, 1);

    boxEnter = new QCheckBox(tr("回车"), this);
    layout->addWidget(boxEnter,  3, 0);
    boxCrc16 = new QCheckBox(tr("CRC校验"), this);
    layout->addWidget(boxCrc16,  3, 1);

    QPushButton *btnConnect = new QPushButton(tr("打开连接"), this);
    connect(btnConnect, SIGNAL(clicked(bool)), this, SLOT(openSocketDev()));
    btnConnect->setFixedHeight(44);
    layout->addWidget(btnConnect, 4, 0, 1, 2);

    QPushButton *btnClear = new QPushButton(tr("清空显示"), this);
    connect(btnClear, SIGNAL(clicked(bool)), textRecv, SLOT(clear()));
    btnClear->setFixedHeight(44);
    layout->addWidget(btnClear, 5, 0, 1, 2);

    toplayout->addLayout(layout);
    toplayout->setStretch(0, 1);
}

void AppSocket::initSendView()
{
    QGridLayout *layout = new QGridLayout;
    for (int i=0; i < 8; i++) {
        QLineEdit *box = new QLineEdit(this);
        boxSender.append(box);
        box->setFixedHeight(32);
        layout->addWidget(box, i, 0);

        QPushButton *btn = new QPushButton(tr("发送"), this);
        layout->addWidget(btn, i, 1);
        btnSender.append(btn);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(sendSocketDat()));

        layout->addWidget(new QLabel(tr("自动回复"), this), i, 2);

        QLineEdit *rpy = new QLineEdit(this);
        layout->addWidget(rpy, i, 3);
        rpy->setFixedHeight(32);
        boxReturn.append(rpy);

        layout->addWidget(new QLabel(tr("回复延时"), this), i, 4);

        QLineEdit *dly = new QLineEdit(this);
        layout->addWidget(dly, i, 5);
        dly->setFixedSize(106, 32);
        boxDelays.append(dly);
    }
    lowlayout->addLayout(layout);
}

void AppSocket::initSettings()
{
    QSettings *set = new QSettings("./nandflash/config.ini", QSettings::IniFormat);
    set->setIniCodec("GB18030");
    set->beginGroup("CSocket");
    textAddr->setText(set->value("HostAddr", "192.168.1.57").toString());
    textPort->setText(set->value("HostPort", "8234").toString());

    boxSpace->setChecked(set->value("boxSpace", "false").toBool());
    boxHex16->setChecked(set->value("boxHex16", "false").toBool());
    boxCrc16->setChecked(set->value("boxCrc16", "false").toBool());
    boxEnter->setChecked(set->value("boxEnter", "false").toBool());

    for (int i=0; i < boxSender.size(); i++) {
        boxSender.at(i)->setText(set->value(tr("SENDER%1").arg(i)).toString());
        boxReturn.at(i)->setText(set->value(tr("RETURN%1").arg(i)).toString());
        boxDelays.at(i)->setText(set->value(tr("DELAYS%1").arg(i), "100").toString());
    }

    set->deleteLater();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendSocketDev()));
    udp = NULL;
}

void AppSocket::saveSettings()
{
    QSettings *set = new QSettings("./nandflash/config.ini", QSettings::IniFormat);
    set->setIniCodec("GB18030");
    set->beginGroup("CSocket");
    set->setValue("HostAddr", textAddr->text());
    set->setValue("HostPort", textPort->text());

    set->setValue("boxSpace", boxSpace->isChecked());
    set->setValue("boxHex16", boxHex16->isChecked());
    set->setValue("boxCrc16", boxCrc16->isChecked());
    set->setValue("boxEnter", boxEnter->isChecked());
    for (int i=0; i < boxSender.size(); i++) {
        set->setValue(tr("SENDER%1").arg(i), boxSender.at(i)->text());
        set->setValue(tr("RETURN%1").arg(i), boxReturn.at(i)->text());
        set->setValue(tr("DELAYS%1").arg(i), boxDelays.at(i)->text());
    }
    set->deleteLater();
}

void AppSocket::openSocketDev()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn->text() == tr("关闭连接")) {
        btn->setText(tr("打开连接"));
        udp->deleteLater();
        timer->stop();
        return;
    }
    udp = new QUdpSocket(this);
    connect(udp, SIGNAL(readyRead()), this, SLOT(recvSocketDev()));
    int port = textPort->text().toInt();
    if (udp->bind(QHostAddress::AnyIPv4, port)) {
        btn->setText("关闭连接");
        timer->start(1);
    } else {
        textRecv->insertPlainText(tr("连接失败\n").toUtf8());
        textRecv->moveCursor(QTextCursor::End);
    }
}

void AppSocket::sendSocketDat()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    int index = btnSender.indexOf(btn);
    QString str = boxSender.at(index)->text();
    sendSocketMsg(str.toUtf8());
}

void AppSocket::sendSocketDev()
{
    for (int i=0; i < boxSender.size(); i++) {
        if (isRecv.value(i) > 0) {
            QString str = boxReturn.at(i)->text();
            quint32 ttt = wait.value(i) - 1;
            wait.insert(i, ttt);
            if (ttt == 0) {
                sendSocketMsg(str.toUtf8());
            }
        }
    }
}

void AppSocket::recvSocketDev()
{
    while (udp->hasPendingDatagrams()) {
        tmpByte.resize(udp->pendingDatagramSize());
        udp->readDatagram(tmpByte.data(), tmpByte.size(), &recvAddr, &recvPort);
        qDebug() << "udp recv:" << tmpByte;
        display(tmpByte);
        tmpByte.clear();
    }
}

void AppSocket::sendSocketMsg(QByteArray msg)
{
    if (udp == NULL || udp->state() != QAbstractSocket::BoundState) {
        textRecv->insertPlainText(tr("请打开连接\n").toUtf8());
        textRecv->moveCursor(QTextCursor::End);
        return;
    }
    if (msg.isEmpty())
        return;
    if (boxHex16->isChecked()) {
        msg.replace(" ", "");
        msg = QByteArray::fromHex(msg);
    }
    if (boxCrc16->isChecked()) {
        msg = crc16(msg);
    }
    recvAddr = textAddr->text();
    recvPort = textPort->text().toInt();
    udp->writeDatagram(msg, recvAddr, recvPort);
    udp->waitForBytesWritten();
    qDebug() << "udp send:" << msg;
}

void AppSocket::display(QByteArray msg)
{
    QByteArray hex = msg.toHex().toUpper();
    QByteArray txt = msg;
    if (boxHex16->isChecked() && boxSpace->isChecked()) {
        QStringList tmp;
        for (int i=0; i < hex.size()/2; i++) {
            tmp.append(QString(hex.mid(i*2, 2)));
        }
        txt = QString(tmp.join(" ")).toUtf8();
    }
    txt = (boxHex16->isChecked()) ? txt : msg;

    QString tmp;
    tmp.append("[");
    tmp.append(QTime::currentTime().toString("hh:mm:ss.zzz"));
    tmp.append("] ");
    tmp.append(QString("%1:%2: ").arg(recvAddr.toString()).arg(recvPort));
    tmp.append(txt);
    tmp.append("\n");
    textRecv->insertPlainText(tmp);
    textRecv->moveCursor(QTextCursor::End);

    for (int i=0; i < boxSender.size(); i++) {
        QByteArray str = boxSender.at(i)->text().toUtf8();
        if (boxHex16->isChecked()) {
            str.replace(" ", "");
            str = QByteArray::fromHex(str);
        }
        if (boxCrc16->isChecked()) {
            str = crc16(str);
        }
        if (msg == str) {
            wait.insert(i, boxDelays.at(i)->text().toInt());
            isRecv.insert(i, 1);
        }
    }
}

QByteArray AppSocket::crc16(QByteArray msg)
{
    quint16 crc = 0xFFFF;
    for (int t=0; t < msg.size(); t++) {
        crc = crc ^ quint8(msg.at(t));
        for (int i=0; i < 8; i++) {
            if ((crc & 0x0001) > 0) {
                crc = crc >> 1;
                crc = crc ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    msg.append(crc/256);
    msg.append(crc%256);
    return msg;
}

void AppSocket::showEvent(QShowEvent *e)
{
    initSettings();
    e->accept();
}

void AppSocket::hideEvent(QHideEvent *e)
{
    saveSettings();
    e->accept();
}
