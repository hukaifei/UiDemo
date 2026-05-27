#include "RangeSlider.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QtMath>
#include <QEasingCurve>

RangeSlider::RangeSlider(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 初始化动画
    m_animation = new QPropertyAnimation(this, "displayValue", this);
    m_animation->setDuration(m_animationDuration);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

void RangeSlider::setMinimum(double min)
{
    if (qFuzzyCompare(m_minimum, min))
        return;
    m_minimum = min;
    if (m_value < m_minimum)
        setValue(m_minimum);
    update();
}

void RangeSlider::setMaximum(double max)
{
    if (qFuzzyCompare(m_maximum, max))
        return;
    m_maximum = max;
    if (m_value > m_maximum)
        setValue(m_maximum);
    update();
}

void RangeSlider::setRange(double min, double max)
{
    m_minimum = min;
    m_maximum = max;
    m_value = qBound(m_minimum, m_value, m_maximum);
    m_displayValue = m_value;
    update();
}

void RangeSlider::setValue(double val)
{
    val = qBound(m_minimum, val, m_maximum);
    if (qFuzzyCompare(m_value, val))
        return;
    m_value = val;
    emit valueChanged(m_value);

    // 启动动画：从当前显示值平滑过渡到目标值
    m_animation->stop();
    m_animation->setStartValue(m_displayValue);
    m_animation->setEndValue(m_value);
    m_animation->start();
}

void RangeSlider::setDisplayValue(double val)
{
    if (qFuzzyCompare(m_displayValue, val))
        return;
    m_displayValue = val;
    update();
}

void RangeSlider::setValidMinimum(double min)
{
    m_validMin = min;
    update();
}

void RangeSlider::setValidMaximum(double max)
{
    m_validMax = max;
    update();
}

void RangeSlider::setValidRange(double min, double max)
{
    m_validMin = min;
    m_validMax = max;
    update();
}

void RangeSlider::setTickCount(int count)
{
    if (count < 2)
        count = 2;
    m_tickCount = count;
    update();
}

bool RangeSlider::isInValidRange() const
{
    return m_value >= m_validMin && m_value <= m_validMax;
}

QSize RangeSlider::sizeHint() const
{
    return QSize(400, 90);
}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(200, 70);
}

QRectF RangeSlider::trackRect() const
{
    double x = kMarginLeft;
    double y = kMarginTop;
    double w = width() - kMarginLeft - kMarginRight;
    double h = kTrackHeight;
    return QRectF(x, y, w, h);
}

double RangeSlider::valueToX(double val) const
{
    QRectF track = trackRect();
    if (qFuzzyCompare(m_maximum, m_minimum))
        return track.left();
    double ratio = (val - m_minimum) / (m_maximum - m_minimum);
    return track.left() + ratio * track.width();
}

void RangeSlider::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawTrackBackground(painter);
    drawValidRangeMarkers(painter);
    drawProgress(painter);
    drawTicks(painter);
    drawIndicator(painter);
    drawValueLabel(painter);
}

void RangeSlider::drawTrackBackground(QPainter &painter)
{
    QRectF track = trackRect();
    double radius = kTrackHeight / 2.0;

    // 绘制轨道背景，带边框
    painter.setPen(QPen(QColor(160, 160, 160), 1.5));
    painter.setBrush(m_trackColor);
    painter.drawRoundedRect(track, radius, radius);
}

void RangeSlider::drawValidRangeMarkers(QPainter &painter)
{
    QRectF track = trackRect();
    double x1 = valueToX(m_validMin);
    double x2 = valueToX(m_validMax);
    double radius = kTrackHeight / 2.0;

    QRectF validRect(x1, track.top(), x2 - x1, track.height());
    painter.setPen(QPen(m_validRangeBorderColor, 1.5));
    painter.setBrush(m_validRangeColor);
    painter.drawRoundedRect(validRect, radius, radius);
}

void RangeSlider::drawProgress(QPainter &painter)
{
    Q_UNUSED(painter);
}

void RangeSlider::drawTicks(QPainter &painter)
{
    QRectF track = trackRect();
    QFont font = this->font();
    font.setPixelSize(10);
    painter.setFont(font);
    QFontMetrics fm(font);

    for (int i = 0; i < m_tickCount; ++i) {
        double ratio = static_cast<double>(i) / (m_tickCount - 1);
        double x = track.left() + ratio * track.width();
        double tickTop = track.bottom() + 4;
        double tickBottom = tickTop + kTickLength;

        // 绘制刻度线
        painter.setPen(QPen(m_tickColor, 1));
        painter.drawLine(QPointF(x, tickTop), QPointF(x, tickBottom));

        // 绘制刻度值
        double val = m_minimum + ratio * (m_maximum - m_minimum);
        QString text = QString::number(val, 'f', 0);
        int textWidth = fm.horizontalAdvance(text);
        painter.setPen(m_textColor);
        painter.drawText(QPointF(x - textWidth / 2.0, tickBottom + fm.height()),
                         text);
    }
}

void RangeSlider::drawIndicator(QPainter &painter)
{
    double x = valueToX(m_displayValue);
    QRectF track = trackRect();

    bool displayInValid = m_displayValue >= m_validMin && m_displayValue <= m_validMax;

    double indicatorW = kIndicatorWidth;
    QRectF indicatorRect(x - indicatorW / 2.0, track.top(), indicatorW, track.height());

    if (displayInValid) {
        painter.setPen(QPen(m_indicatorBorderColor, 1.5));
        painter.setBrush(m_indicatorColor);
    } else {
        painter.setPen(QPen(m_indicatorBorderColorInvalid, 1.5));
        painter.setBrush(m_indicatorColorInvalid);
    }
    painter.drawRoundedRect(indicatorRect, indicatorW / 2.0, indicatorW / 2.0);
}

void RangeSlider::drawValueLabel(QPainter &painter)
{
    double x = valueToX(m_displayValue);
    QRectF track = trackRect();

    QFont font = this->font();
    font.setPixelSize(11);
    font.setBold(true);
    painter.setFont(font);
    QFontMetrics fm(font);

    QString text = QString::number(m_displayValue, 'f', 1);
    int textWidth = fm.horizontalAdvance(text);

    // 根据显示值判断是否在有效区间内
    bool displayInValid = m_displayValue >= m_validMin && m_displayValue <= m_validMax;

    // 在指示器上方显示当前值
    QColor bgColor = displayInValid ? m_progressColorValid : QColor(120, 120, 120);
    double labelW = textWidth + 10;
    double labelH = fm.height() + 6;
    double labelX = x - labelW / 2.0;
    double labelY = track.top() - kIndicatorExtraHeight - 10 - labelH;

    // 确保标签不超出控件左右边界
    if (labelX < 2)
        labelX = 2;
    if (labelX + labelW > width() - 2)
        labelX = width() - 2 - labelW;

    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(QRectF(labelX, labelY, labelW, labelH), 3, 3);

    // 绘制文字
    painter.setPen(Qt::white);
    painter.drawText(QRectF(labelX, labelY, labelW, labelH),
                     Qt::AlignCenter, text);
}
