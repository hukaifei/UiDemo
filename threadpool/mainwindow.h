#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include <QLabel>
#include <QThreadPool>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartTask();
    void onTaskStarted(int taskId);
    void onTaskProgress(int taskId, int progress);
    void onTaskCompleted(int taskId, QString result);

private:
    QPushButton *m_startButton;
    QTextEdit *m_logText;
    QSpinBox *m_taskCountSpinBox;
    QSpinBox *m_workloadSpinBox;
    QLabel *m_poolInfoLabel;
    QThreadPool *m_threadPool;
    int m_nextTaskId;
};

#endif // MAINWINDOW_H
