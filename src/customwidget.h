#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QVector>

class QListWidget;
class QLabel;
class QSpinBox;
class QPushButton;

/**
 * @brief 自定义增益 / 补光灯亮度设置控件
 *
 * 使用 QListWidget 实现，每行从左到右包含三个子控件：
 *   - QLabel     : 显示增益名称（默认 5 项：增益1 ~ 增益5）
 *   - QSpinBox   : 调节该增益对应的补光灯亮度
 *   - QPushButton: 点击"确认"提交当前行的设定
 *
 * 顶部带有列标题（增益 / 补光灯亮度调节 / 确认），列宽与行控件保持一致。
 */
class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = nullptr);
    ~CustomWidget() override;

    // ------------------------------------------------------------
    // 公共接口
    // ------------------------------------------------------------

    /** 追加一个增益项 */
    void addGainItem(const QString &gainName, int initialBrightness = 50);

    /** 清空全部增益项 */
    void clearItems();

    /** 获取指定行的亮度值，行号无效时返回 -1 */
    int brightness(int row) const;

    /** 设置指定行的亮度值 */
    void setBrightness(int row, int value);

    /** 当前增益项的数量 */
    int count() const;

    /** 设置所有 SpinBox 的取值范围 */
    void setBrightnessRange(int minVal, int maxVal);

signals:
    /** 用户点击某行的"确认"按钮时发出 */
    void confirmed(int row, const QString &gainName, int brightness);

    /** 任一行的 SpinBox 数值发生变化时实时发出 */
    void brightnessChanged(int row, int value);

private:
    /** 单行子控件的句柄 */
    struct RowWidgets {
        QLabel      *label   = nullptr;
        QSpinBox    *spinBox = nullptr;
        QPushButton *button  = nullptr;
    };

    void     setupUi();
    QWidget *createHeader();
    QWidget *createRowWidget(const QString &gainName, int initialBrightness, RowWidgets &out);

    // ---- 成员变量 ----
    QListWidget        *m_listWidget = nullptr;
    QVector<RowWidgets> m_rows;

    // 三列宽度（表头与每一行都使用相同列宽，保证视觉对齐）
    int m_colWidthLabel   = 110;
    int m_colWidthSpinBox = 200;
    int m_colWidthButton  = 100;

    // 亮度取值范围
    int m_minBrightness = 0;
    int m_maxBrightness = 100;
};

#endif // CUSTOMWIDGET_H
