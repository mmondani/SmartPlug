#ifndef SMARTPLUGCONNECTION_H
#define SMARTPLUGCONNECTION_H

#include <QMetaType>
#include <QHostAddress>
#include <QDateTime>


class SmartPlugConnection
{
public:
    explicit SmartPlugConnection();
    void setMACAddress (QByteArray address);
    void setRSSI (quint8 rssi);
    void setPort (quint16 port);
    void setID (QString ID);
    void setIPAddress (QHostAddress address);
    void setLastTime (QDateTime dateTime);

    QByteArray getMACAddress ();
    quint8 getRSSI ();
    quint16 getPort ();
    QString getID ();
    QHostAddress getIPAddress ();
    QDateTime getLastTime();

signals:

public slots:

private:
    QByteArray MACAdress;
    quint8 rssi;
    quint16 port;
    QString ID;
    QHostAddress ipAddress;
    QDateTime lastTime;
};

Q_DECLARE_METATYPE(SmartPlugConnection*)


#endif // SMARTPLUGCONNECTION_H
