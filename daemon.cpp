#include "daemon.h"
#include <QStringList>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <iostream>
#include <QThread>

Daemon::Daemon(const QStringList & proPaths, QObject *parent) : QObject(parent)
{
    for(auto iterator = proPaths.begin(); iterator != proPaths.end(); iterator++)
    {
        QProcess * temp = new QProcess(this);
        proList.push_back(temp);
        proMap.insert(temp, *iterator);
        connect(temp, &QProcess::readyRead, this, &Daemon::output);
        /*设置启动py文件的运行路径，对于依赖路径的py文件很重要*/
        temp->setWorkingDirectory(QFileInfo(*iterator).path());
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
    closeAllPro();
    delete[] preTime;
    delete[] crashTimes;
}

void Daemon::proExitHandler(/*int exitCode, QProcess::ExitStatus exitStatus*/)
{
    QProcess * senderPro = static_cast<QProcess *>(sender());

    /*发出进程终止信号*/
    emit terminateRun(proMap[senderPro]);

    /*若对同一程序重启10次且相邻两次重启时间小于1秒则判断为该进程无法重启，抛出重启失败信号*/
    int index = proList.lastIndexOf(senderPro);
    if(time(NULL) - preTime[index] <= 1)
    {
        crashTimes[index]++;
    }
    else
    {
        crashTimes[index] = 0;
    }
    preTime[index] = time(NULL);
    if(crashTimes[index] > 10/*0*/)
    {
        qCritical() << proMap[senderPro] + " restart failed!";
        disconnect(senderPro, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Daemon::proExitHandler);
        emit restartFailed(proMap[senderPro]);
    }
    else
    {
        senderPro->start("python", QStringList() << "-u" << proMap[senderPro]);
        bool rstSuccess = senderPro->waitForStarted(30000);
        if(!rstSuccess)
        {
            /*emit noPythonEnv();*/
        }
        else
        {
            /*重启进程成功发出进程开始运行信号*/
            emit startRun(proMap[senderPro]);
        }
    }
}

void Daemon::startAllPro()
{
    if(proStatus() == true)
        return;
    for(auto iterator = proList.begin(); iterator != proList.end(); iterator++)
    {
        QProcess * temp = *iterator;
        connect(temp, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Daemon::proExitHandler);
        temp->start("python", QStringList() << "-u" << proMap[temp]);
        bool rstSuccess = temp->waitForStarted(30000);
        if(!rstSuccess)
        {
            emit noPythonEnv();
            disconnect(temp, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Daemon::proExitHandler);
        }
        else
        {
            emit startRun(proMap[temp]);
        }
    }
    setProStatus(true);
}

void Daemon::closeAllPro()
{
    if(proStatus() == false)
        return;
    for(auto iterator = proList.rbegin(); iterator != proList.rend(); iterator++)
    {
        QProcess * temp = *iterator;
        disconnect(temp, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Daemon::proExitHandler);
        temp->close();
        bool closeSuccess = temp->waitForFinished(30000);
        //这里永远返回false，原因待查，所以terminateRun信号在if中也发送，因为else永远不成立
        if(!closeSuccess)
        {
            emit closeFailed(proMap[temp]);
            emit terminateRun(proMap[temp]);
        }
        else
        {
            emit terminateRun(proMap[temp]);
        }
    }
    setProStatus(false);
}

void Daemon::setProPaths(const QStringList proPaths)
{
    for(auto iterator = proList.rbegin(); iterator != proList.rend(); iterator++)
    {
        QProcess * temp = *iterator;
        disconnect(temp, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Daemon::proExitHandler);
        temp->close();
        bool closeSuccess = temp->waitForFinished(30000);
        if(!closeSuccess)
        {
            emit closeFailed(proMap[temp]);
        }
        delete temp;
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
        connect(temp, &QProcess::readyRead, this, &Daemon::output);
        temp->setWorkingDirectory(QFileInfo(*iterator).path());
    }
    preTime = new time_t[proList.count()];
    crashTimes = new int[proList.count()];
    for(int i = 0; i < proList.count(); i++)
    {
        preTime[i] = time(NULL);
        crashTimes[i] = 0;
    }
}

void Daemon::output()
{
    QProcess * proSender = static_cast<QProcess *>(sender());
    emit readyRead(proMap[proSender], QString(proSender->readAll()));
}
