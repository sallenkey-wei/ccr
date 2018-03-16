#ifndef PROCESSMGMT_H
#define PROCESSMGMT_H

#include <QObject>
#include <QProcess>
#include <time.h>


class ProcessMgmt : public QObject
{
    Q_OBJECT
public:
    explicit ProcessMgmt(QObject *parent = 0);
    ~ProcessMgmt();
    QProcess * proAlexNet;
    QProcess * proCRNN;
    QProcess * proCTPN;
    QProcess * proDeepLab;
    QProcess * proEAST;
signals:

public slots:
    void restartProcess(QProcess::ProcessState newState);
    void closeAllProcess();
    void startProcess();
    void errorHandler(QProcess::ProcessError);
    void finishHandler(int, QProcess::ExitStatus);
private:
   time_t currentTime;
   int crashTimes = 0;
};

#endif // PROCESSMGMT_H
