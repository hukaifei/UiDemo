#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QWidget>
#include <QColor>
#include <QPropertyAnimation>

/**
 * @brief 自定义滑条控件（只读，由外部数据驱动）
 * 
 * 功能:
 * - 滑条上显示刻度和范围值
 * - 支持设置有效区间 [validMin, validMax]
 * - 当位置值在有效区间内时，进度条显示绿色高亮背景
 * - 滑条默认背景颜色为灰色
 * - 不支持鼠标交互，值由外部实时更新
 */
class RangeSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double displayValue READ displayValue WRITE setDisplayValue)
    Q_PROPERTY(double validMinimum READ validMinimum WRITE setValidMinimum)
    Q_PROPERTY(double validMaximum READ validMaximum WRITE setValidMaximum)
    Q_PROPERTY(int tickCount READ tickCount WRITE setTickCount)

public:
    explicit RangeSlider(QWidget *parent = nullptr);
    ~RangeSlider() override = default;

    // 范围属性
    double minimum() const { return m_minimum; }
    void setMinimum(double min);

    double maximum() const { return m_maximum; }
    void setMaximum(double max);

    void setRange(double min, double max);

    // 当前值（由外部设置）
    double value() const { return m_value; }
    void setValue(double val);

    // 用于动画的显示值
    double displayValue() const { return m_displayValue; }
    void setDisplayValue(double val);

    // 有效区间
    double validMinimum() const { return m_validMin; }
    void setValidMinimum(double min);

    double validMaximum() const { return m_validMax; }
    void setValidMaximum(double max);

    void setValidRange(double min, double max);

    // 刻度数量
    int tickCount() const { return m_tickCount; }
    void setTickCount(int count);

    // 是否在有效区间内
    bool isInValidRange() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void valueChanged(double value);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // 值与像素坐标互转
    double valueToX(double val) const;

    // 绘制各部分
    void drawTrackBackground(QPainter &painter);
    void drawValidRangeMarkers(QPainter &painter);
    void drawProgress(QPainter &painter);
    void drawTicks(QPainter &painter);
    void drawIndicator(QPainter &painter);
    void drawValueLabel(QPainter &painter);

    // 获取滑轨区域
    QRectF trackRect() const;

    double m_minimum = 0.0;
    double m_maximum = 100.0;
    double m_value = 50.0;
    double m_displayValue = 50.0;   // 动画用的显示值
    double m_validMin = 20.0;
    double m_validMax = 80.0;
    int m_tickCount = 11;       // 刻度数量（包含首尾）

    // 动画
    QPropertyAnimation *m_animation = nullptr;
    int m_animationDuration = 200;  // 动画时长（毫秒）

    // 外观参数
    static constexpr int kTrackHeight = 30;
    static constexpr int kIndicatorWidth = 16;
    static constexpr int kIndicatorExtraHeight = 6;
    static constexpr int kMarginLeft = 35;
    static constexpr int kMarginRight = 35;
    static constexpr int kMarginTop = 35;
    static constexpr int kMarginBottom = 35;
    static constexpr int kTickLength = 8;

    QColor m_trackColor{200, 200, 200};        // 灰色背景
    QColor m_progressColorValid{76, 175, 80};  // 绿色（在有效区间内）
    QColor m_progressColorInvalid{200, 200, 200}; // 灰色（不在有效区间内）
    QColor m_validRangeColor{178, 233, 205};   // 有效区间标记（#B2E9CD）
    QColor m_validRangeBorderColor{34, 210, 121}; // 有效区间边框（#22D279）
    QColor m_indicatorColor{34, 210, 121, 128};   // 指示器颜色 rgba(34,210,121,0.5)
    QColor m_indicatorBorderColor{34, 210, 121};  // 指示器边框（#22D279）
    QColor m_indicatorColorInvalid{220, 50, 50, 128};      // 无效区间指示器（半透明红色）
    QColor m_indicatorBorderColorInvalid{220, 50, 50};     // 无效区间指示器边框（红色）
    QColor m_tickColor{100, 100, 100};         // 刻度颜色
    QColor m_textColor{60, 60, 60};            // 文字颜色
};

#endif // RANGESLIDER_H
