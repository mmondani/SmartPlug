#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    portValidator = new QIntValidator(0, 65535, this);
    UDPSocket = new QUdpSocket(this);
    TCPSocket = new QTcpSocket(this);

    UDPSocket->bind(QHostAddress::LocalHost, ui->lineUDPPort);

    connect(UDPSocket, SIGNAL(readyRead()), this, );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readPendingUDPDatagram()
{
    while (UDPSocket->hasPendingDatagrams())
    {
        QHostAddress sender;
        quint16 senderPort;

        UDPDatagram.resize(UDPSocket->pendingDatagramSize());
        UDPDatagram.clear();

        UDPSocket->readDatagram(UDPDatagram.data(), UDPDatagram.size(), &sender, &senderPort);

        // Se busca en la lista de los dispositivos conectados previamente y si está se actualizan los datos.
        // En caso de no estar, se lo agrega.
        for (int i; i < ui->listConnected->count(); i++);
        {
            QListWidgetItem* item = ui->listConnected->item(i);

        }
    }
}
