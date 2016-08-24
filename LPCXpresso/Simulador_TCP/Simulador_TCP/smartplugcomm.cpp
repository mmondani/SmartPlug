#include "smartplugcomm.h"

SmartPlugComm::SmartPlugComm(QObject *parent) : QObject(parent)
{

}

void SmartPlugComm::sendMsg(QHostAddress destIP, int port, char command, char reg, QByteArray *data)
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    // Arma la trama a enviar
    data2Send.clear();
    data2Send.append('#');
    data2Send.append('!');
    data2Send.append(data->length() + 4);
    data2Send.append(command);
    data2Send.append(reg);
    data2Send.append(*data);
    data2Send.append('#');
    data2Send.append('!');


    socket->connectToHost(destIP, port);

    if (socket->waitForConnected(5000))
    {
        qDebug() << "No se puso conectar con el Smart Plug";
    }
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
    delete socket;
    data2Send.clear();
    dataReceived.clear();
}

void SmartPlugComm::readyRead()
{
    // Llegó información del Smart Plug se la parsea
    dataReceived.append(socket->readAll());

    if (dataReceived.length() >= 3)
    {
        // Ya llegaron por lo menos 3 bytes. Se obtiene la longitud de la trama a esperar.
        length2Receive = dataReceived.at(2);
    }

    if (dataReceived.length() >= (length2Receive + 3))
    {
        // Llegó la trama completa
        SmartPlugMsg_t msg;

        msg.len = length2Receive - 4;       // Se le restan los 2 bytes de COMANDO y REGISTRO y los dos bytes finales (#!)
        msg.command = dataReceived.at(3);
        msg.reg = dataReceived.at(4);
        msg.rawData = dataReceived.mid(5, msg.len);

        emit newMsg(msg);
    }
}

