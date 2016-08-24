#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QIntValidator>
#include <QListWidget>
#include <smartplugconnection.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void readPendingUDPDatagram ();
    void listItemChanged (QListWidgetItem* current, QListWidgetItem* previous);

private:
    Ui::MainWindow *ui;
    QIntValidator* portValidator;
    QUdpSocket* UDPSocket;
    QTcpSocket* TCPSocket;
    QByteArray UDPDatagram;
};


#endif // MAINWINDOW_H
