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



    connect(UDPSocket, SIGNAL(readyRead()), this, SLOT(readPendingUDPDatagram()));
    connect (ui->listConnected, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->labelMAC->setText(QString(value->getMACAddress()));
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
