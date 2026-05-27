#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QtMath>
#include "RangeSlider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(QString::fromUtf8("RangeSlider - 外部数据驱动的只读滑条控件"));
    window.resize(650, 350);

    QWidget *central = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ============ 滑条控件 ============
    QGroupBox *sliderGroup = new QGroupBox(QString::fromUtf8("实时数据监控（外部数据驱动，不可拖动）"));
    QVBoxLayout *sliderLayout = new QVBoxLayout(sliderGroup);

    RangeSlider *slider = new RangeSlider;
    slider->setRange(-30, 30);
    slider->setValidRange(-10, 10);
    slider->setValue(0);
    slider->setTickCount(11);
    sliderLayout->addWidget(slider);

    mainLayout->addWidget(sliderGroup);

    // ============ 状态显示 ============
    QGroupBox *statusGroup = new QGroupBox(QString::fromUtf8("状态信息"));
    QHBoxLayout *statusLayout = new QHBoxLayout(statusGroup);

    QLabel *valueLabel = new QLabel(QString::fromUtf8("当前值: 0.0"));
    QLabel *rangeLabel = new QLabel(QString::fromUtf8("有效区间: [-10, 10]"));
    QLabel *statusLabel = new QLabel(QString::fromUtf8("状态: ---"));

    statusLayout->addWidget(valueLabel);
    statusLayout->addWidget(rangeLabel);
    statusLayout->addWidget(statusLabel);

    mainLayout->addWidget(statusGroup);

    // ============ 模拟控制 ============
    QGroupBox *ctrlGroup = new QGroupBox(QString::fromUtf8("模拟外部数据源"));
    QHBoxLayout *ctrlLayout = new QHBoxLayout(ctrlGroup);

    QPushButton *startBtn = new QPushButton(QString::fromUtf8("启动模拟"));
    QPushButton *stopBtn = new QPushButton(QString::fromUtf8("停止模拟"));
    stopBtn->setEnabled(false);

    QLabel *speedLabel = new QLabel(QString::fromUtf8("更新间隔(ms):"));
    QSpinBox *speedSpin = new QSpinBox;
    speedSpin->setRange(20, 1000);
    speedSpin->setValue(50);
    speedSpin->setSingleStep(10);

    ctrlLayout->addWidget(startBtn);
    ctrlLayout->addWidget(stopBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(speedLabel);
    ctrlLayout->addWidget(speedSpin);

    mainLayout->addWidget(ctrlGroup);

    // ============ 参数调整 ============
    QGroupBox *paramGroup = new QGroupBox(QString::fromUtf8("有效区间设置"));
    QHBoxLayout *paramLayout = new QHBoxLayout(paramGroup);

    paramLayout->addWidget(new QLabel(QString::fromUtf8("最小值:")));
    QDoubleSpinBox *validMinSpin = new QDoubleSpinBox;
    validMinSpin->setRange(-30, 30);
    validMinSpin->setValue(-10);
    paramLayout->addWidget(validMinSpin);

    paramLayout->addWidget(new QLabel(QString::fromUtf8("最大值:")));
    QDoubleSpinBox *validMaxSpin = new QDoubleSpinBox;
    validMaxSpin->setRange(-30, 30);
    validMaxSpin->setValue(10);
    paramLayout->addWidget(validMaxSpin);

    paramLayout->addStretch();

    mainLayout->addWidget(paramGroup);
    mainLayout->addStretch();

    // ============ 定时器模拟外部数据 ============
    QTimer *timer = new QTimer(&window);
    double phase = 0.0;

    QObject::connect(timer, &QTimer::timeout, [&]() {
        // 模拟正弦波数据，在 -30~30 之间变化
        phase += 0.05;
        double val = 30.0 * qSin(phase);
        slider->setValue(val);
    });

    // ============ 信号连接 ============
    QObject::connect(slider, &RangeSlider::valueChanged, [=](double val) {
        valueLabel->setText(QString::fromUtf8("当前值: %1").arg(val, 0, 'f', 1));
        if (slider->isInValidRange()) {
            statusLabel->setText(QString::fromUtf8("状态: 在有效区间内 ✓"));
            statusLabel->setStyleSheet("color: green; font-weight: bold;");
        } else {
            statusLabel->setText(QString::fromUtf8("状态: 不在有效区间 ✗"));
            statusLabel->setStyleSheet("color: red; font-weight: bold;");
        }
    });

    QObject::connect(startBtn, &QPushButton::clicked, [=, &phase]() {
        phase = 0.0;
        timer->start(speedSpin->value());
        startBtn->setEnabled(false);
        stopBtn->setEnabled(true);
    });

    QObject::connect(stopBtn, &QPushButton::clicked, [=]() {
        timer->stop();
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
    });

    QObject::connect(speedSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                     [=](int val) {
        if (timer->isActive()) {
            timer->setInterval(val);
        }
    });

    QObject::connect(validMinSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=](double val) {
        slider->setValidMinimum(val);
        rangeLabel->setText(QString::fromUtf8("有效区间: [%1, %2]")
                           .arg(val, 0, 'f', 0)
                           .arg(slider->validMaximum(), 0, 'f', 0));
    });

    QObject::connect(validMaxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=](double val) {
        slider->setValidMaximum(val);
        rangeLabel->setText(QString::fromUtf8("有效区间: [%1, %2]")
                           .arg(slider->validMinimum(), 0, 'f', 0)
                           .arg(val, 0, 'f', 0));
    });

    window.setCentralWidget(central);
    window.show();

    return app.exec();
}
