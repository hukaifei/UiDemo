#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QVector>

class QListWidget;
class QLabel;
class QSpinBox;
class QPushButton;

class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = nullptr);
    ~CustomWidget() override;

    void addGainItem(const QString &gainName, int initialBrightness = 50);
    void clearItems();
    int brightness(int row) const;
    void setBrightness(int row, int value);
    int count() const;
    void setBrightnessRange(int minVal, int maxVal);

signals:
    void confirmed(int row, const QString &gainName, int brightness);
    void brightnessChanged(int row, int value);

private:
    struct RowWidgets {
        QLabel      *label   = nullptr;
        QSpinBox    *spinBox = nullptr;
        QPushButton *button  = nullptr;
    };

    void     setupUi();
    QWidget *createHeader();
    QWidget *createRowWidget(const QString &gainName, int initialBrightness, RowWidgets &out);

    QListWidget        *m_listWidget = nullptr;
    QVector<RowWidgets> m_rows;

    int m_colWidthLabel   = 110;
    int m_colWidthSpinBox = 200;
    int m_colWidthButton  = 100;

    int m_minBrightness = 0;
    int m_maxBrightness = 100;
};

#endif // CUSTOMWIDGET_H
