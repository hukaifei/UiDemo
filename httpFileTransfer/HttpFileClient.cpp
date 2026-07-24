#include "HttpFileClient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>
#include <QUrl>
#include <QHttpMultiPart>
#include <QSpinBox>
#include <QMessageBox>
#include <QRegularExpression>

HttpFileClient::HttpFileClient(QWidget *parent)
    : QWidget(parent), manager(new QNetworkAccessManager(this)) {
    setWindowTitle("HTTP文件传输客户端");
    resize(550, 450);

    // 服务器地址
    auto *addrLayout = new QHBoxLayout();
    addrLayout->addWidget(new QLabel("服务器:"));
    hostEdit = new QLineEdit("127.0.0.1", this);
    addrLayout->addWidget(hostEdit, 1);
    addrLayout->addWidget(new QLabel("端口:"));
    portSpin = new QSpinBox(this);
    portSpin->setRange(1024, 65535);
    portSpin->setValue(8080);
    addrLayout->addWidget(portSpin);

    // 进度条
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);

    statusLabel = new QLabel("就绪", this);

    // 上传区域
    uploadBtn = new QPushButton("选择文件并上传", this);

    // 下载区域
    auto *downloadLayout = new QHBoxLayout();
    downloadFileEdit = new QLineEdit(this);
    downloadFileEdit->setPlaceholderText("输入要下载的文件名...");
    downloadBtn = new QPushButton("下载文件", this);
    downloadLayout->addWidget(downloadFileEdit, 1);
    downloadLayout->addWidget(downloadBtn);

    // 日志
    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);

    // 布局
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(addrLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(new QLabel("上传文件:"));
    mainLayout->addWidget(uploadBtn);
    mainLayout->addWidget(new QLabel("下载文件 (输入服务器上的文件名):"));
    mainLayout->addLayout(downloadLayout);
    mainLayout->addWidget(new QLabel("操作日志:"));
    mainLayout->addWidget(logEdit, 1);

    connect(uploadBtn, &QPushButton::clicked, this, &HttpFileClient::onUpload);
    connect(downloadBtn, &QPushButton::clicked, this, &HttpFileClient::onDownload);
}

HttpFileClient::~HttpFileClient() = default;

QString HttpFileClient::serverUrl() const {
    return QString("http://%1:%2").arg(hostEdit->text()).arg(portSpin->value());
}

void HttpFileClient::onUpload() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择要上传的文件");
    if (filePath.isEmpty()) return;

    QFileInfo fi(filePath);
    QFile *file = new QFile(filePath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        log(QString("无法打开文件: %1").arg(filePath));
        delete file;
        return;
    }

    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
        QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(fi.fileName())));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader,
        QVariant("application/octet-stream"));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // multiPart 负责释放 file
    multiPart->append(filePart);

    QNetworkRequest request(QUrl(serverUrl() + "/upload"));
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    progressBar->setVisible(true);
    progressBar->setValue(0);
    statusLabel->setText("正在上传...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onUploadFinished(reply);
    });
    connect(reply, &QNetworkReply::uploadProgress, this, &HttpFileClient::onUploadProgress);

    log(QString("[%1] 开始上传: %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(fi.fileName()));
}

void HttpFileClient::onDownload() {
    QString fileName = downloadFileEdit->text().trimmed();
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要下载的文件名");
        return;
    }

    QNetworkRequest request(QUrl(serverUrl() + "/download/" + fileName));
    QNetworkReply *reply = manager->get(request);

    progressBar->setVisible(true);
    progressBar->setValue(0);
    statusLabel->setText("正在下载...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDownloadFinished(reply);
    });
    connect(reply, &QNetworkReply::downloadProgress, this, &HttpFileClient::onDownloadProgress);

    log(QString("[%1] 开始下载: %2")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(fileName));
}

void HttpFileClient::onUploadFinished(QNetworkReply *reply) {
    progressBar->setVisible(false);

    if (reply->error() == QNetworkReply::NoError) {
        log(QString("[%1] 上传成功")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        statusLabel->setText("上传完成");
    } else {
        log(QString("[%1] 上传失败: %2")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(reply->errorString()));
        statusLabel->setText("上传失败");
    }
    reply->deleteLater();
}

void HttpFileClient::onDownloadFinished(QNetworkReply *reply) {
    progressBar->setVisible(false);

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();

        // 从 Content-Disposition 获取文件名
        QString fileName = downloadFileEdit->text().trimmed();
        QString disposition = reply->rawHeader("Content-Disposition");
        if (disposition.contains("filename=\"")) {
            QRegularExpression re("filename=\"([^\"]+)\"");
            auto match = re.match(disposition);
            if (match.hasMatch()) {
                fileName = match.captured(1);
            }
        }

        QString savePath = QFileDialog::getSaveFileName(this, "保存文件", fileName);
        if (!savePath.isEmpty()) {
            QFile file(savePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                log(QString("[%1] 下载完成，已保存到: %2 (%3 bytes)")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                    .arg(savePath)
                    .arg(data.size()));
                statusLabel->setText(QString("下载完成 (%1 KB)").arg(data.size() / 1024.0, 0, 'f', 1));
            } else {
                log(QString("[%1] 保存失败: %2")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                    .arg(file.errorString()));
                statusLabel->setText("保存失败");
            }
        }
    } else {
        log(QString("[%1] 下载失败: %2")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
            .arg(reply->errorString()));
        statusLabel->setText("下载失败");
    }
    reply->deleteLater();
}

void HttpFileClient::onUploadProgress(qint64 sent, qint64 total) {
    if (total > 0) {
        progressBar->setMaximum(static_cast<int>(total));
        progressBar->setValue(static_cast<int>(sent));
        statusLabel->setText(QString("上传中... %1%")
            .arg(static_cast<int>(sent * 100 / total)));
    }
}

void HttpFileClient::onDownloadProgress(qint64 received, qint64 total) {
    if (total > 0) {
        progressBar->setMaximum(static_cast<int>(total));
        progressBar->setValue(static_cast<int>(received));
        statusLabel->setText(QString("下载中... %1%")
            .arg(static_cast<int>(received * 100 / total)));
    }
}

void HttpFileClient::log(const QString &msg) {
    logEdit->append(msg);
}
