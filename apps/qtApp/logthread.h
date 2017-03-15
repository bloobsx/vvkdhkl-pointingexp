#ifndef LOGGERTHREAD_H
#define LOGGERTHREAD_H


class LoggerThread : public QThread
{
    Q_OBJECT
protected:
    void run();
};

void LoggerThread::run()
{

}

#endif // LOGGERTHREAD_H
