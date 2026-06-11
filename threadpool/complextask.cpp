#include "complextask.h"
#include <QThread>
#include <QDebug>

ComplexTask::ComplexTask(int taskId, int workload)
    : m_taskId(taskId), m_workload(workload)
{
    setAutoDelete(true);
}

void ComplexTask::run()
{
    emit taskStarted(m_taskId);
    
    int result = 0;
    for (int i = 0; i < m_workload; ++i) {
        QThread::msleep(100);
        result += i * i;
        int progress = (i + 1) * 100 / m_workload;
        emit taskProgress(m_taskId, progress);
    }
    
    emit taskCompleted(m_taskId, QString("Task %1 result: %2").arg(m_taskId).arg(result));
}
