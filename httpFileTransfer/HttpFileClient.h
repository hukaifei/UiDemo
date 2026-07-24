#ifndef HTTPFILECLIENT_H
#define HTTPFILECLIENT_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QSpinBox>

class HttpFileClient : public QWidget {
    Q_OBJECT

public:
    explicit HttpFileClient(QWidget *parent = nullptr);
    ~HttpFileClient();

private slots:
    void onUpload();
    void onDownload();
    void onUploadFinished(QNetworkReply *reply);
    void onDownloadFinished(QNetworkReply *reply);
    void onUploadProgress(qint64 sent, qint64 total);
    void onDownloadProgress(qint64 received, qint64 total);

private:
    void log(const QString &msg);
    QString serverUrl() const;

    QNetworkAccessManager *manager;
    QTextEdit *logEdit;
    QPushButton *uploadBtn;
    QPushButton *downloadBtn;
    QLineEdit *hostEdit;
    QSpinBox *portSpin;
    QLineEdit *downloadFileEdit;
    QProgressBar *progressBar;
    QLabel *statusLabel;
};

#endif
