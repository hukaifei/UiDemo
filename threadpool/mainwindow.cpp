#include "mainwindow.h"
#include "complextask.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_nextTaskId(1)
{
    m_threadPool = QThreadPool::globalInstance();
    
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(new QLabel("任务数量:"));
    m_taskCountSpinBox = new QSpinBox();
    m_taskCountSpinBox->setRange(1, 20);
    m_taskCountSpinBox->setValue(5);
    controlLayout->addWidget(m_taskCountSpinBox);
    
    controlLayout->addWidget(new QLabel("工作量:"));
    m_workloadSpinBox = new QSpinBox();
    m_workloadSpinBox->setRange(1, 100);
    m_workloadSpinBox->setValue(10);
    controlLayout->addWidget(m_workloadSpinBox);
    
    m_startButton = new QPushButton("启动任务");
    controlLayout->addWidget(m_startButton);
    controlLayout->addStretch();
    
    m_poolInfoLabel = new QLabel();
    m_poolInfoLabel->setText(QString("线程池: 最大线程数=%1, 活动线程数=%2")
                             .arg(m_threadPool->maxThreadCount())
                             .arg(m_threadPool->activeThreadCount()));
    
    m_logText = new QTextEdit();
    m_logText->setReadOnly(true);
    
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_poolInfoLabel);
    mainLayout->addWidget(m_logText);
    
    setCentralWidget(centralWidget);
    setWindowTitle("Qt线程池复杂任务Demo");
    resize(800, 600);
    
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartTask);
}

MainWindow::~MainWindow()
{
    m_threadPool->waitForDone();
}

void MainWindow::onStartTask()
{
    int taskCount = m_taskCountSpinBox->value();
    int workload = m_workloadSpinBox->value();
    
    m_logText->append(QString("开始提交 %1 个任务...").arg(taskCount));
    
    for (int i = 0; i < taskCount; ++i) {
        ComplexTask *task = new ComplexTask(m_nextTaskId++, workload);
        connect(task, &ComplexTask::taskStarted, this, &MainWindow::onTaskStarted);
        connect(task, &ComplexTask::taskProgress, this, &MainWindow::onTaskProgress);
        connect(task, &ComplexTask::taskCompleted, this, &MainWindow::onTaskCompleted);
        m_threadPool->start(task);
    }
    
    m_poolInfoLabel->setText(QString("线程池: 最大线程数=%1, 活动线程数=%2")
                             .arg(m_threadPool->maxThreadCount())
                             .arg(m_threadPool->activeThreadCount()));
}

void MainWindow::onTaskStarted(int taskId)
{
    m_logText->append(QString("任务 %1 已启动").arg(taskId));
}

void MainWindow::onTaskProgress(int taskId, int progress)
{
    m_logText->append(QString("任务 %1 进度: %2%").arg(taskId).arg(progress));
}

void MainWindow::onTaskCompleted(int taskId, QString result)
{
    m_logText->append(QString("✓ %1").arg(result));
    m_poolInfoLabel->setText(QString("线程池: 最大线程数=%1, 活动线程数=%2")
                             .arg(m_threadPool->maxThreadCount())
                             .arg(m_threadPool->activeThreadCount()));
}
