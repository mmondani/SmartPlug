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

private:
    QString getMACString (QByteArray MAC);

private slots:
    void readPendingUDPDatagram ();
    void listItemChanged (QListWidgetItem* current, QListWidgetItem* previous);
    void on_pushOpen_clicked();
    void on_pushClose_clicked();

    void on_pushSend_clicked();

    void on_comboCommand_currentIndexChanged(const QString &command);

    void on_comboRegister_currentIndexChanged(const QString &reg);

private:
    Ui::MainWindow *ui;
    QIntValidator* portValidator;
    QUdpSocket* UDPSocket;
    QTcpSocket* TCPSocket;
    QByteArray UDPDatagram;
};


#endif // MAINWINDOW_H
