#ifndef HELPOVERLAY_H
#define HELPOVERLAY_H

#include <QWidget>
#include <QPainterPath>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QPropertyAnimation>

class QPushButton;
class QLabel;

// ─── 引导步骤数据结构 ───
struct TourStep
{
    // 要高亮的控件（为 nullptr 时高亮整个窗口中央区域）
    QWidget *targetWidget = nullptr;
    // 提示标题
    QString title;
    // 提示描述
    QString description;
    // 提示气泡相对于高亮区域的位置
    enum Position { Top, Bottom, Left, Right, Center };
    Position tooltipPosition = Bottom;
    // 高亮区域四边的扩展像素（让高亮框比控件大一圈）
    int highlightPadding = 8;
    // 高亮区域圆角半径
    int highlightRadius = 8;
    // 进入此步骤前回调
    std::function<void()> onEnter;
    // 离开此步骤前回调
    std::function<void()> onLeave;
};

// ─── 帮助引导蒙版覆盖层 ───
class HelpOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit HelpOverlay(QWidget *parent = nullptr);
    ~HelpOverlay() override = default;

    // 设置引导步骤列表
    void setSteps(const QVector<TourStep> &steps);
    // 启动引导
    void startTour();
    // 停止引导
    void stopTour();

    // 获取当前步骤索引
    int currentStepIndex() const { return m_currentIndex; }
    // 获取总步骤数
    int totalSteps() const { return m_steps.size(); }

signals:
    void tourStarted();
    void tourFinished();
    void stepChanged(int current, int total);
    void skipped();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void showStep(int index);
    void updateOverlayGeometry();
    QRect getHighlightRect() const;
    void positionTooltip(const QRect &highlightRect);
    void updateButtonsVisibility();
    void animateTransition();

    // 绘制
    void paintOverlay(QPainter &painter, const QRect &cutout);

    // 步骤数据
    QVector<TourStep> m_steps;
    int m_currentIndex = -1;

    // UI 元素
    QWidget *m_tooltipWidget;
    QLabel *m_titleLabel;
    QLabel *m_descLabel;
    QLabel *m_stepIndicator;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    QPushButton *m_skipBtn;

    // 高亮区域
    QRect m_highlightRect;
    // 动画
    QPropertyAnimation *m_opacityAnimation = nullptr;
    bool m_animating = false;
};

// ─── 引导管理器（便捷入口）─
class GuidedTourManager : public QObject
{
    Q_OBJECT

public:
    explicit GuidedTourManager(QWidget *ownerWindow);

    // 添加步骤
    void addStep(const TourStep &step);
    // 快速添加步骤（目标控件 + 描述）
    void addStep(QWidget *target, const QString &title, const QString &desc,
                 TourStep::Position pos = TourStep::Bottom);
    // 启动引导
    void start();
    // 停止引导
    void stop();

private:
    QWidget *m_ownerWindow;
    HelpOverlay *m_overlay = nullptr;
    QVector<TourStep> m_steps;
};

#endif // HELPOVERLAY_H
