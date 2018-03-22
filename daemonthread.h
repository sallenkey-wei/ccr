#ifndef DAEMONTHREAD_H
#define DAEMONTHREAD_H

#include <QThread>

class Daemon;

class DaemonThread : public QThread
{
    Q_OBJECT
public:
    explicit DaemonThread(QObject *parent = 0);
    ~DaemonThread();
    Daemon * daemon;

signals:

public slots:
};

#endif // DAEMONTHREAD_H
