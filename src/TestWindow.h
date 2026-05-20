#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>

class CustomWidget;

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);

private slots:
    void onConfirmed(int row, const QString &gainName, int brightness);
    void onBrightnessChanged(int row, int value);

private:
    CustomWidget *m_widget = nullptr;
};

#endif // TESTWINDOW_H
