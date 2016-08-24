#include "smartplugconnection.h"

SmartPlugConnection::SmartPlugConnection()
{
}

void SmartPlugConnection::setMACAddress(QByteArray address)
{
    this->MACAdress = address;
}

void SmartPlugConnection::setRSSI(quint8 rssi)
{
    this->rssi = rssi;
}

void SmartPlugConnection::setPort(quint16 port)
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

void SmartPlugConnection::setLastTime(QDateTime dateTime)
{
    this->lastTime = dateTime;
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

QDateTime SmartPlugConnection::getLastTime()
{
    return lastTime;
}

