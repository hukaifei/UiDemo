#include "WebEngineDemo.h"

#include <QWebEngineView>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QWebEnginePage>
#include <QMessageBox>
#include <QUrl>

WebEngineDemo::WebEngineDemo(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("QWebEngineView Demo"));
    resize(1024, 680);
    setupUi();
    setupToolBar();

    updateNavigationButtons();

    loadLocalTestPage();
}

void WebEngineDemo::setupUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *barLayout = new QVBoxLayout();
    barLayout->setContentsMargins(6, 6, 6, 4);

    auto *urlLayout = new QHBoxLayout();

    m_urlBar = new QLineEdit(central);
    m_urlBar->setPlaceholderText(QStringLiteral("输入网址后按回车..."));
    m_urlBar->setClearButtonEnabled(true);
    urlLayout->addWidget(m_urlBar, 1);

    m_btnLocal = new QPushButton(QStringLiteral("📄 本地测试页"), central);
    urlLayout->addWidget(m_btnLocal);

    barLayout->addLayout(urlLayout);

    m_progress = new QProgressBar(central);
    m_progress->setMaximumHeight(4);
    m_progress->setTextVisible(false);
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    m_progress->hide();
    barLayout->addWidget(m_progress);

    m_statusLabel = new QLabel(central);
    m_statusLabel->setMinimumHeight(20);
    barLayout->addWidget(m_statusLabel);

    layout->addLayout(barLayout);

    m_webView = new QWebEngineView(central);
    m_webView->setMinimumSize(640, 400);
    layout->addWidget(m_webView, 1);

    connect(m_urlBar, &QLineEdit::returnPressed,
            this, &WebEngineDemo::navigateToUrl);
    connect(m_btnLocal, &QPushButton::clicked,
            this, &WebEngineDemo::loadLocalTestPage);
    connect(m_webView, &QWebEngineView::loadStarted,
            this, &WebEngineDemo::onLoadStarted);
    connect(m_webView, &QWebEngineView::loadFinished,
            this, &WebEngineDemo::onLoadFinished);
    connect(m_webView, &QWebEngineView::loadProgress,
            this, &WebEngineDemo::onLoadProgress);
    connect(m_webView, &QWebEngineView::urlChanged,
            this, &WebEngineDemo::onUrlChanged);

    statusBar()->showMessage(QStringLiteral("欢迎使用 QWebEngineView Demo"), 5000);
}

void WebEngineDemo::setupToolBar()
{
    auto *toolBar = addToolBar(QStringLiteral("导航"));

    m_btnBack = new QPushButton(QStringLiteral("◀ 后退"), this);
    m_btnForward = new QPushButton(QStringLiteral("▶ 前进"), this);
    m_btnRefresh = new QPushButton(QStringLiteral("🔄 刷新"), this);
    m_btnStop = new QPushButton(QStringLiteral("⏹ 停止"), this);

    toolBar->addWidget(m_btnBack);
    toolBar->addWidget(m_btnForward);
    toolBar->addWidget(m_btnRefresh);
    toolBar->addWidget(m_btnStop);

    connect(m_btnBack, &QPushButton::clicked,
            this, &WebEngineDemo::goBack);
    connect(m_btnForward, &QPushButton::clicked,
            this, &WebEngineDemo::goForward);
    connect(m_btnRefresh, &QPushButton::clicked,
            this, &WebEngineDemo::refresh);
    connect(m_btnStop, &QPushButton::clicked,
            this, &WebEngineDemo::stop);
}

void WebEngineDemo::onLoadStarted()
{
    m_progress->show();
    m_progress->setValue(0);
    m_statusLabel->setText(QStringLiteral("⏳ 正在加载..."));
}

void WebEngineDemo::onLoadFinished(bool ok)
{
    m_progress->hide();
    if (ok) {
        m_statusLabel->setText(QStringLiteral("✅ 加载完成"));
    } else {
        m_statusLabel->setText(QStringLiteral("❌ 加载失败"));
    }
    updateNavigationButtons();
}

void WebEngineDemo::onLoadProgress(int progress)
{
    m_progress->setValue(progress);
}

void WebEngineDemo::onUrlChanged(const QUrl &url)
{
    m_urlBar->setText(url.toString());
    updateNavigationButtons();
}

void WebEngineDemo::navigateToUrl()
{
    QString text = m_urlBar->text().trimmed();
    if (text.isEmpty())
        return;

    QUrl url(text);

    if (url.scheme().isEmpty()) {
        url.setScheme("https");
    }

    if (!url.isValid()) {
        QMessageBox::warning(this,
            QStringLiteral("无效网址"),
            QStringLiteral("请输入有效的 URL：\n%1").arg(text));
        return;
    }

    m_webView->load(url);
    m_urlBar->setText(url.toString());
}

void WebEngineDemo::goBack()
{
    m_webView->back();
}

void WebEngineDemo::goForward()
{
    m_webView->forward();
}

void WebEngineDemo::refresh()
{
    m_webView->reload();
}

void WebEngineDemo::stop()
{
    m_webView->stop();
}

void WebEngineDemo::loadLocalTestPage()
{
    const QString html = QStringLiteral(
        "<!DOCTYPE html>"
        "<html lang=\"zh-CN\">"
        "<head>"
        "<meta charset=\"utf-8\">"
        "<title>本地测试页面</title>"
        "<style>"
        "  * { margin: 0; padding: 0; box-sizing: border-box; }"
        "  body {"
        "    font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
        "    min-height: 100vh; display: flex; align-items: center; justify-content: center;"
        "  }"
        "  .card {"
        "    background: white; border-radius: 16px; padding: 48px 40px;"
        "    box-shadow: 0 20px 60px rgba(0,0,0,0.3); max-width: 520px; text-align: center;"
        "  }"
        "  h1 { color: #333; font-size: 28px; margin-bottom: 12px; }"
        "  p { color: #666; font-size: 16px; line-height: 1.7; margin-bottom: 20px; }"
        "  .badge {"
        "    display: inline-block; background: #667eea; color: white;"
        "    border-radius: 20px; padding: 6px 18px; font-size: 14px; margin: 4px;"
        "  }"
        "  button {"
        "    background: #667eea; color: white; border: none;"
        "    padding: 12px 32px; border-radius: 8px; font-size: 16px;"
        "    cursor: pointer; margin-top: 16px; transition: background 0.2s;"
        "  }"
        "  button:hover { background: #5a6fd6; }"
        "  #counter { font-weight: bold; font-size: 20px; color: #764ba2; }"
        "  .links { margin-top: 16px; }"
        "  .links a { color: #667eea; text-decoration: none; margin: 0 8px; font-size: 14px; }"
        "  .links a:hover { text-decoration: underline; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class=\"card\">"
        "  <h1>🚀 QWebEngineView Demo</h1>"
        "  <p>这是一个由 <strong>QWebEngineView</strong> 渲染的本地 HTML 页面。</p>"
        "  <div>"
        "    <span class=\"badge\">Qt WebEngine</span>"
        "    <span class=\"badge\">Chromium 内核</span>"
        "    <span class=\"badge\">HTML5</span>"
        "  </div>"
        "  <p style=\"margin-top:20px;\">"
        "    点击次数：<span id=\"counter\">0</span>"
        "  </p>"
        "  <button onclick=\"increment()\">点我计数</button>"
        "  <div class=\"links\">"
        "    <a href=\"https://www.qt.io\" target=\"_blank\">Qt 官网</a>"
        "    <a href=\"https://doc.qt.io/qt-6/qwebengineview.html\" target=\"_blank\">API 文档</a>"
        "    <a href=\"https://github.com\" target=\"_blank\">GitHub</a>"
        "  </div>"
        "</div>"
        "<script>"
        "  let count = 0;"
        "  function increment() {"
        "    count++;"
        "    document.getElementById('counter').textContent = count;"
        "  }"
        "</script>"
        "</body>"
        "</html>"
    );

    m_webView->setHtml(html, QUrl("local://testpage/"));
    m_urlBar->setText(QStringLiteral("local://testpage/"));
}

void WebEngineDemo::updateNavigationButtons()
{
    if (!m_webView)
        return;

    auto *history = m_webView->history();
    m_btnBack->setEnabled(history->canGoBack());
    m_btnForward->setEnabled(history->canGoForward());
}
