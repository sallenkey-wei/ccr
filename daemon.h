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
    explicit Daemon(const QStringList &, QObject *parent = 0);
    ~Daemon();

signals:
    void restartFailed(QString&);
    void noPythonEnv();
    void closeFailed(QString&);

public slots:
    void startAllPro();
    void closeAllPro();
    QStringList proPaths();
    bool setProPaths(const QStringList &);

private:
    QMap<QProcess *, QString, int *> proMap;
    QList<QProcess *> proList;
    time_t * preTime;
    int * crashTimes;
    void proExitHandler(/*int exitCode, QProcess::ExitStatus exitStatus*/);
};

#endif // DAEMON_H
