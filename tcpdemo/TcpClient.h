#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

class TcpClient : public QWidget {
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

private slots:
    void onConnect();
    void onSend();
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    QTcpSocket *socket;
    QTextEdit *logEdit;
    QLineEdit *messageEdit;
    QPushButton *connectBtn;
    QPushButton *sendBtn;
};

#endif
