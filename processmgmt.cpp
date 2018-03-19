#include "processmgmt.h"
#include "mymessagebox.h"
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>


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

#if 0
    connect(proAlexNet, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proCRNN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proCTPN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proDeepLab, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proEAST, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
#endif
    connect(this->proAlexNet, &QProcess::readyReadStandardOutput, this, &ProcessMgmt::output);
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
            proCRNN->start("python", QStringList() << proAlexNetPath);
        }
        else if(proCTPN == sender())
        {
            proCTPN->start("python", QStringList() << proAlexNetPath);
        }
        else if(proDeepLab == sender())
        {
            proDeepLab->start("python", QStringList() << proAlexNetPath);
        }
        else if(proEAST == sender())
        {
            proEAST->start("python", QStringList() << proAlexNetPath);
        }
    }
}
#endif

void ProcessMgmt::finishHandler(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
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
    currentTime = time(NULL);
    if(crashTimes > 10)
    {
        emit proCrash(QString("local server crash"));
        disconnect(proAlexNet, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
        disconnect(proCRNN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
        disconnect(proCTPN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
        disconnect(proDeepLab, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
        disconnect(proEAST, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
        qCritical() << "local server start failed!";
        myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                      QStringLiteral("本地服务启动失败!"), QStringLiteral("请检查该服务路径是否正确并重启程序"));
        qApp->quit();
    }
    if(proAlexNet==sender())
    {
        emit proCrash(proAlexNetPath);
        proAlexNet->start("python", QStringList() << proAlexNetPath);
    }
    else if(proCRNN == sender())
    {
        emit proCrash(proCRNNPath);
        proCRNN->start("python", QStringList() << proCRNNPath);
    }
    else if(proCTPN == sender())
    {
        emit proCrash(proCTPNPath);
        proCTPN->start("python", QStringList() << proCTPNPath);
    }
    else if(proDeepLab == sender())
    {
        emit proCrash(proDeepLabPath);
        proDeepLab->start("python", QStringList() << proDeepLabPath);
    }
    else if(proEAST == sender())
    {
        emit proCrash(proEASTPath);
        proEAST->start("python", QStringList() << proEASTPath);
    }
    emit proRecover();
}

void ProcessMgmt::startProcess()
{
    connect(proAlexNet, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proCRNN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proCTPN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proDeepLab, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    connect(proEAST, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);


    this->proAlexNet->start("python", QStringList() << proAlexNetPath);
#if 1
    bool startSuccess;
    startSuccess = proAlexNet->waitForStarted(30000);
    if(!startSuccess)
    {
        qCritical() << proAlexNetPath + " start failed!";
        myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                     proAlexNetPath + QStringLiteral(" 启动失败!"), QStringLiteral("请检查该服务路径是否正确并重启程序和是否安装python环境！"));
        qApp->quit();
    }
#endif
    this->proCRNN->start("python", QStringList() << proCRNNPath);
    //this->proCTPN->start("python", QStringList() << proCTPNPath);
    this->proDeepLab->start("python", QStringList() << proDeepLabPath);
    this->proEAST->start("python", QStringList() << proEASTPath);
    connect(proAlexNet, SIGNAL(readyReadStandardOutput()), this, SLOT(output()));

}

void ProcessMgmt::errorHandler(QProcess::ProcessError /*error*/)
{
    //std::cout << error << static_cast<QProcess *>(sender())->errorString().toStdString() <<std::endl;
std::cout << "in errorHandle " << std::endl;
     myMessageBox(static_cast<QWidget *>(this->parent()), QMessageBox::Critical, QStringLiteral("错误"), \
                     proAlexNetPath + QStringLiteral(" 启动失败!"), QStringLiteral("请检查该服务路径是否正确并重启程序"));
}

void ProcessMgmt::closeAllProcess()
{
    disconnect(proAlexNet, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    disconnect(proCRNN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    disconnect(proCTPN, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    disconnect(proDeepLab, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    disconnect(proEAST, static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessMgmt::finishHandler);
    this->proAlexNet->close();
    this->proCRNN->close();
    this->proCTPN->close();
    this->proDeepLab->close();
    this->proEAST->close();

}

void ProcessMgmt::output()
{

    QString logDirectory  = QApplication::applicationDirPath() + "/Log/";
    QDir dir(logDirectory);
    if(!dir.exists())
        dir.mkdir(logDirectory);
    logDirectory.append("python_server.log");

    QFile file(logDirectory);
    file.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&file);
    textStream <<  proAlexNet->readAll();
std::cout << proAlexNet->readAll().toStdString();
    file.close();

}
