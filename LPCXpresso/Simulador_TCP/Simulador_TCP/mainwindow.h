#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QIntValidator>
#include <QListWidget>
#include <math.h>
#include <smartplugconnection.h>
#include <smartplugcomm.h>
#include <smartplugdefinitions.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString getMACString (QByteArray MAC);
    void loadRegisterNameMap ();

private slots:
    void readPendingUDPDatagram ();
    void listItemChanged (QListWidgetItem* current, QListWidgetItem* previous);
    void on_pushOpen_clicked();
    void on_pushClose_clicked();
    void on_pushSend_clicked();
    void on_comboCommand_currentIndexChanged(const QString &command);
    void on_comboRegister_currentIndexChanged(const QString &reg);
    void newSmartPlugMsgReceived (SmartPlugMsg_t msg);

private:
    Ui::MainWindow *ui;
    QIntValidator* portValidator;
    QUdpSocket* UDPSocket;
    QTcpSocket* TCPSocket;
    QByteArray UDPDatagram;
    SmartPlugComm tcpComm;
    QMap<QString, quint8> registerNameMap;
    QMap<quint8, QString> registerValueMap;
    float hexToFloat(const QByteArray &_array);
};


#endif // MAINWINDOW_H
