#pragma once

#include <functional>
#include <exception>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QThread>
#include <QObject>

class Util
{
public:
    template<typename T>
    static bool  asyncProcessing(T *dialog,std::function<bool(T*)> f)
    {
        bool  bRes = true;
        QFutureWatcher<bool> watcher;
        QFuture<bool> future = QtConcurrent::run([=]()->bool{
            QThread::msleep(200);
            try
            {
                return f(dialog);
            }
            catch(const std::exception& e)
            {
                qInfo("asyncProcessing error:% %s",e.what());
                return false;
            }
        });
        QObject::connect(&watcher,&QFutureWatcher<void>::finished,dialog,[=]{
            Qthread::msleep(100);
            dialog->accept();
        })
        watcher.setFuture(future);
        try
        {
            dialog->exec();
            bRes = watcher.result();
        }
        return bRes;
    }
};



