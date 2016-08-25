#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushOpen->setEnabled(true);
    ui->pushClose->setEnabled(false);

    portValidator = new QIntValidator(0, 65535, this);
    ui->lineUDPPort->setValidator(portValidator);
    ui->lineTCPPort->setValidator(portValidator);

    UDPSocket = new QUdpSocket(this);
    TCPSocket = new QTcpSocket(this);


    // Agregar comandos
    QStringList commands;
    commands << "GET" << "SET" << "RESET" << "NODE ON" << "NODE OFF";
    ui->comboCommand->addItems(commands);

    // Agregar registros
    QStringList registers;
    registers << "V_RMS" <<"I_RMS" << "POWER_FACTOR" << "FREQUENCY" << "ACTIVE_POWER" << "TOTAL_POWER" <<
                 "CURRENT_HOUR_ENERGY" << "DEVICE_ID" << "LOAD_STATE" <<
                 "MONDAY_LOAD_ON_TIME" << "MONDAY_LOAD_OF_TIME" <<
                 "TUESDAY_LOAD_ON_TIME" << "TUESDAY_LOAD_OF_TIME" <<
                 "WEDNESDAY_LOAD_ON_TIME" << "WEDNESDAY_LOAD_OF_TIME" <<
                 "THURSDAY_LOAD_ON_TIME" << "THURSDAY_LOAD_OF_TIME" <<
                 "FRIDAY_LOAD_ON_TIME" << "FRIDAY_LOAD_OF_TIME" <<
                 "SATURDAY_LOAD_ON_TIME" << "SATURDAY_LOAD_OF_TIME" <<
                 "SUNDAY_LOAD_ON_TIME" << "SUNDAY_LOAD_OF_TIME" <<
                 "PER_HOUR_ENERGY" << "PER_HOUR_ACIVE_POWER";
    ui->comboRegister->addItems(registers);

    connect(UDPSocket, SIGNAL(readyRead()), this, SLOT(readPendingUDPDatagram()));
    connect (ui->listConnected, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getMACString(QByteArray MAC)
{
    QString strMAC;
    int m1 = MAC.at(0) & 0xFF;
    int m2 = MAC.at(1) & 0xFF;
    int m3 = MAC.at(2) & 0xFF;
    int m4 = MAC.at(3) & 0xFF;
    int m5 = MAC.at(4) & 0xFF;
    int m6 = MAC.at(5) & 0xFF;
    strMAC = QString("%1:%2:%3:%4:%5:%6").arg(MAC.at(0) & 0xFF, 2, 16, QChar('0')).
                                         arg(MAC.at(1) & 0xFF, 2, 16, QChar('0')).
                                         arg(MAC.at(2) & 0xFF, 2, 16, QChar('0')).
                                         arg(MAC.at(3) & 0xFF, 2, 16, QChar('0')).
                                         arg(MAC.at(4) & 0xFF, 2, 16, QChar('0')).
                                         arg(MAC.at(5) & 0xFF, 2, 16, QChar('0'));

    return strMAC;
}

void MainWindow::readPendingUDPDatagram()
{
    QByteArray receivedData;
    QHostAddress sender;
    quint16 senderPort;
    int i;

    while (UDPSocket->hasPendingDatagrams())
    {
        UDPDatagram.resize(UDPSocket->pendingDatagramSize());

        UDPSocket->readDatagram(UDPDatagram.data(), UDPDatagram.size(), &sender, &senderPort);

        receivedData.append(UDPDatagram);
    }


    // Se obtienen los campos de interés de la trama
    bool ipv6 = (sender.protocol() == QAbstractSocket::IPv6Protocol)? true : false;
    QString ip;
    if (ipv6)
        ip = sender.toString().remove(0,7);
    else
        ip = sender.toString();

    QByteArray MACAddress = receivedData.mid(0,6);
    quint8 rssi = receivedData.mid(7,1).at(0);
    quint16 localPort = receivedData.mid(8,2).toInt();
    QString ID = QString(receivedData.mid(60,32));

    // Se busca en la lista de los dispositivos conectados previamente y si está se actualizan los datos.
    // En caso de no estar, se lo agrega.
    for (i = 0; i < ui->listConnected->count(); i++)
    {
        QListWidgetItem* item = ui->listConnected->item(i);

        // En UserRole se guarda un puntero a SmartPlugConnection
        SmartPlugConnection* value = item->data(Qt::UserRole).value<SmartPlugConnection*>();

        if (MACAddress == value->getMACAddress())
        {
            // Se encontró la MAC por lo que ya está en la lista. Se actualizan los datos.
            value->setRSSI(rssi);
            value->setLastTime(QDateTime::currentDateTime());
            value->setIPAddress(sender);

            // Si es el current item, actualizar la GUI
            if (ui->listConnected->currentItem() == item)
            {
                ui->labelRSSI->setText(QString::number(rssi));
                ui->labelIP->setText(value->getIPAddress().toString());
                ui->labelLastTime->setText(value->getLastTime().toString("hh:mm:ss - dd/MM/yyyy"));
            }

            break;
        }
    }

    if (i >= ui->listConnected->count())
    {
        // No se encontró la MAC, se la agrega a la lista.
        SmartPlugConnection* newConn = new SmartPlugConnection ();
        newConn->setID(ID);
        newConn->setIPAddress(sender);
        newConn->setLastTime(QDateTime::currentDateTime());
        newConn->setMACAddress(MACAddress);
        newConn->setPort(localPort);
        newConn->setRSSI(rssi);

        QListWidgetItem* item = new QListWidgetItem(ID);
        item->setData(Qt::UserRole, QVariant::fromValue(newConn));
        ui->listConnected->addItem(item);
    }
}

void MainWindow::listItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    // Cuando se cambia el item seleccionado de la lista se actualiza la GUI.
    SmartPlugConnection* value = current->data(Qt::UserRole).value<SmartPlugConnection*>();

    ui->labelID->setText(value->getID());
    ui->labelPORT->setText(QString::number(value->getPort()));
    ui->labelMAC->setText(getMACString(value->getMACAddress()));
    ui->labelRSSI->setText(QString::number(value->getRSSI()));
    ui->labelIP->setText(value->getIPAddress().toString());
    ui->labelLastTime->setText(value->getLastTime().toString("hh:mm:ss - dd/MM/yyyy"));
}

void MainWindow::on_pushOpen_clicked()
{
    ui->pushOpen->setEnabled(false);
    ui->pushClose->setEnabled(true);

    UDPSocket->bind(ui->lineUDPPort->text().toUInt());
}

void MainWindow::on_pushClose_clicked()
{
    ui->pushOpen->setEnabled(true);
    ui->pushClose->setEnabled(false);

    UDPSocket->close();
}



void MainWindow::on_pushSend_clicked()
{

}

void MainWindow::on_comboCommand_currentIndexChanged(const QString &command)
{
    if (command == "SET")
    {
        ui->linePayload->setEnabled(true);
        ui->comboRegister->setEnabled(true);
    }
    else if (command == "GET")
    {
        ui->linePayload->setEnabled(false);
        ui->comboRegister->setEnabled(true);
    }
    else if (command == "RESET")
    {
        ui->linePayload->setEnabled(false);
        ui->comboRegister->setEnabled(true);
    }
    else if (command == "NODE ON" || command == "NODE OFF")
    {
        ui->linePayload->setEnabled(false);
        ui->comboRegister->setEnabled(false);
    }

}

void MainWindow::on_comboRegister_currentIndexChanged(const QString &reg)
{

}
