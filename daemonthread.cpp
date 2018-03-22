#include "daemonthread.h"
#include <QThread>
#include "daemon.h"

DaemonThread::DaemonThread(QObject *parent) : QThread(parent)
{
    daemon = new Daemon(QStringList());
    daemon->moveToThread(this);
    //daemon->setProPaths(list);
    this->start();
}

DaemonThread::~DaemonThread()
{
    daemon->deleteLater();
    this->quit();
    this->wait();
}
