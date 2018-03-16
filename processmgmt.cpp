#include "processmgmt.h"
#include "mymessagebox.h"
#include <QApplication>
#include <QDebug>
#include <iostream>

static QString proAlexNetPath = "E:/Python\ Demo/untitled/day1/var.py";
static QString proCRNNPath = "E:/Python\ Demo/untitled/day1/var.py";
static QString proCTPNPath = "E:/Python\ Demo/untitled/day1/var.py";
static QString proDeepLabPath = "E:/Python\ Demo/untitled/day1/var.py";
static QString proEASTPath = "E:/Python\ Demo/untitled/day1/var.py";

ProcessMgmt::ProcessMgmt(QObject *parent) : QObject(parent), currentTime(time(NULL))
{
    this->proAlexNet = new QProcess(this);
    this->proCRNN = new QProcess(this);
    this->proCTPN = new QProcess(this);
    this->proDeepLab = new QProcess(this);
    this->proEAST = new QProcess(this);
#if 0
    connect(proAlexNet, &QProcess::stateChanged, this, &ProcessMgmt::restartProcess);
    connect(proCRNN, &QProcess::stateChanged, this, &ProcessMgmt::restartProcess);
    connect(proCTPN, &QProcess::stateChanged, this, &ProcessMgmt::restartProcess);
    connect(proDeepLab, &QProcess::stateChanged, this, &ProcessMgmt::restartProcess);
    connect(proEAST, &QProcess::stateChanged, this, &ProcessMgmt::restartProcess);

    connect(proAlexNet, &QProcess::errorOccurred, this, &ProcessMgmt::errorHandler);
#endif

#if 1
    connect(proAlexNet, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
#endif
}

ProcessMgmt::~ProcessMgmt()
{

}

#if 1//为什么不行呢
void ProcessMgmt::restartProcess(QProcess::ProcessState newState)
{
std::cout << "in restart Process" << newState << std::endl;
    qDebug() << newState;
    if(newState == QProcess::NotRunning)
    {
        if(proAlexNet==sender())
        {
            proAlexNet->start("python", QStringList() << proAlexNetPath);
        }
        else if(proCRNN == sender())
        {
        }
        else if(proCTPN == sender())
        {
        }
        else if(proDeepLab == sender())
        {
        }
        else if(proEAST == sender())
        {
        }
    }
}
#endif

void ProcessMgmt::finishHandler(int exitCode, QProcess::ExitStatus exitStatus)
{
//std::cout << "exit code: " << exitCode << "exitStatus: "  << exitStatus << std::endl;
    if(time(NULL)-currentTime <= 1)
    {
       crashTimes++;
    }
    else
    {
        crashTimes = 0;
    }
    if(crashTimes > 10)
    {
        qCritical() << proAlexNetPath + " start failed!";
        myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                     proAlexNetPath.append(QStringLiteral(" 启动失败!")), QStringLiteral("请检查该服务路径是否正确并重启程序"));
        qApp->quit();
    }

    if(proAlexNet==sender())
    {
        proAlexNet->start("python", QStringList() << proAlexNetPath);
    }
    else if(proCRNN == sender())
    {
    }
    else if(proCTPN == sender())
    {
    }
    else if(proDeepLab == sender())
    {
    }
    else if(proEAST == sender())
    {
    }

}

void ProcessMgmt::startProcess()
{
    this->proAlexNet->start("python", QStringList() << proAlexNetPath);
    bool startSuccess;
    startSuccess = proAlexNet->waitForStarted(30000);
    if(!startSuccess)
    {
        qCritical() << proAlexNetPath + " start failed!";
        myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                     proAlexNetPath + QStringLiteral(" 启动失败!"), QStringLiteral("请检查该服务路径是否正确并重启程序和是否安装python环境！"));
    }

}

void ProcessMgmt::errorHandler(QProcess::ProcessError error)
{
    //std::cout << error << static_cast<QProcess *>(sender())->errorString().toStdString() <<std::endl;
std::cout << "in errorHandle " << std::endl;
     myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                     proAlexNetPath + QStringLiteral(" 启动失败!"), QStringLiteral("请检查该服务路径是否正确并重启程序"));
}

void ProcessMgmt::closeAllProcess()
{

}
