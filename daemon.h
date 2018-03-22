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
    void setProStatus(bool status) { isRunning = status;}
    bool proStatus() { return isRunning;}


signals:
    void restartFailed(QString);
    void noPythonEnv();
    void closeFailed(QString);
    void readyRead(QString, QString);
    void startRun(QString );
    void terminateRun(QString );

public slots:
    void startAllPro();
    void closeAllPro();
    void setProPaths(const QStringList );
private slots:
    void output();

private:
    QMap<QProcess *, QString> proMap;
    QList<QProcess *> proList;
    time_t * preTime;
    int * crashTimes;
    void proExitHandler(/*int exitCode, QProcess::ExitStatus exitStatus*/);
    bool isRunning = false;
};

#endif // DAEMON_H
