#include "log.h"
#include <QString>
#include <QLoggingCategory>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QApplication>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("[Debug]");
        break;
    case QtInfoMsg:
        text = QString("[Info]");
        break;
    case QtWarningMsg:
        text = QString("[Warning]");
        break;
    case QtCriticalMsg:
        text = QString("[Critical]");
        break;
    case QtFatalMsg:
        text = QString("[Fatal]");
    }
    text.append(QString(" [%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    text.append(QString(" [%1: Line: %2] ").arg(QString(context.file)).arg(context.line));
    text.append(QString(" [Function: %1] \r\n").arg(QString(context.function)));
    text.append(QString("Message: %1 ").arg(msg));


    QString logDirectory  = QApplication::applicationDirPath() + "/Log/";
    QDir dir(logDirectory);
    if(!dir.exists())
        dir.mkdir(logDirectory);
    logDirectory.append(QApplication::applicationName()+".log");
    QFile file(logDirectory);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << text << "\r\n";
    file.close();
    mutex.unlock();
}

void logInit()
{
    qInstallMessageHandler(outputMessage);
}
