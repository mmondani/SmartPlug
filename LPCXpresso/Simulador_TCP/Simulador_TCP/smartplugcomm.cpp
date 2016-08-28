#include "smartplugcomm.h"

SmartPlugComm::SmartPlugComm(QObject *parent) : QObject(parent)
{

}

QByteArray& SmartPlugComm::sendMsg(QString destIP, int port, char command, char reg, QByteArray data)
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    // Arma la trama a enviar
    data2Send.clear();

    data2Send.append('#');
    data2Send.append('!');

    if (command != CMD_NODE_OFF && command != CMD_NODE_ON)
        data2Send.append(data.length() + 4);
    else
        data2Send.append(data.length() + 3);

    data2Send.append(command);

    if (command != CMD_NODE_OFF && command != CMD_NODE_ON)
        data2Send.append(reg);

    data2Send.append(data);

    data2Send.append('#');
    data2Send.append('!');


    socket->connectToHost(destIP, port);

    return data2Send;
}

void SmartPlugComm::closeConn()
{
    if (socket->isOpen())
        socket->close();
}

void SmartPlugComm::connected()
{
    // Si se pudo enviar se envìa la trama armada
    socket->write(data2Send);

    dataReceived.clear();
}

void SmartPlugComm::disconnected()
{
    // Cuando se desconecta se destruye el socket
    socket->deleteLater();
    data2Send.clear();
    dataReceived.clear();
}

void SmartPlugComm::readyRead()
{
    // Llegó información del Smart Plug se la parsea
    dataReceived.append(socket->readAll());

    if (QString(dataReceived) == "*HELLO*" || QString(dataReceived) == "*CLOS*")
        dataReceived.clear();


    if (dataReceived.length() >= 3)
    {
        // Ya llegaron por lo menos 3 bytes. Se obtiene la longitud de la trama a esperar.
        length2Receive = dataReceived.at(2);
    }

    if (dataReceived.length() >= (length2Receive + 3))
    {
        // Llegó la trama completa
        SmartPlugMsg_t msg;
        msg.command = dataReceived.at(3);
        if (msg.command != CMD_RESP_NODE_ON && msg.command != CMD_RESP_NODE_OFF)
        {
            msg.len = length2Receive - 4;       // Se le restan los 2 bytes de COMANDO y REGISTRO y los dos bytes finales (#!)
            msg.reg = dataReceived.at(4);
            msg.rawData = dataReceived.mid(5, msg.len);
        }
        else
        {
            msg.len = length2Receive - 3;       // Se le restan 1 byte de COMANDO y los dos bytes finales (#!)
            msg.reg = dataReceived.at(3);
            msg.rawData = dataReceived.mid(4, msg.len);
        }


        // Se eliminan los bytes ya procesados
        dataReceived.clear();

        emit newMsg(msg);
    }
}

