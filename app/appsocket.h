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
    ~AppSocket();
private slots:
    void initUI();
    void initSkin();
    void initTitle();
    void initLayout();
    void initRecvBar();
    void initSendBar();
    void initSettings();
    void saveSettings();
    void openSocketDev();
    void recvSocketDev();
    void sendSocketDev();
    void sendCurrentText();
    void sendCurrentList();
    void stopCurrentLoop();
    void sendCurrentByte(QByteArray msg);
    void display(QByteArray msg);
    void appendCurrentItem();
    void deleteCurrentItem();
    void updateCurrentItem(int r, int c);
    virtual void hideEvent(QHideEvent *e);
private:
    QGridLayout *layout;
    QTextEdit *textUdpRecv;
    QComboBox *boxHostAddr;
    QComboBox *boxHostPort;
    QCheckBox *boxAutoSpace;
    QCheckBox *boxGetToHEX;
    QLineEdit *textPutTime;
    QLineEdit *textPutRate;
    QCheckBox *boxLoopSend;
    QCheckBox *boxPutToHEX;
    QLineEdit *textUdpSend;
    QTableWidget *tabList;

    QStringList tabTexts;
    QStringList tabNotes;

    QUdpSocket *udp;
    QTimer *timer;
    int timeOut;
    int sendOut;
    quint16 recvPort;
    QHostAddress recvAddr;
    QByteArray tmpByte;
};

#endif // APPSOCKET_H
