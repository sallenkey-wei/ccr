#include "daemon.h"
#include <QStringList>
#include <QProcess>

Daemon::Daemon(QStringList & proPaths, QObject *parent) : QObject(parent)
{
    for(auto iterator = proPaths.begin(); iterator != proPaths.end(); iterator++)
    {
        QProcess * temp = new QProcess(this);
        proList.push_back(temp);
        proMap.insert(temp, *iterator);
        connect(temp, static_cast<int, QProcess::ExitStatus>(&QProcess::finished), this, &Daemon::proExitHandler);
    }
    preTime = new time_t[proList.count()];
    crashTimes = new int[proList.count()];
    for(int i = 0; i < proList.count(); i++)
    {
        preTime[i] = time(NULL);
        crashTimes[i] = 0;
    }
}

void Daemon::proExitHandler(/*int exitCode, QProcess::ExitStatus exitStatus*/)
{
    QProcess * senderPro = static_cast<QProcess *>(sender());
    int index = proList.lastIndexOf(senderPro);
    if(time(NULL) - preTime[index] <= 1)
    {
        crashTimes[index]++;
    }
    else
    {
        crashTimes[index] = 0;
    }
    if(crashTimes[index] > 10)
    {
        qCritical() << proMap[senderPro] + " restart failed!";
        emit restartFailed(proMap[senderPro]);
    }
    else
    {
        senderPro->start("python", QStringList() << proMap[senderPro]);
        bool rstSuccess = senderPro->waitForStarted(30000);
        {

        }
    }
}
