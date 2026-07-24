#ifndef HTTPFILESERVER_H
#define HTTPFILESERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QListWidget>
#include <QFile>

class HttpFileServer : public QWidget {
    Q_OBJECT

public:
    explicit HttpFileServer(QWidget *parent = nullptr);
    ~HttpFileServer();

private slots:
    void onStartStop();
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void handleRequest(QTcpSocket *client, const QByteArray &request);
    void sendFileResponse(QTcpSocket *client, const QString &filePath);
    void saveUploadedFile(QTcpSocket *client, const QByteArray &body, const QString &contentType);
    void sendResponse(QTcpSocket *client, int statusCode, const QString &statusText,
                      const QByteArray &contentType, const QByteArray &body);
    void refreshFileList();
    void log(const QString &msg);
    QString serverDir() const;

    QTcpServer *server;
    QList<QTcpSocket*> clients;
    QTextEdit *logEdit;
    QPushButton *startStopBtn;
    QSpinBox *portSpin;
    QListWidget *fileList;
    quint16 port;
};

#endif
