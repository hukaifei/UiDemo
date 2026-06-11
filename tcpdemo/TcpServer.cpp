#include "TcpServer.h"
#include <QVBoxLayout>
#include <QDateTime>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent), server(nullptr), port(8888) {
    
    setWindowTitle("TCP服务端");
    resize(500, 400);
    
    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);
    
    startBtn = new QPushButton("启动服务器", this);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(logEdit);
    layout->addWidget(startBtn);
    
    connect(startBtn, &QPushButton::clicked, this, &TcpServer::onStartServer);
}

TcpServer::~TcpServer() {
    if (server) {
        server->close();
    }
}

void TcpServer::onStartServer() {
    if (!server) {
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
        
        if (server->listen(QHostAddress::Any, port)) {
            logEdit->append(QString("[%1] 服务器启动成功，监听端口: %2")
                .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                .arg(port));
            startBtn->setEnabled(false);
        } else {
            logEdit->append(QString("[%1] 服务器启动失败: %2")
                .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                .arg(server->errorString()));
        }
    }
}

void TcpServer::onNewConnection() {
    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);
    
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
    
    logEdit->append(QString("[%1] 新客户端连接: %2:%3")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(client->peerAddress().toString())
        .arg(client->peerPort()));
}

void TcpServer::onReadyRead() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        logEdit->append(QString("[%1] 收到消息: %2")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(QString::fromUtf8(data)));
        
        // 回显消息
        client->write("服务器收到: " + data);
    }
}

void TcpServer::onDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        logEdit->append(QString("[%1] 客户端断开连接: %2:%3")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(client->peerAddress().toString())
            .arg(client->peerPort()));
        
        clients.removeOne(client);
        client->deleteLater();
    }
}
