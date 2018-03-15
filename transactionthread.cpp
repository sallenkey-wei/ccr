#include "transactionthread.h"
#include <string>
#include <QFileInfo>
#include <iostream>
#include <exception>
#include <qDebug>
#include <iostream>

Transaction * const EndTransaction = 0;

void LocationTvTransaction::apply()
{
    bool ret = true;
    ret = false;
    //ret = choose_mat(openSourceFileName.toStdString(), tempJpgFileName.toStdString(), 2);
    if(ret == false)
    {
        qCritical("choose_mat failed!");
        return;
    }
    //ret = choose_with_color(tempJpgFileName.toStdString(), saveResultFileName.toStdString());
    if(ret == false)
    {
        qCritical("choose_with_color failed!");
    }
}

LocationTvTransaction::LocationTvTransaction(QString openSourceFileName, QString tempJpgFileName, QString saveResultFileName)
{
    this->openSourceFileName = openSourceFileName;
    this->tempJpgFileName = tempJpgFileName;
    this->saveResultFileName = saveResultFileName;
}

TransactionThread::TransactionThread()
{
    start();
}

TransactionThread::~TransactionThread()
{
    {
        QMutexLocker locker(&mutex);
        while (!transactions.isEmpty())
            delete transactions.dequeue();
        transactions.enqueue(EndTransaction);
        transactionAdded.wakeOne();
    }
    wait();
}

void TransactionThread::addTransaction(Transaction *transact)
{
    QMutexLocker locker(&mutex);
    transactions.enqueue(transact);
    transactionAdded.wakeOne();
}

void TransactionThread::run()
{
    Transaction *transact = 0;
    forever
    {
        {
            QMutexLocker locker(&mutex);
            while (transactions.isEmpty())
                transactionAdded.wait(&mutex);
            transact = transactions.dequeue();
            if (transact == EndTransaction)
                break;
        }
        transact->apply();
        delete transact;
        {
            QMutexLocker locker(&mutex);
            if (transactions.isEmpty())
            {
                emit allTransactionsDone("result");
            }
        }
    }
}
