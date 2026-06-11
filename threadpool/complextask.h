#ifndef COMPLEXTASK_H
#define COMPLEXTASK_H

#include <QRunnable>
#include <QObject>

class ComplexTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ComplexTask(int taskId, int workload);
    void run() override;

signals:
    void taskStarted(int taskId);
    void taskProgress(int taskId, int progress);
    void taskCompleted(int taskId, QString result);

private:
    int m_taskId;
    int m_workload;
};

#endif // COMPLEXTASK_H
