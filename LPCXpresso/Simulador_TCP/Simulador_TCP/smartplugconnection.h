#ifndef SMARTPLUGCONNECTION_H
#define SMARTPLUGCONNECTION_H

#include <QObject>
#include <QHostAddress>

class SmartPlugConnection : public QObject
{
    Q_OBJECT
public:
    explicit SmartPlugConnection(QObject *parent = 0);
    void setMACAddress (QByteArray address);
    void setRSSI (quint8 rssi);
    void setPort (quint16 port);
    void setID (QString ID);
    void setIPAddress (QHostAddress address);

    QByteArray getMACAddress ();
    quint8 getRSSI ();
    quint16 getPort ();
    QString getID ();
    QHostAddress getIPAddress ();

signals:

public slots:

private:
    QByteArray* MACAdress;
    quint8 rssi;
    quint16 port;
    QString ID;
    QHostAddress ipAddress;
};

#endif // SMARTPLUGCONNECTION_H
