#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QMap>
#include <time.h>

class QStringList;
class QProcess;

class Daemon : public QObject
{
    Q_OBJECT
public:
    explicit Daemon(QStringList &, QObject *parent = 0);

signals:
    void restartFailed(QString&);

public slots:

private:
    QMap<QProcess *, QString, int *> proMap;
    QList<QProcess *> proList;
    time_t * preTime;
    int * crashTimes;
    void proExitHandler(/*int exitCode, QProcess::ExitStatus exitStatus*/);
};

#endif // DAEMON_H
