#ifndef TRANSACTIONTHREAD_H
#define TRANSACTIONTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

class Transaction
{
public:
    virtual ~Transaction() { }
    virtual void apply() = 0;
private:
};

class LocationTvTransaction : public Transaction
{
private:
    QString openSourceFileName = QString::null;
    QString tempJpgFileName = QString::null;
    QString saveResultFileName = QString::null;
public:
    LocationTvTransaction(QString openSourceFileName, QString tempJgpFileName, QString saveResultFileName);
    void apply();
};

/*class OnlineTvTransaction : public Transaction
{
private:
    QString saveSourceFileName = QString::null;
    QString saveResultFileName = QString::null;
public:
    OnlineTvTransaction(QString saveSourceFileName, QString saveResultFileName);
    void apply();
};*/

class TransactionThread : public QThread
{
    Q_OBJECT

public:
    TransactionThread();
    ~TransactionThread();

    void addTransaction(Transaction *transact);

signals:
//    void transactionStarted(const QString &message);
    void allTransactionsDone(QString);

protected:
    void run();

private:
    QQueue<Transaction *> transactions;
    QWaitCondition transactionAdded;
    QMutex mutex;
};

#endif
