#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QList>

class TcpServer : public QWidget {
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onStartServer();

private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
    QTextEdit *logEdit;
    QPushButton *startBtn;
    quint16 port;
};

#endif
