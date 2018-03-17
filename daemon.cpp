#include "daemon.h"
#include <QStringList>
#include <QProcess>

Daemon::Daemon(const QStringList & proPaths, QObject *parent) : QObject(parent)
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
Daemon::~Daemon()
{
    delete[] preTime;
    delete[] crashTimes;
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
        if(!rstSuccess)
        {
            /*emit noPythonEnv();*/
        }
    }
}

void Daemon::startAllPro()
{
    for(auto iterator = proList.begin(); iterator != proList.end(); iterator++)
    {
        QProcess * temp = *iterator;
        temp->start("python", QStringList() << proMap[temp]);
        bool rstSuccess = temp->waitForStarted(30000);
        if(!rstSuccess)
        {
            emit noPythonEnv();
        }
    }
}

void Daemon::closeAllPro()
{
    for(auto iterator = proList.begin(); iterator != proList.end(); iterator++)
    {
        QProcess * temp = *iterator;
        temp->close();
        bool closeSuccess = temp->waitForFinished(30000);
        if(!closeSuccess)
        {
            emit closeFailed(proMap[temp]);
        }
    }
}

bool Daemon::setProPaths(QStringList & proPaths)
{
    closeAllPro();
    for(auto iterator = proMap.begin(); iterator != proMap.end(); iterator++)
    {
        delete iterator.key();
    }
    proMap.clear();
    proList.clear();
    delete[] preTime;
    delete[] crashTimes;
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
