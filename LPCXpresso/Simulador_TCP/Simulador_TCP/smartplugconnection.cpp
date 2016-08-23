#include "smartplugconnection.h"

SmartPlugConnection::SmartPlugConnection(QObject *parent) : QObject(parent)
{
    // Guarda los 6 bytes de la dirección MAC
    MACAdress = new QByteArray(6);
}

void SmartPlugConnection::setMACAddress(QByteArray address)
{
    this->MACAdress = address;
}

void SmartPlugConnection::setRSSI(qint8 rssi)
{
    this->rssi = rssi;
}

void SmartPlugConnection::setPort(qint16 port)
{
    this->port = port;
}

void SmartPlugConnection::setID(QString ID)
{
    this->ID = ID;
}

void SmartPlugConnection::setIPAddress(QHostAddress address)
{
    this->ipAddress = address;
}

QByteArray SmartPlugConnection::getMACAddress()
{
    return MACAdress;
}

quint8 SmartPlugConnection::getRSSI()
{
    return rssi;
}

quint16 SmartPlugConnection::getPort()
{
    return port;
}

QString SmartPlugConnection::getID()
{
    return ID;
}

QHostAddress SmartPlugConnection::getIPAddress()
{
    return ipAddress;
}

