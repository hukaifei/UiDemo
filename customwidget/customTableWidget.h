#ifndef GAINLIGHTTABLE_H
#define GAINLIGHTTABLE_H

#include <QTableWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QHeaderView>

class GainLightTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit GainLightTable(QWidget *parent = nullptr);

    // 设置某一行的增益显示值
    void setGainValue(int row, const QString &value);

    // 获取某一行的补光灯亮度值
    int lightValue(int row) const;

signals:
    // 点击设置按钮时发出信号
    void setButtonClicked(int row, QString gain, int lightValue);

private:
    void initTable();
};

#endif // GAINLIGHTTABLE_H
