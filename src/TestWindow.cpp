#include "TestWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include "customwidget.h"

TestWindow::TestWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("自定义控件测试 - 增益 / 补光灯亮度"));
    resize(560, 420);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    auto *title = new QLabel(QStringLiteral("📋 增益与补光灯亮度设置"), central);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    m_widget = new CustomWidget(central);
    mainLayout->addWidget(m_widget);

    mainLayout->addStretch();

    connect(m_widget, &CustomWidget::confirmed,
            this, &TestWindow::onConfirmed);
    connect(m_widget, &CustomWidget::brightnessChanged,
            this, &TestWindow::onBrightnessChanged);

    statusBar()->showMessage(
        QStringLiteral("默认已加载 5 个增益项，点击「确认」按钮提交设定。")
    );
}

void TestWindow::onConfirmed(int row, const QString &gainName, int brightness)
{
    const QString msg = QStringLiteral("[确认] 第 %1 行  %2  →  补光灯亮度 = %3 %")
                            .arg(row + 1)
                            .arg(gainName)
                            .arg(brightness);
    statusBar()->showMessage(msg, 4000);
}

void TestWindow::onBrightnessChanged(int row, int value)
{
    statusBar()->showMessage(
        QStringLiteral("第 %1 行亮度调节为 %2 %（未确认）").arg(row + 1).arg(value)
    );
}
