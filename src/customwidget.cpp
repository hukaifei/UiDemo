#include "customwidget.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ============================================================
// 构造 / 析构
// ============================================================

CustomWidget::CustomWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();

    // 默认包含 5 个增益（增益1 ~ 增益5）
    for (int i = 1; i <= 5; ++i) {
        addGainItem(QStringLiteral("增益%1").arg(i), 50);
    }
}

CustomWidget::~CustomWidget() = default;

// ============================================================
// 界面构建
// ============================================================

void CustomWidget::setupUi()
{
    setWindowTitle(QStringLiteral("增益与补光灯亮度调节"));

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(0);

    // ---- 表头 ----
    QWidget *header = createHeader();
    mainLayout->addWidget(header);

    // ---- 列表 ----
    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("gainListWidget");
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainLayout->addWidget(m_listWidget);

    // 估算合适的初始大小（三列宽 + 间距 + 边距 + 滚动条）
    const int contentWidth =
        m_colWidthLabel + m_colWidthSpinBox + m_colWidthButton
        + 8 * 4    // 行内间距
        + 24;      // 边距与滚动条余量
    resize(contentWidth + 24, 360);
}

QWidget *CustomWidget::createHeader()
{
    auto *header = new QWidget(this);
    header->setObjectName("HeaderWidget");
    header->setFixedHeight(38);

    auto *layout = new QHBoxLayout(header);
    // 与每行的内边距保持一致，保证三列视觉对齐
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(8);

    auto *labelGain = new QLabel(QStringLiteral("增益"), header);
    labelGain->setFixedWidth(m_colWidthLabel);
    labelGain->setAlignment(Qt::AlignCenter);

    auto *labelBrightness = new QLabel(QStringLiteral("补光灯亮度调节"), header);
    labelBrightness->setFixedWidth(m_colWidthSpinBox);
    labelBrightness->setAlignment(Qt::AlignCenter);

    auto *labelConfirm = new QLabel(QStringLiteral("确认"), header);
    labelConfirm->setFixedWidth(m_colWidthButton);
    labelConfirm->setAlignment(Qt::AlignCenter);

    layout->addWidget(labelGain);
    layout->addWidget(labelBrightness);
    layout->addWidget(labelConfirm);
    layout->addStretch();

    return header;
}

QWidget *CustomWidget::createRowWidget(const QString &gainName,
                                       int initialBrightness,
                                       RowWidgets &out)
{
    auto *row = new QWidget;

    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);

    // 列 1：QLabel —— 显示增益名称
    auto *label = new QLabel(gainName, row);
    label->setFixedWidth(m_colWidthLabel);
    label->setAlignment(Qt::AlignCenter);
    label->setProperty("class", "gainLabel");

    // 列 2：QSpinBox —— 调节补光灯亮度
    auto *spin = new QSpinBox(row);
    spin->setRange(m_minBrightness, m_maxBrightness);
    spin->setValue(initialBrightness);
    spin->setSuffix(QStringLiteral(" %"));
    spin->setFixedWidth(m_colWidthSpinBox);
    spin->setAlignment(Qt::AlignCenter);
    spin->setProperty("class", "brightnessSpinBox");

    // 列 3：QPushButton —— 确认
    auto *btn = new QPushButton(QStringLiteral("确认"), row);
    btn->setFixedWidth(m_colWidthButton);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("class", "confirmButton");

    layout->addWidget(label);
    layout->addWidget(spin);
    layout->addWidget(btn);
    layout->addStretch();

    out.label   = label;
    out.spinBox = spin;
    out.button  = btn;

    return row;
}

// ============================================================
// 公共接口实现
// ============================================================

void CustomWidget::addGainItem(const QString &gainName, int initialBrightness)
{
    RowWidgets w;
    QWidget *rowWidget = createRowWidget(gainName, initialBrightness, w);

    auto *item = new QListWidgetItem(m_listWidget);
    item->setSizeHint(QSize(rowWidget->sizeHint().width(), 44));
    m_listWidget->addItem(item);
    m_listWidget->setItemWidget(item, rowWidget);

    const int rowIndex = m_rows.size();
    m_rows.append(w);

    // 点击"确认"按钮 -> 发出 confirmed 信号
    connect(w.button, &QPushButton::clicked, this, [this, rowIndex]() {
        if (rowIndex < 0 || rowIndex >= m_rows.size()) return;
        const auto &r = m_rows[rowIndex];
        emit confirmed(rowIndex, r.label->text(), r.spinBox->value());
    });

    // SpinBox 值变化 -> 发出 brightnessChanged 信号（实时）
    connect(w.spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this, rowIndex](int value) {
        emit brightnessChanged(rowIndex, value);
    });
}

void CustomWidget::clearItems()
{
    m_listWidget->clear();
    m_rows.clear();
}

int CustomWidget::brightness(int row) const
{
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows[row].spinBox->value();
}

void CustomWidget::setBrightness(int row, int value)
{
    if (row < 0 || row >= m_rows.size()) return;
    m_rows[row].spinBox->setValue(value);
}

int CustomWidget::count() const
{
    return m_rows.size();
}

void CustomWidget::setBrightnessRange(int minVal, int maxVal)
{
    if (minVal > maxVal) std::swap(minVal, maxVal);
    m_minBrightness = minVal;
    m_maxBrightness = maxVal;
    for (auto &r : m_rows) {
        if (r.spinBox) r.spinBox->setRange(minVal, maxVal);
    }
}
