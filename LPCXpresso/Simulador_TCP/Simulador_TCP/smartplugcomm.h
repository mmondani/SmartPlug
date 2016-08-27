#ifndef SMARTPLUGCOMM_H
#define SMARTPLUGCOMM_H

#include <QObject>
#include <QByteArray>
#include <QHostAddress>
#include <QTcpSocket>
#include "smartplugdefinitions.h"


typedef struct
{
    char len;
    char command;
    char reg;
    QByteArray rawData;
}SmartPlugMsg_t;



class SmartPlugComm : public QObject
{
    Q_OBJECT
public:
    explicit SmartPlugComm(QObject *parent = 0);
    QByteArray &sendMsg(QString destIP, int port, char command, char reg, QByteArray data = 0);

signals:
    void newMsg (SmartPlugMsg_t msg);

public slots:

private slots:
    void connected ();
    void disconnected ();
    void readyRead ();

private:
    QTcpSocket* socket;
    QByteArray data2Send;
    QByteArray dataReceived;
    char length2Receive;
};

#endif // SMARTPLUGCOMM_H
