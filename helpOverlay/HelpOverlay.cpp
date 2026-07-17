#include "HelpOverlay.h"

#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPainterPath>
#include <QtMath>

// ─── 样式常量 ───
static const QColor OverlayColor(0, 0, 0, 140);
static const QColor CutoutBorderColor("#4A90D9");
static const QColor TooltipBg("#2C2C2C");
static const QColor TooltipTextColor(Qt::white);
static const int TooltipWidth = 320;
static const int CutoutBorderWidth = 2;
static const int AnimationDuration = 250;

// ═══════════════════════════════════════════
//  HelpOverlay 实现
// ═══════════════════════════════════════════

HelpOverlay::HelpOverlay(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    // 必须设置透明背景，否则 CompositionMode_Clear 会清除为黑色
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    // 覆盖父窗口整个区域
    if (parent)
    {
        setGeometry(parent->rect());
    }

    // ── 创建提示气泡 ──
    m_tooltipWidget = new QWidget(this);
    m_tooltipWidget->setObjectName(QStringLiteral("tooltipWidget"));
    m_tooltipWidget->setFixedWidth(TooltipWidth);
    m_tooltipWidget->setStyleSheet(
        QStringLiteral(
            "#tooltipWidget {"
            "  background-color: %1;"
            "  border-radius: 10px;"
            "  border: 1px solid #3A3A3A;"
            "}"
        ).arg(TooltipBg.name())
    );
    m_tooltipWidget->hide();

    // 阴影效果
    auto *shadow = new QGraphicsDropShadowEffect(m_tooltipWidget);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 100));
    m_tooltipWidget->setGraphicsEffect(shadow);

    // 布局
    auto *mainLayout = new QVBoxLayout(m_tooltipWidget);
    mainLayout->setContentsMargins(16, 14, 16, 14);
    mainLayout->setSpacing(8);

    // 步骤指示器
    m_stepIndicator = new QLabel(m_tooltipWidget);
    m_stepIndicator->setStyleSheet(QStringLiteral(
        "QLabel { color: #8E8E8E; font-size: 11px; }"
    ));
    mainLayout->addWidget(m_stepIndicator);

    // 标题
    m_titleLabel = new QLabel(m_tooltipWidget);
    m_titleLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: white; font-size: 15px; font-weight: bold; }"
    ));
    m_titleLabel->setWordWrap(true);
    mainLayout->addWidget(m_titleLabel);

    // 描述
    m_descLabel = new QLabel(m_tooltipWidget);
    m_descLabel->setStyleSheet(QStringLiteral(
        "QLabel { color: #C0C0C0; font-size: 13px; line-height: 1.6; }"
    ));
    m_descLabel->setWordWrap(true);
    mainLayout->addWidget(m_descLabel);

    // 按钮栏
    auto *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);

    m_skipBtn = new QPushButton(QStringLiteral("跳过"), m_tooltipWidget);
    m_skipBtn->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #8E8E8E; background: transparent; border: none;"
        "  font-size: 12px; padding: 6px 12px;"
        "}"
        "QPushButton:hover { color: #C0C0C0; }"
    ));
    m_skipBtn->setCursor(Qt::PointingHandCursor);
    btnLayout->addWidget(m_skipBtn);

    btnLayout->addStretch();

    m_prevBtn = new QPushButton(QStringLiteral("上一步"), m_tooltipWidget);
    m_prevBtn->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #C0C0C0; background: #3A3A3A; border: 1px solid #4A4A4A;"
        "  border-radius: 4px; font-size: 12px; padding: 6px 14px;"
        "}"
        "QPushButton:hover { background: #4A4A4A; color: white; }"
        "QPushButton:disabled { color: #555; background: #2A2A2A; }"
    ));
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    btnLayout->addWidget(m_prevBtn);

    m_nextBtn = new QPushButton(QStringLiteral("下一步"), m_tooltipWidget);
    m_nextBtn->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: white; background-color: #4A90D9; border: none;"
        "  border-radius: 4px; font-size: 12px; font-weight: bold;"
        "  padding: 6px 18px;"
        "}"
        "QPushButton:hover { background-color: #5BA0E9; }"
        "QPushButton:disabled { background-color: #3A3A3A; color: #666; }"
    ));
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    btnLayout->addWidget(m_nextBtn);

    mainLayout->addLayout(btnLayout);

    // ── 信号连接 ──
    connect(m_nextBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex + 1 < m_steps.size())
        {
            showStep(m_currentIndex + 1);
        }
        else
        {
            stopTour();
        }
    });

    connect(m_prevBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex > 0)
            showStep(m_currentIndex - 1);
    });

    connect(m_skipBtn, &QPushButton::clicked, this, [this]() {
        emit skipped();
        stopTour();
    });

    // 安装监视器以追踪父窗口变化
    if (parent)
    {
        parent->installEventFilter(this);
    }
}

void HelpOverlay::setSteps(const QVector<TourStep> &steps)
{
    m_steps = steps;
    m_currentIndex = -1;
}

void HelpOverlay::startTour()
{
    if (m_steps.isEmpty())
        return;

    m_currentIndex = 0;

    if (parentWidget())
    {
        setGeometry(parentWidget()->rect());
    }
    raise();
    show();
    showStep(m_currentIndex);
}

void HelpOverlay::stopTour()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_steps.size())
    {
        auto &step = m_steps[m_currentIndex];
        if (step.onLeave)
            step.onLeave();
    }

    m_tooltipWidget->hide();
    hide();
    m_currentIndex = -1;
    emit tourFinished();
}

void HelpOverlay::showStep(int index)
{
    if (index < 0 || index >= m_steps.size())
        return;

    // 离开上一步
    if (m_currentIndex >= 0 && m_currentIndex < m_steps.size())
    {
        auto &prevStep = m_steps[m_currentIndex];
        if (prevStep.onLeave)
            prevStep.onLeave();
    }

    m_currentIndex = index;
    auto &step = m_steps[m_currentIndex];

    // 进入新步骤
    if (step.onEnter)
        step.onEnter();

    emit stepChanged(m_currentIndex, m_steps.size());

    // 更新高亮区域
    if (step.targetWidget)
    {
        QPoint topLeft = step.targetWidget->mapTo(parentWidget(), QPoint(0, 0));
        QSize size = step.targetWidget->size();
        m_highlightRect = QRect(topLeft, size);
    }
    else
    {
        // 无目标时高亮父窗口中心区域
        int w = parentWidget() ? parentWidget()->width() / 3 : 200;
        int h = parentWidget() ? parentWidget()->height() / 3 : 150;
        QPoint center = parentWidget() ? parentWidget()->rect().center() : QPoint(0, 0);
        m_highlightRect = QRect(center.x() - w / 2, center.y() - h / 2, w, h);
    }

    // 扩展高亮区域
    m_highlightRect.adjust(
        -step.highlightPadding, -step.highlightPadding,
        step.highlightPadding, step.highlightPadding
    );

    // 更新控件文本
    m_titleLabel->setText(step.title);
    m_descLabel->setText(step.description);
    m_stepIndicator->setText(
        QStringLiteral("第 %1 / %2 步").arg(m_currentIndex + 1).arg(m_steps.size())
    );

    // 更新完成按钮文本
    if (m_currentIndex == m_steps.size() - 1)
    {
        m_nextBtn->setText(QStringLiteral("完成"));
    }
    else
    {
        m_nextBtn->setText(QStringLiteral("下一步"));
    }

    updateButtonsVisibility();
    positionTooltip(m_highlightRect);

    // 重绘蒙版
    animateTransition();
    update();
}

void HelpOverlay::positionTooltip(const QRect &highlightRect)
{
    if (!m_tooltipWidget || !parentWidget())
        return;

    const TourStep &step = m_steps[m_currentIndex];
    int tw = m_tooltipWidget->width();
    int th = m_tooltipWidget->sizeHint().height();
    int pw = parentWidget()->width();
    int ph = parentWidget()->height();
    int margin = 16;

    int x = 0, y = 0;

    // 根据指定位置计算坐标
    switch (step.tooltipPosition)
    {
    case TourStep::Top:
        x = highlightRect.center().x() - tw / 2;
        y = highlightRect.top() - th - margin;
        break;
    case TourStep::Bottom:
        x = highlightRect.center().x() - tw / 2;
        y = highlightRect.bottom() + margin;
        break;
    case TourStep::Left:
        x = highlightRect.left() - tw - margin;
        y = highlightRect.center().y() - th / 2;
        break;
    case TourStep::Right:
        x = highlightRect.right() + margin;
        y = highlightRect.center().y() - th / 2;
        break;
    case TourStep::Center:
        x = pw / 2 - tw / 2;
        y = ph / 2 - th / 2;
        break;
    }

    // 边界修正
    if (x + tw > pw)  x = pw - tw - margin;
    if (x < margin)   x = margin;
    if (y + th > ph)  y = ph - th - margin;
    if (y < margin)   y = margin;

    m_tooltipWidget->move(x, y);
    m_tooltipWidget->adjustSize();

    // 如果之前是隐藏的，用淡入动画
    if (!m_tooltipWidget->isVisible())
    {
        m_tooltipWidget->show();
    }
}

void HelpOverlay::updateButtonsVisibility()
{
    m_prevBtn->setVisible(m_currentIndex > 0);
    m_nextBtn->setVisible(true);
}

void HelpOverlay::animateTransition()
{
    m_tooltipWidget->show();

    if (m_opacityAnimation)
    {
        m_opacityAnimation->stop();
        delete m_opacityAnimation;
        m_opacityAnimation = nullptr;
    }

    QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(
        m_tooltipWidget->graphicsEffect()
    );
    // 我们只在 tooltip 上保留了阴影特效，这里不用 opacity 动画
    // 直接更新即可
}

QRect HelpOverlay::getHighlightRect() const
{
    return m_highlightRect;
}

void HelpOverlay::updateOverlayGeometry()
{
    if (parentWidget())
        setGeometry(parentWidget()->rect());
}

// ─── 事件处理 ───

bool HelpOverlay::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::Move))
    {
        updateOverlayGeometry();
        if (m_currentIndex >= 0 && m_currentIndex < m_steps.size())
        {
            const auto &step = m_steps[m_currentIndex];
            if (step.targetWidget)
            {
                QPoint topLeft = step.targetWidget->mapTo(parentWidget(), QPoint(0, 0));
                QSize size = step.targetWidget->size();
                m_highlightRect = QRect(topLeft, size);
                m_highlightRect.adjust(
                    -step.highlightPadding, -step.highlightPadding,
                    step.highlightPadding, step.highlightPadding
                );
            }
            positionTooltip(m_highlightRect);
            update();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void HelpOverlay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_currentIndex >= 0 && m_currentIndex < m_steps.size())
    {
        positionTooltip(m_highlightRect);
        update();
    }
}

// ─── 绘制 ───

void HelpOverlay::paintEvent(QPaintEvent * /*event*/)
{
    if (m_currentIndex < 0 || m_currentIndex >= m_steps.size())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const TourStep &step = m_steps[m_currentIndex];

    // ── 1. 构建"掏洞"路径：全窗口矩形 减去 高亮区域 ──
    QPainterPath fullPath;
    fullPath.addRect(rect());

    QPainterPath cutoutPath;
    cutoutPath.addRoundedRect(m_highlightRect, step.highlightRadius, step.highlightRadius);

    QPainterPath overlayPath = fullPath.subtracted(cutoutPath);

    // 填充覆盖层（高亮区域透明，其余区域半透明黑）
    painter.fillPath(overlayPath, OverlayColor);

    // ── 2. 绘制高亮区域边框 ──
    QPen borderPen(CutoutBorderColor, CutoutBorderWidth);
    borderPen.setCosmetic(true);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(
        m_highlightRect.adjusted(1, 1, -1, -1),
        step.highlightRadius, step.highlightRadius
    );

    // ── 3. 绘制提示气泡到高亮区域的引导线 ──
    if (step.tooltipPosition != TourStep::Center)
    {
        QPoint highlightEdge;
        switch (step.tooltipPosition)
        {
        case TourStep::Top:    highlightEdge = QPoint(m_highlightRect.center().x(), m_highlightRect.top()); break;
        case TourStep::Bottom: highlightEdge = QPoint(m_highlightRect.center().x(), m_highlightRect.bottom()); break;
        case TourStep::Left:   highlightEdge = QPoint(m_highlightRect.left(), m_highlightRect.center().y()); break;
        case TourStep::Right:  highlightEdge = QPoint(m_highlightRect.right(), m_highlightRect.center().y()); break;
        default: break;
        }

        QPoint tooltipEdge;
        QRect tr = m_tooltipWidget->geometry();
        switch (step.tooltipPosition)
        {
        case TourStep::Top:    tooltipEdge = QPoint(tr.center().x(), tr.bottom()); break;
        case TourStep::Bottom: tooltipEdge = QPoint(tr.center().x(), tr.top()); break;
        case TourStep::Left:   tooltipEdge = QPoint(tr.right(), tr.center().y()); break;
        case TourStep::Right:  tooltipEdge = QPoint(tr.left(), tr.center().y()); break;
        default: break;
        }

        // 绘制连接虚线
        QPen dashPen(CutoutBorderColor, 1.5, Qt::DashLine);
        painter.setPen(dashPen);
        painter.drawLine(highlightEdge, tooltipEdge);

        // 绘制指示圆点
        painter.setPen(Qt::NoPen);
        painter.setBrush(CutoutBorderColor);
        painter.drawEllipse(highlightEdge, 4, 4);
        painter.drawEllipse(tooltipEdge, 4, 4);
    }
}

// ═══════════════════════════════════════════
//  GuidedTourManager 实现
// ═══════════════════════════════════════════

GuidedTourManager::GuidedTourManager(QWidget *ownerWindow)
    : QObject(ownerWindow)
    , m_ownerWindow(ownerWindow)
{
}

void GuidedTourManager::addStep(const TourStep &step)
{
    m_steps.append(step);
}

void GuidedTourManager::addStep(QWidget *target, const QString &title,
                                  const QString &desc, TourStep::Position pos)
{
    TourStep step;
    step.targetWidget = target;
    step.title = title;
    step.description = desc;
    step.tooltipPosition = pos;
    m_steps.append(step);
}

void GuidedTourManager::start()
{
    if (m_overlay)
    {
        m_overlay->deleteLater();
    }

    m_overlay = new HelpOverlay(m_ownerWindow);
    m_overlay->setSteps(m_steps);
    m_overlay->startTour();
}

void GuidedTourManager::stop()
{
    if (m_overlay)
    {
        m_overlay->stopTour();
        m_overlay->deleteLater();
        m_overlay = nullptr;
    }
}
