#include "customTableWidget.h"

GainLightTable::GainLightTable(QWidget *parent)
    : QTableWidget(parent)
{
    initTable();
}

void GainLightTable::initTable()
{
    // 设置 4 行 3 列
    setRowCount(4);
    setColumnCount(3);

    // 设置表头
    QStringList headers;
    headers << "增益" << "补光灯亮度" << "设置";
    setHorizontalHeaderLabels(headers);

    // 设置表格不可编辑
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置选择行为
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // 隐藏垂直表头
    verticalHeader()->setVisible(false);

    // 表头自适应
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置行高
    for (int row = 0; row < rowCount(); ++row)
    {
        setRowHeight(row, 45);
    }

    // 添加每一行的控件
    for (int row = 0; row < rowCount(); ++row)
    {
        // 第 1 列：QLabel，显示增益
        QLabel *gainLabel = new QLabel(this);
        gainLabel->setText(QString("增益 %1").arg(row + 1));
        gainLabel->setAlignment(Qt::AlignCenter);

        // 第 2 列：QSpinBox，设置补光灯亮度
        QSpinBox *lightSpinBox = new QSpinBox(this);
        lightSpinBox->setRange(0, 100);
        lightSpinBox->setValue(50);
        lightSpinBox->setSuffix(" %");
        lightSpinBox->setAlignment(Qt::AlignCenter);

        // 第 3 列：QPushButton，设置按钮
        QPushButton *setButton = new QPushButton("设置", this);

        // 放入表格
        setCellWidget(row, 0, gainLabel);
        setCellWidget(row, 1, lightSpinBox);
        setCellWidget(row, 2, setButton);

        // 按钮点击信号
        connect(setButton, &QPushButton::clicked, this, [=]() {
            QString gain = gainLabel->text();
            int lightValue = lightSpinBox->value();

            emit setButtonClicked(row, gain, lightValue);
        });
    }
}

void GainLightTable::setGainValue(int row, const QString &value)
{
    if (row < 0 || row >= rowCount())
        return;

    QLabel *label = qobject_cast<QLabel *>(cellWidget(row, 0));
    if (label)
    {
        label->setText(value);
    }
}

int GainLightTable::lightValue(int row) const
{
    if (row < 0 || row >= rowCount())
        return -1;

    QSpinBox *spinBox = qobject_cast<QSpinBox *>(cellWidget(row, 1));
    if (spinBox)
    {
        return spinBox->value();
    }

    return -1;
}
