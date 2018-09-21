/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       网络调试助手
*******************************************************************************/
#ifndef APPSOCKET_H
#define APPSOCKET_H

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QLayout>
#include <QDateTime>
#include <QListView>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QSettings>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QElapsedTimer>
#include <QTableWidgetItem>
#include <QApplication>
#include <QMainWindow>

#include <QHideEvent>
#include <QUdpSocket>

class AppSocket : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppSocket(QWidget *parent = 0);
private slots:
    void initUI();
    void initTitle();
    void initLayout();
    void initRecvText();
    void initRecvPort();
    void initSendView();
    void initSettings();
    void saveSettings();
    void openSocketDev();
    void sendSocketDat();
    void sendSocketDev();
    void recvSocketDev();
    void sendSocketMsg(QByteArray msg);
    void display(QByteArray msg);
    QByteArray crc16(QByteArray msg);
    virtual void showEvent(QShowEvent *e);
    virtual void hideEvent(QHideEvent *e);
private:
    QHBoxLayout *toplayout;
    QHBoxLayout *lowlayout;
    QTextEdit *textRecv;
    QLineEdit *textAddr;
    QLineEdit *textPort;
    QCheckBox *boxSpace;
    QCheckBox *boxHex16;
    QCheckBox *boxCrc16;
    QCheckBox *boxEnter;
    QList<QLineEdit*> boxSender;
    QList<QPushButton*> btnSender;
    QList<QLineEdit*> boxReturn;
    QList<QLineEdit*> boxDelays;
    QMap<int, int> wait;
    QMap<int, int> isRecv;

    QUdpSocket *udp;
    QTimer *timer;
    quint16 recvPort;
    QHostAddress recvAddr;
    QByteArray tmpByte;
};

#endif // APPSOCKET_H
