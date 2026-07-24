#include "HttpFileServer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QMimeDatabase>
#include <QUrl>
#include <QStandardPaths>

HttpFileServer::HttpFileServer(QWidget *parent)
    : QWidget(parent), server(nullptr), port(8080) {
    setWindowTitle("HTTP文件服务器");
    resize(600, 500);

    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);

    startStopBtn = new QPushButton("启动服务", this);
    portSpin = new QSpinBox(this);
    portSpin->setRange(1024, 65535);
    portSpin->setValue(port);

    fileList = new QListWidget(this);

    auto *topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("端口:"));
    topLayout->addWidget(portSpin);
    topLayout->addWidget(startStopBtn);
    topLayout->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(new QLabel("服务器文件列表:"));
    mainLayout->addWidget(fileList, 1);
    mainLayout->addWidget(new QLabel("请求日志:"));
    mainLayout->addWidget(logEdit, 2);

    connect(startStopBtn, &QPushButton::clicked, this, &HttpFileServer::onStartStop);

    // 确保接收目录存在
    QDir().mkpath(serverDir());
    refreshFileList();
}

HttpFileServer::~HttpFileServer() {
    if (server) {
        server->close();
    }
}

void HttpFileServer::onStartStop() {
    if (server) {
        // 停止服务器
        server->close();
        for (auto *c : clients) {
            c->close();
            c->deleteLater();
        }
        clients.clear();
        server->deleteLater();
        server = nullptr;
        startStopBtn->setText("启动服务");
        portSpin->setEnabled(true);
        log(QString("服务器已停止"));
    } else {
        // 启动服务器
        port = portSpin->value();
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &HttpFileServer::onNewConnection);

        if (server->listen(QHostAddress::Any, port)) {
            log(QString("服务器启动成功，监听端口: %1").arg(port));
            log(QString("上传地址: http://localhost:%1/upload").arg(port));
            log(QString("下载地址: http://localhost:%1/download/文件名").arg(port));
            startStopBtn->setText("停止服务");
            portSpin->setEnabled(false);
        } else {
            log(QString("服务器启动失败: %1").arg(server->errorString()));
        }
    }
}

void HttpFileServer::onNewConnection() {
    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);

    connect(client, &QTcpSocket::readyRead, this, &HttpFileServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &HttpFileServer::onDisconnected);

    log(QString("[%1] 新连接: %2:%3")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(client->peerAddress().toString())
        .arg(client->peerPort()));
}

void HttpFileServer::onReadyRead() {
    auto *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QByteArray data = client->readAll();
    handleRequest(client, data);
}

void HttpFileServer::onDisconnected() {
    auto *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        log(QString("[%1] 断开连接: %2:%3")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(client->peerAddress().toString())
            .arg(client->peerPort()));

        clients.removeOne(client);
        client->deleteLater();
    }
}

void HttpFileServer::handleRequest(QTcpSocket *client, const QByteArray &request) {
    QString reqText = QString::fromUtf8(request);
    QStringList lines = reqText.split("\r\n");

    if (lines.isEmpty()) return;

    QStringList requestLine = lines[0].split(' ');
    if (requestLine.size() < 2) return;

    QString method = requestLine[0];
    QString path = requestLine[1];

    // 解析请求头
    QMap<QString, QString> headers;
    int i = 1;
    for (; i < lines.size(); ++i) {
        if (lines[i].isEmpty()) break; // 头部结束
        int colonIdx = lines[i].indexOf(':');
        if (colonIdx > 0) {
            headers[lines[i].left(colonIdx).trimmed().toLower()] =
                lines[i].mid(colonIdx + 1).trimmed();
        }
    }

    // 获取请求体
    QByteArray body;
    int contentLength = headers.value("content-length", "0").toInt();
    if (contentLength > 0) {
        // 找到头部结束后 (\r\n\r\n) 的 body 数据
        int headerEnd = request.indexOf("\r\n\r\n");
        if (headerEnd >= 0) {
            body = request.mid(headerEnd + 4);
        }
        // 如果 body 不完整，等待更多数据（简化处理：直接读可用数据）
    }

    log(QString("[%1] %2 %3")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(method).arg(path));

    if (method == "GET" && path.startsWith("/download/")) {
        // 下载文件: GET /download/filename
        QString fileName = QUrl::fromPercentEncoding(path.mid(10).toUtf8());
        QString filePath = serverDir() + "/" + fileName;
        if (QFile::exists(filePath)) {
            log(QString("  -> 发送文件: %1").arg(fileName));
            sendFileResponse(client, filePath);
        } else {
            log(QString("  -> 文件不存在: %1").arg(fileName));
            sendResponse(client, 404, "Not Found", "text/html", "<h1>404 - 文件不存在</h1>");
        }
    } else if (method == "POST" && path == "/upload") {
        // 上传文件: POST /upload
        saveUploadedFile(client, body, headers.value("content-type"));
    } else if (path == "/") {
        sendResponse(client, 200, "OK", "text/html",
            "<h2>HTTP File Transfer Server</h2>"
            "<p>上传: POST /upload</p>"
            "<p>下载: GET /download/文件名</p>");
    } else {
        sendResponse(client, 404, "Not Found", "text/html", "<h1>404</h1>");
    }
}

void HttpFileServer::sendFileResponse(QTcpSocket *client, const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        sendResponse(client, 500, "Internal Server Error", "text/html",
            "<h1>500 - 无法读取文件</h1>");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QMimeDatabase mimeDb;
    QString mimeType = mimeDb.mimeTypeForFile(filePath).name();
    QFileInfo fi(filePath);

    QByteArray response;
    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + mimeType.toUtf8() + "\r\n";
    response += "Content-Disposition: attachment; filename=\"" + fi.fileName().toUtf8() + "\"\r\n";
    response += "Content-Length: " + QByteArray::number(fileData.size()) + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += fileData;

    client->write(response);
    client->flush();
    client->disconnectFromHost();
}

void HttpFileServer::saveUploadedFile(QTcpSocket *client, const QByteArray &body,
                                       const QString &contentType) {
    // 从 multipart/form-data 中提取文件名和内容
    QString boundary;
    if (contentType.startsWith("multipart/form-data")) {
        QRegularExpression re("boundary=(.+)");
        auto match = re.match(contentType);
        if (match.hasMatch()) {
            boundary = "--" + match.captured(1);
        }
    }

    QString fileName = "uploaded_file";
    QByteArray fileContent;

    if (!boundary.isEmpty() && !body.isEmpty()) {
        // 解析 multipart
        QString bodyStr = QString::fromUtf8(body);
        QStringList parts = bodyStr.split(boundary);

        for (const QString &part : parts) {
            if (part.contains("filename=\"")) {
                // 提取文件名
                QRegularExpression fnRe("filename=\"([^\"]+)\"");
                auto fnMatch = fnRe.match(part);
                if (fnMatch.hasMatch()) {
                    fileName = fnMatch.captured(1);
                }

                // 提取文件内容（头部后的 \r\n\r\n）
                int contentStart = part.indexOf("\r\n\r\n");
                if (contentStart >= 0) {
                    // 从原始 body 中找到对应位置
                    int bodyOffset = body.indexOf(part.toUtf8());
                    if (bodyOffset >= 0) {
                        int partContentStart = bodyOffset + contentStart + 4;
                        // 去掉末尾的 \r\n
                        int partEnd = body.lastIndexOf(boundary.toUtf8()) - 2;
                        if (partEnd > partContentStart) {
                            fileContent = body.mid(partContentStart, partEnd - partContentStart);
                        } else {
                            fileContent = body.mid(partContentStart);
                        }
                    }
                }
                break;
            }
        }
    } else {
        // 原始 body 就是文件内容
        fileContent = body;
        // 尝试从 Content-Disposition 获取文件名
        fileName = "upload_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".bin";
    }

    QString safeName = fileName;
    safeName.replace('/', '_').replace('\\', '_');

    QString filePath = serverDir() + "/" + safeName;
    // 处理重名
    QString base = safeName;
    int dotIdx = base.lastIndexOf('.');
    QString stem = dotIdx > 0 ? base.left(dotIdx) : base;
    QString ext = dotIdx > 0 ? base.mid(dotIdx) : "";
    int counter = 1;
    while (QFile::exists(filePath)) {
        filePath = serverDir() + "/" + stem + "_" + QString::number(counter++) + ext;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(fileContent);
        file.close();
        log(QString("  -> 文件已保存: %1 (%2 bytes)")
            .arg(QFileInfo(filePath).fileName())
            .arg(fileContent.size()));
        refreshFileList();
        sendResponse(client, 200, "OK", "text/html",
            "<h1>上传成功</h1><p>文件: " + QFileInfo(filePath).fileName().toUtf8() +
            "</p><p>大小: " + QByteArray::number(fileContent.size()) + " bytes</p>");
    } else {
        log(QString("  -> 保存失败: %1").arg(file.errorString()));
        sendResponse(client, 500, "Internal Server Error", "text/html",
            "<h1>500 - 保存文件失败</h1>");
    }
}

void HttpFileServer::sendResponse(QTcpSocket *client, int statusCode,
                                   const QString &statusText,
                                   const QByteArray &contentType,
                                   const QByteArray &body) {
    QByteArray response;
    response += "HTTP/1.1 " + QByteArray::number(statusCode) + " " + statusText.toUtf8() + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    client->write(response);
    client->flush();
    client->disconnectFromHost();
}

void HttpFileServer::refreshFileList() {
    fileList->clear();
    QDir dir(serverDir());
    for (const QFileInfo &fi : dir.entryInfoList(QDir::Files, QDir::Name)) {
        fileList->addItem(QString("%1  (%2 KB)")
            .arg(fi.fileName())
            .arg(fi.size() / 1024.0, 0, 'f', 1));
    }
}

void HttpFileServer::log(const QString &msg) {
    logEdit->append(msg);
}

QString HttpFileServer::serverDir() const {
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/httpFileServer";
}
