#ifndef WEBENGINEDEMO_H
#define WEBENGINEDEMO_H

#include <QMainWindow>

class QWebEngineView;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QLabel;

class WebEngineDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit WebEngineDemo(QWidget *parent = nullptr);
    ~WebEngineDemo() override = default;

private slots:
    void onLoadStarted();
    void onLoadFinished(bool ok);
    void onLoadProgress(int progress);
    void onUrlChanged(const QUrl &url);
    void navigateToUrl();
    void goBack();
    void goForward();
    void refresh();
    void stop();
    void loadLocalTestPage();

private:
    void setupUi();
    void setupToolBar();
    void updateNavigationButtons();

    QWebEngineView *m_webView = nullptr;
    QLineEdit      *m_urlBar = nullptr;
    QProgressBar   *m_progress = nullptr;
    QPushButton    *m_btnBack = nullptr;
    QPushButton    *m_btnForward = nullptr;
    QPushButton    *m_btnRefresh = nullptr;
    QPushButton    *m_btnStop = nullptr;
    QPushButton    *m_btnLocal = nullptr;
    QLabel         *m_statusLabel = nullptr;
};

#endif // WEBENGINEDEMO_H
