#include "TcpClient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent), socket(new QTcpSocket(this)) {
    
    setWindowTitle("TCP客户端");
    resize(500, 400);
    
    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);
    
    messageEdit = new QLineEdit(this);
    messageEdit->setPlaceholderText("输入消息...");
    
    connectBtn = new QPushButton("连接服务器", this);
    sendBtn = new QPushButton("发送", this);
    sendBtn->setEnabled(false);
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageEdit);
    inputLayout->addWidget(sendBtn);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(logEdit);
    layout->addWidget(connectBtn);
    layout->addLayout(inputLayout);
    
    connect(connectBtn, &QPushButton::clicked, this, &TcpClient::onConnect);
    connect(sendBtn, &QPushButton::clicked, this, &TcpClient::onSend);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
}

TcpClient::~TcpClient() {
    socket->close();
}

void TcpClient::onConnect() {
    socket->connectToHost("127.0.0.1", 8888);
    logEdit->append(QString("[%1] 正在连接服务器...")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void TcpClient::onConnected() {
    logEdit->append(QString("[%1] 已连接到服务器")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    connectBtn->setEnabled(false);
    sendBtn->setEnabled(true);
}

void TcpClient::onDisconnected() {
    logEdit->append(QString("[%1] 与服务器断开连接")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    connectBtn->setEnabled(true);
    sendBtn->setEnabled(false);
}

void TcpClient::onSend() {
    QString message = messageEdit->text();
    if (!message.isEmpty()) {
        socket->write(message.toUtf8());
        logEdit->append(QString("[%1] 发送: %2")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(message));
        messageEdit->clear();
    }
}

void TcpClient::onReadyRead() {
    QByteArray data = socket->readAll();
    logEdit->append(QString("[%1] 收到: %2")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(QString::fromUtf8(data)));
}
