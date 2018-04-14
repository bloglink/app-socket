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
    initSettings();
}

AppSocket::~AppSocket()
{
}

void AppSocket::initUI()
{
    initSkin();
    initTitle();
    initLayout();
    initRecvBar();
    initSendBar();
}

void AppSocket::initSkin()
{
    QFile file;
    QString qss;
    file.setFileName(":/qss_black.css");
    file.open(QFile::ReadOnly);
    qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

void AppSocket::initTitle()
{
    char s_month[5];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    int month, day, year;

    sscanf((__DATE__), "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month)-month_names)/3+1;

    QDate dt;
    dt.setDate(year, month, day);
    static const QTime tt = QTime::fromString(__TIME__, "hh:mm:ss");

    QDateTime t(dt, tt);
    QString verNumb = QString("V-0.1.%1").arg(t.toString("yyMMdd-hhmm"));
    this->setWindowTitle(QString("网络调试助手%1").arg(verNumb));
}

void AppSocket::initLayout()
{
    layout = new QGridLayout;
    QFrame *frame = new QFrame(this);
    frame->setLayout(layout);

    this->setCentralWidget(frame);
    this->resize(800, 600);
}

void AppSocket::initRecvBar()
{
    textUdpRecv = new QTextEdit(this);
    layout->addWidget(textUdpRecv, 0, 0, 8, 4);

    boxHostAddr = new QComboBox(this);
    boxHostAddr->setEditable(true);
    boxHostAddr->setFixedHeight(44);
    boxHostAddr->setView(new QListView);
    layout->addWidget(new QLabel(tr("设备地址"), this), 3, 4);
    layout->addWidget(boxHostAddr, 3, 5);

    boxHostPort = new QComboBox(this);
    boxHostPort->setEditable(true);
    boxHostPort->setFixedHeight(44);
    boxHostPort->setView(new QListView);
    layout->addWidget(new QLabel(tr("设备端口"), this), 4, 4);
    layout->addWidget(boxHostPort, 4, 5);

    boxAutoSpace = new QCheckBox(tr("空格"), this);
    layout->addWidget(boxAutoSpace, 5, 4);
    boxGetToHEX = new QCheckBox(tr("十六进制接收"), this);
    layout->addWidget(boxGetToHEX, 5, 5);

    QPushButton *btnConnect = new QPushButton(tr("打开连接"), this);
    connect(btnConnect, SIGNAL(clicked(bool)), this, SLOT(openSocketDev()));
    btnConnect->setFixedHeight(44);
    layout->addWidget(btnConnect, 6, 4, 1, 2);

    QPushButton *btnClear = new QPushButton(tr("清空显示"), this);
    connect(btnClear, SIGNAL(clicked(bool)), textUdpRecv, SLOT(clear()));
    btnClear->setFixedHeight(44);
    layout->addWidget(btnClear, 7, 4, 1, 2);
}

void AppSocket::initSendBar()
{
    textPutTime = new QLineEdit("1", this);
    textPutTime->setFixedHeight(44);
    layout->addWidget(new QLabel(tr("发送次数:"), this), 8, 0);
    layout->addWidget(textPutTime, 8, 1);
    textPutRate = new QLineEdit("1", this);
    textPutRate->setFixedHeight(44);
    layout->addWidget(new QLabel(tr("发送间隔(ms):"), this), 8, 2);
    layout->addWidget(textPutRate, 8, 3);

    boxLoopSend = new QCheckBox(tr("循环"), this);
    layout->addWidget(boxLoopSend, 8, 4);
    boxPutToHEX = new QCheckBox(tr("十六进制发送"), this);
    layout->addWidget(boxPutToHEX, 8, 5);

    textUdpSend = new QLineEdit(this);
    textUdpSend->setFixedHeight(44);
    layout->addWidget(new QLabel(tr("发送数据:"), this), 9, 0);
    layout->addWidget(textUdpSend, 9, 1, 1, 3);

    QPushButton *btnSendCurr = new QPushButton(tr("发送当前"), this);
    connect(btnSendCurr, SIGNAL(clicked(bool)), this, SLOT(sendCurrentText()));
    btnSendCurr->setFixedHeight(44);
    layout->addWidget(btnSendCurr, 9, 4, 1, 2);

    QStringList headers;
    headers << tr("发送数据列表") << tr("备注");
    tabList = new QTableWidget(this);
    tabList->setColumnCount(2);
    tabList->setHorizontalHeaderLabels(headers);
    tabList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tabList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    layout->addWidget(tabList, 10, 0, 4, 4);
    connect(tabList, SIGNAL(cellClicked(int, int)), this, SLOT(updateCurrentItem(int, int)));

    QPushButton *btnSendList = new QPushButton(tr("发送列表"), this);
    connect(btnSendList, SIGNAL(clicked(bool)), this, SLOT(sendCurrentList()));
    btnSendList->setFixedHeight(44);
    layout->addWidget(btnSendList, 10, 4, 1, 2);

    QPushButton *btnStopSend = new QPushButton(tr("停止发送"), this);
    connect(btnStopSend, SIGNAL(clicked(bool)), this, SLOT(stopCurrentLoop()));
    btnStopSend->setFixedHeight(44);
    layout->addWidget(btnStopSend, 11, 4, 1, 2);

    QPushButton *btnAppend = new QPushButton(tr("添加任务"), this);
    connect(btnAppend, SIGNAL(clicked(bool)), this, SLOT(appendCurrentItem()));
    btnAppend->setFixedHeight(44);
    layout->addWidget(btnAppend, 12, 4, 1, 2);

    QPushButton *btnDelete = new QPushButton(tr("删除任务"), this);
    connect(btnDelete, SIGNAL(clicked(bool)), this, SLOT(deleteCurrentItem()));
    btnDelete->setFixedHeight(44);
    layout->addWidget(btnDelete, 13, 4, 1, 2);

    QString tmp = tr("网络调试助手 by link");
    layout->addWidget(new QLabel(tmp, this), 14, 0, 1, 6);
}

void AppSocket::initSettings()
{
    QSettings *set = new QSettings("./nandflash/config.ini", QSettings::IniFormat);
    set->setIniCodec("GB18030");
    set->beginGroup("CSocket");
    QString TableItem = "7B 06 00 00 06 7D";
    boxHostAddr->addItem(set->value("HostAddr", "192.168.1.57").toString());
    boxHostPort->addItem(set->value("HostPort", "8234").toString());
    boxAutoSpace->setChecked(set->value("AutoSpace", "false").toBool());
    boxLoopSend->setChecked(set->value("LoopSend", "false").toBool());
    boxGetToHEX->setChecked(set->value("GetToHEX", "false").toBool());
    boxPutToHEX->setChecked(set->value("PutToHEX", "false").toBool());
    textPutRate->setText(set->value("SendRate", "100").toString());
    textPutTime->setText(set->value("PutTime", "1").toString());
    textUdpSend->setText(set->value("PutText", TableItem).toString());

    tabTexts = QString(set->value("TabTexts", TableItem).toString()).split("@@");
    tabNotes = QString(set->value("TabNotes", "").toString()).split("@@");
    tabList->setRowCount(qMin(tabTexts.size(), tabNotes.size()));
    for (int i=0; i < tabTexts.size(); i++) {
        tabList->setItem(i, 0, new QTableWidgetItem);
        tabList->setItem(i, 1, new QTableWidgetItem);
        tabList->item(i, 0)->setText(tabTexts.at(i));
        tabList->item(i, 1)->setText(tabNotes.at(i));
    }
    set->deleteLater();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendSocketDev()));
    stopCurrentLoop();
    udp = NULL;
}

void AppSocket::saveSettings()
{
    QSettings *set = new QSettings("./nandflash/config.ini", QSettings::IniFormat);
    set->setIniCodec("GB18030");
    set->beginGroup("CSocket");
    set->setValue("HostAddr", boxHostAddr->currentText());
    set->setValue("HostPort", boxHostPort->currentText());
    set->setValue("AutoSpace", boxAutoSpace->isChecked());
    set->setValue("GetToHEX", boxGetToHEX->isChecked());
    set->setValue("LoopSend", boxLoopSend->isChecked());
    set->setValue("PutToHEX", boxPutToHEX->isChecked());
    set->setValue("SendRate", textPutRate->text());
    set->setValue("PutTime", textPutTime->text());
    set->setValue("PutText", textUdpSend->text());
    for (int i=0; i < tabTexts.size(); i++) {
        tabTexts[i] = tabList->item(i, 0)->text();
    }
    set->setValue("TabTexts", tabTexts.join("@@"));
    for (int i=0; i < tabNotes.size(); i++) {
        tabNotes[i] = tabList->item(i, 1)->text();
    }
    set->setValue("TabNotes", tabNotes.join("@@"));
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
    int port = boxHostPort->currentText().toInt();
    if (udp->bind(QHostAddress::AnyIPv4, port)) {
        btn->setText("关闭连接");
        timer->start(1);
    } else {
        textUdpRecv->insertPlainText(tr("连接失败\n").toUtf8());
        textUdpRecv->moveCursor(QTextCursor::End);
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

void AppSocket::sendSocketDev()
{
    if (sendOut == 1) {
        timeOut++;
        int t = timeOut / textPutRate->text().toInt();
        if (t >= textPutTime->text().toInt() && !boxLoopSend->isChecked()) {
            stopCurrentLoop();
        } else if (timeOut % textPutRate->text().toInt() == 0) {
            sendCurrentByte(textUdpSend->text().toUtf8());
        }
    }
    if (sendOut == 2) {
        timeOut++;
        int t = timeOut / textPutRate->text().toInt();
        int tt = t % tabList->rowCount();
        int ttt = t / tabList->rowCount();
        if (ttt >= textPutTime->text().toInt() && !boxLoopSend->isChecked()) {
            stopCurrentLoop();
        } else if (timeOut % textPutRate->text().toInt() == 0) {
            sendCurrentByte(tabList->item(tt, 0)->text().toUtf8());
        }
    }
}

void AppSocket::sendCurrentText()
{
    QString temp = textUdpSend->text();
    if (temp.isEmpty())
        return;
    sendOut = 1;
    sendCurrentByte(temp.toUtf8());
}

void AppSocket::sendCurrentList()
{
    QString temp = tabList->item(0, 0)->text();
    if (temp.isEmpty())
        return;
    sendOut = 2;
    sendCurrentByte(temp.toUtf8());
}

void AppSocket::stopCurrentLoop()
{
    sendOut = 0;
    timeOut = 0;
}

void AppSocket::sendCurrentByte(QByteArray msg)
{
    if (udp == NULL || udp->state() != QAbstractSocket::BoundState) {
        textUdpRecv->insertPlainText(tr("请打开连接\n").toUtf8());
        textUdpRecv->moveCursor(QTextCursor::End);
        stopCurrentLoop();
        return;
    }
    if (msg.isEmpty())
        return;
    if (boxPutToHEX->isChecked()) {
        msg.replace(" ", "");
        msg = QByteArray::fromHex(msg);
    }
    recvAddr = boxHostAddr->currentText();
    recvPort = boxHostPort->currentText().toInt();
    udp->writeDatagram(msg, recvAddr, recvPort);
    udp->waitForBytesWritten();
    qDebug() << "udp send:" << msg;
}

void AppSocket::display(QByteArray msg)
{
    QByteArray hex = msg.toHex().toUpper();
    QByteArray temp;
    if (boxGetToHEX->isChecked()) {
        if (boxAutoSpace->isChecked()) {
            for (int i=0; i < hex.size()/2; i++) {
                temp.append(hex.at(2*i));
                temp.append(hex.at(2*i+1));
                temp.append(" ");
            }
        } else {
            temp = hex;
        }
    } else {
        temp = msg;
    }
    QString tmp;
    tmp.append("[");
    tmp.append(QTime::currentTime().toString("hh:mm:ss.zzz"));
    tmp.append("] ");
    tmp.append(QString("%1:%2: ").arg(recvAddr.toString()).arg(recvPort));
    tmp.append(temp);
    tmp.append("\n");
    textUdpRecv->insertPlainText(tmp);
    textUdpRecv->moveCursor(QTextCursor::End);
}

void AppSocket::appendCurrentItem()
{
    int t = tabTexts.size();
    tabList->setRowCount(t+1);
    tabList->setItem(t, 0, new QTableWidgetItem);
    tabList->setItem(t, 1, new QTableWidgetItem);
    tabTexts.append(textUdpSend->text());
    tabNotes.append("");
    tabList->item(t, 0)->setText(tabTexts.last());
    tabList->item(t, 1)->setText(tabNotes.last());
}

void AppSocket::deleteCurrentItem()
{
    int t = tabList->currentRow();
    if (t < 0)
        return;
    int k = tabList->rowCount();
    tabList->setRowCount(k-1);
    tabTexts.removeAt(t);
    tabNotes.removeAt(t);
    for (int i=0; i < k-1; i++) {
        tabList->item(i, 0)->setText(tabTexts.at(i));
        tabList->item(i, 1)->setText(tabNotes.at(i));
    }
}

void AppSocket::updateCurrentItem(int r, int c)
{
    if (c == 0)
        textUdpSend->setText(tabList->item(r, 0)->text());
}

void AppSocket::hideEvent(QHideEvent *e)
{
    saveSettings();
    e->accept();
}
