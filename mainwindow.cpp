#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_groupboxforlocation.h"
#include "ui_groupboxforonline.h"
#include "ui_widgetforopensource.h"
#include "ui_widgetforsavesource.h"
#include "common.h"
#include <QLoggingCategory>
#include <QMessageLogger>
#include "daemonthread.h"
#include "daemon.h"
#include <iterator>
#include <map>
#include <algorithm>
#include <iostream>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    groupBoxForLocation(new QGroupBox(this)),
    stack2OpenUi(new Ui::GroupBoxForLocation),
    groupBoxForOnline(new QGroupBox(this)),
    stack2SaveUi(new Ui::GroupBoxForOnline),
    widgetForOpenSource(new QWidget(this)),
    stack1OpenUi(new Ui::WidgetForOpenSource),
    widgetForSaveSource(new QWidget(this)),
    stack1SaveUi(new Ui::WidgetForSaveSource),
    timer(new QTimer(this))
{
    ui->setupUi(this);
    stack2OpenUi->setupUi(groupBoxForLocation);
    stack2SaveUi->setupUi(groupBoxForOnline);
    stack1OpenUi->setupUi(widgetForOpenSource);
    stack1SaveUi->setupUi(widgetForSaveSource);
    setUpStackedLayout();
    setUpStackedLayout2();
    setUpStatusBar();

    initMap();

    initConnection();
    readSetting();
//for(auto iterator = mapPaths.begin(); iterator != mapPaths.end(); iterator++)
//{
//    qDebug() << "in construce MainWindow" << iterator.key() << "   " << iterator.value()->objectName();
//}
    initUiState();

    ui->menuBar->hide();
    ui->saveResultLineEdit->setFocusPolicy(Qt::NoFocus);
    stack1SaveUi->saveSourceLineEdit->setFocusPolicy(Qt::NoFocus);
    stack1OpenUi->openSourceLineEdit->setFocusPolicy(Qt::NoFocus);








    daemonThread = new DaemonThread(this);
    connect(ui->openServerPB, &QPushButton::clicked, this, &MainWindow::startDaemon);
    connect(ui->closeServerPB, &QPushButton::clicked, this, &MainWindow::endDaemon);

    connect(daemonThread->daemon, &Daemon::restartFailed, this, &MainWindow::daemonRstFailedHandler);//, Qt::BlockingQueuedConnection);
    connect(daemonThread->daemon, &Daemon::noPythonEnv, this, &MainWindow::daeNoPyHandler);//, Qt::BlockingQueuedConnection);
    connect(daemonThread->daemon, &Daemon::readyRead, this, &MainWindow::outToTE);//, Qt::BlockingQueuedConnection);
    connect(daemonThread->daemon, &Daemon::startRun, this, &MainWindow::certainProStart);
    connect(daemonThread->daemon, &Daemon::terminateRun, this, &MainWindow::certainProPause);

    for(auto iterator = listPaths.begin(); iterator != listPaths.end(); iterator++)
    {
        connect(*iterator, &QPushButton::clicked, this, &MainWindow::lServerPBSlot);
    }

}

void MainWindow::certainProStart(QString path)
{
//for(auto iterator = mapPaths.begin(); iterator != mapPaths.end(); iterator++)
//{
//    qDebug() << mapComb[iterator.value()]->text->objectName();
//}
    qInfo() << path <<" start";
    mapComb[mapPaths[path]]->green->setStyleSheet(QStringLiteral("image: url(:/new/icon/images/green.png);"));
    mapComb[mapPaths[path]]->red->setStyleSheet(QStringLiteral("image: url(:/new/icon/images/gray.png);"));
}

void MainWindow::certainProPause(QString path)
{
    qInfo() << path << " pause";
    mapComb[mapPaths[path]]->green->setStyleSheet(QStringLiteral("image: url(:/new/icon/images/gray.png);"));
    mapComb[mapPaths[path]]->red->setStyleSheet(QStringLiteral("image: url(:/new/icon/images/red.png);"));
}

void MainWindow::outToTE(QString serPath, QString message)
{
//for(auto iterator = mapPaths.begin(); iterator != mapPaths.end(); iterator++)
//{
//    qDebug() << mapComb[iterator.value()]->text->objectName();
//}
    qInfo() << " serPath print a message: " << message;
    QPlainTextEdit * temp = mapComb[mapPaths[serPath]]->text;
    QTextCursor textCursor = temp->textCursor();
    textCursor.movePosition(QTextCursor::End);
    textCursor.insertText(message);
    QScrollBar * scrollBar = temp->verticalScrollBar();
    if(scrollBar)
    {
        scrollBar->setSliderPosition(scrollBar->maximum());
    }
}

void MainWindow::daeNoPyHandler()
{
    qCritical() << " no Python Environment, please install python";
    myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告") ,QStringLiteral("无法启动python"),\
                 QStringLiteral("请检查系统是否安装python并将python放入系统环境变量"));
    qApp->quit();
}

void MainWindow::daemonRstFailedHandler(QString serverPath)
{
    qCritical() << serverPath << " can not restart, qApp will exit";
    myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("已尝试多次重启服务，均已失败，请确认服务路径正确并重启程序"),\
                 QStringLiteral("出错服务为") + serverPath);
    qApp->quit();
}

void MainWindow::startDaemon()
{
    qInfo() << "start all the server";
    for(auto iterator = mapComb.begin(); iterator != mapComb.end(); iterator++)
    {
        QPushButton * temp = iterator.key();
        temp->setEnabled(false);
    }
    ui->openServerPB->setEnabled(false);
    ui->clearPathsPB->setEnabled(false);
    QStringList strListPaths;
    /*之所以没有直接遍历mapPaths是因为，以下这种方式可以保证按顺序启动进程*/
    for(auto iterator = listPaths.begin(); iterator != listPaths.end(); iterator++)
    {
        QString temp = (mapComb[*iterator])->path->text();
        if(!temp.isEmpty())
        {
            strListPaths << temp;
        }
    }
//for(auto iterator = strListPaths.begin(); iterator != strListPaths.end(); iterator++)
//{
//    QString temp = *iterator;
//    std::cout <<temp.toStdString() << std::endl;
//}
    /*采用这种方式调用是因为Daemon类成员函数非线程安全函数*/
    QMetaObject::invokeMethod(daemonThread->daemon, "setProPaths", Qt::QueuedConnection, Q_ARG(QStringList, strListPaths));
    //daemonThread->daemon->setProPaths(strListPaths);
    QMetaObject::invokeMethod(daemonThread->daemon, "startAllPro", Qt::QueuedConnection);
}

void MainWindow::endDaemon()
{
    qInfo() << "close all the server";
    QMetaObject::invokeMethod(daemonThread->daemon, "closeAllPro", Qt::QueuedConnection);
    for(auto iterator = mapComb.begin(); iterator != mapComb.end(); iterator++)
    {
        QPushButton * temp = iterator.key();
        temp->setEnabled(true);
    }
    ui->openServerPB->setEnabled(true);
    ui->clearPathsPB->setEnabled(true);
}

void MainWindow::lServerPBSlot()
{
//for(auto iterator = mapPaths.begin(); iterator != mapPaths.end(); iterator++)
//{
//    std::cout << iterator.key().toStdString() << std::endl;
//}
    QPushButton * pSender = static_cast<QPushButton *>(sender());

    /*查找此按钮原来设置的路径*/
    QString originalPath;
    std::map<QString, QPushButton *> stdMap = mapPaths.toStdMap();
    std::map<QString, QPushButton*>::iterator tarIterator = std::find_if(stdMap.begin(), stdMap.end(),
        [pSender](std::map<QString, QPushButton *>::value_type item)
    {
        return item.second == pSender;
    });

    if (tarIterator!= stdMap.end())
    {
        originalPath = tarIterator->first;
    }
    else
    {
        originalPath = "";
    }
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), originalPath,tr("py(*.py);;All(*.*)"));
    if(!tempPath.isEmpty())
    {
        qInfo() << tempPath << " is choosed" ;
        if(mapPaths.contains(tempPath))
        {
            myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("此服务已被添加"), QStringLiteral("请重新选择服务"));
            return ;
        }
        if(originalPath != "")
        {
            mapPaths.remove(originalPath);
        }
        mapPaths.insert(tempPath, pSender);
        mapComb[pSender]->path->setText(tempPath);


        QSettings settings("YuCore", "num identify");
        settings.setValue(pSender->objectName(), tempPath);
    }

//for(auto iterator = mapPaths.begin(); iterator != mapPaths.end(); iterator++)
//{
//    qDebug() << iterator.key() << "  " << iterator.value()->objectName();
//}
}

MainWindow::~MainWindow()
{
    writeSetting();
    delete ui;
    delete stack2OpenUi;
    delete stack2SaveUi;
    delete stack1OpenUi;
    delete stack1SaveUi;
    for(auto iterator = mapComb.begin(); iterator != mapComb.end(); iterator++)
    {
        delete iterator.value();
    }
}

void MainWindow::initMap()
{
    Comb * temp = new Comb;
    temp->text = ui->alexNetTE;
    temp->green = ui->alexNetGLabel;
    temp->red = ui->alexNetRLabel;
    temp->path = ui->alexNetLE;
    mapComb.insert(ui->alexNetPB, temp);
    listPaths.push_back(ui->alexNetPB);

    temp = new Comb;
    temp->text = ui->cRNNTE;
    temp->green = ui->cRNNGLabel;
    temp->red = ui->cRNNRLabel;
    temp->path = ui->cRNNLE;
    mapComb.insert(ui->cRNNPB, temp);
    listPaths.push_back(ui->cRNNPB);

    temp = new Comb;
    temp->text = ui->cTPNTE;
    temp->green = ui->cTPNGLabel;
    temp->red = ui->cTPNRLabel;
    temp->path = ui->cTPNLE;
    mapComb.insert(ui->cTPNPB, temp);
    listPaths.push_back(ui->cTPNPB);

    temp = new Comb;
    temp->text = ui->deepLabTE;
    temp->green = ui->deepLabGLabel;
    temp->red = ui->deepLabRLabel;
    temp->path = ui->deepLabLE;
    mapComb.insert(ui->deepLabPB, temp);
    listPaths.push_back(ui->deepLabPB);

    temp = new Comb;
    temp->text = ui->eASTTE;
    temp->green = ui->eASTGLabel;
    temp->red = ui->eASTRLabel;
    temp->path = ui->eASTLE;
    mapComb.insert(ui->eASTPB, temp);
    listPaths.push_back(ui->eASTPB);

    temp = new Comb;
    temp->text = ui->otherTE;
    temp->green = ui->otherGLabel;
    temp->red = ui->otherRLabel;
    temp->path = ui->otherLE;
    mapComb.insert(ui->otherPB, temp);
    listPaths.push_back(ui->otherPB);
}

void MainWindow::initUiState()
{
    stack1SaveUi->saveSourceCheckBox->setChecked(saveSource);
    stack1SaveUi->saveSourceToolButton->setEnabled(saveSource);

    ui->saveResultCheckBox->setChecked(saveResult);
    ui->saveResultToolButton->setEnabled(saveResult);
}

void MainWindow::initConnection()
{
    connect(ui->toolRtv, &QAction::triggered, this, &MainWindow::rtvClickedSlot);
    connect(ui->toolLtv, &QAction::triggered, this, &MainWindow::ltvClickedSlot);
    connect(ui->toolConf, &QAction::triggered, this, &MainWindow::configSlot);
    connect(ui->toolExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->saveResultToolButton, &QToolButton::clicked, this, &MainWindow::saveResultFileSlot);
    connect(stack1OpenUi->openSourceToolButton, &QToolButton::clicked, this, &MainWindow::openSourceFileSlot);
    connect(stack1SaveUi->saveSourceToolButton, &QToolButton::clicked, this, &MainWindow::saveSourceFileSlot);

    connect(ui->saveResultLineEdit, &QLineEdit::editingFinished, this, &MainWindow::saveResultLineEditChangedSlot);
    connect(stack1OpenUi->openSourceLineEdit, &QLineEdit::editingFinished, this, &MainWindow::openSourceLineEditChangedSlot);

    connect(ui->saveResultCheckBox, &QCheckBox::stateChanged, this, &MainWindow::saveResultCheckBoxSlot);
    connect(stack1SaveUi->saveSourceCheckBox, &QCheckBox::stateChanged, this, &MainWindow::saveSourceCheckBoxSlot);

    connect(stack2OpenUi->beginPushButton, &QPushButton::clicked, this, &MainWindow::beginButtonClickSlot);
    connect(stack2OpenUi->pausePushButton, &QPushButton::clicked, this, &MainWindow::pauseButtonClickSlot);
    connect(stack2OpenUi->endPushButton, &QPushButton::clicked, this, &MainWindow::endButtonClickSlot);

    /*完善 serverConnectSlot serverDisconnectSlot*/
    connect(timer, &QTimer::timeout, this, &MainWindow::updatePixmap);

    //connect(&thread, &TransactionThread::allTransactionsDone, this, &MainWindow::allTransactionDone);
    connect(&thread, &TransactionThread::allTransactionsDone, this, &MainWindow::allTransactionDone, Qt::BlockingQueuedConnection);
}

void MainWindow::initPixmapLabelSize()
{
    this->size = ui->inputLabel->size();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(camera.cameraState())//查看是否正在处理图像
    {
        int result = myMessageBox(this, QMessageBox::Question, QStringLiteral("提示"), QStringLiteral("正在处理图像，是否确认关闭？"));
        if(result == QMessageBox::Cancel)
        {
            event->ignore();
        }
        else if(result == QMessageBox::Ok)
            event->accept();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::setUpStatusBar()
{
    progressBar = new QProgressBar(this);
    progressBar->setMaximumSize(5000, 15);
    progressBar->resize(500, 15);
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QWidget * widget = new QWidget(this);
    widget->setFixedSize(30, 15);
    ui->statusBar->addWidget(widget);
    ui->statusBar->addWidget(progressBar);
    this->progressBar->setVisible(false);
}

void MainWindow::setUpStackedLayout2()
{
    stackedLayout2 = new QStackedLayout;
    stackedLayout2->addWidget(groupBoxForOnline);
    stackedLayout2->addWidget(groupBoxForLocation);
    ui->switchFrame2->setLayout(stackedLayout2);
}

void MainWindow::setUpStackedLayout()
{

    stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(widgetForSaveSource);
    stackedLayout->addWidget(widgetForOpenSource);

    ui->switchFrame1->setLayout(stackedLayout);
}

void MainWindow::ltvClickedSlot()
{
    ui->stackedWidget->setCurrentIndex(0);
    stackedLayout->setCurrentIndex(1);
    stackedLayout2->setCurrentIndex(1);
}

void MainWindow::rtvClickedSlot()
{
    ui->stackedWidget->setCurrentIndex(0);
    stackedLayout->setCurrentIndex(0);
    stackedLayout2->setCurrentIndex(0);
}



bool MainWindow::serverConnectSlot()
{
    return true;
}

bool MainWindow::openSourceFileSlot()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), fileDialogPath, QStringLiteral("video (*.avi;*.mp4;*.asf);;All (*.*)"));
    if(!tempPath.isEmpty())
    {
        this->openSourceFileName = tempPath;
        stack1OpenUi->openSourceLineEdit->setText(tempPath);
        fileDialogPath = QFileInfo(tempPath).path();
        qCInfo(QLoggingCategory("custom")) << QString("Open file's name is %1").arg(tempPath);
//std::cout <<"fileDialogPath is " << fileDialogPath.toStdString() << std::endl;
        return true;
    }
    else
    {
        qCInfo(QLoggingCategory("custom")) << QString("Give up open file, old filePath is %1").arg(openSourceFileName);
        return false;
    }
}

void MainWindow::openSourceLineEditChangedSlot()
{
    if(stack1OpenUi->openSourceLineEdit->hasFocus())
        return;
    QFileInfo fileInfo(stack1OpenUi->openSourceLineEdit->text());
    if(stack1OpenUi->openSourceLineEdit->text().isEmpty() || !fileInfo.exists() || fileInfo.isDir())
    {
        myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("您输入的文件不存在！"),QStringLiteral("请您确认后重新输入！"));
        stack1OpenUi->openSourceLineEdit->setText(this->openSourceFileName);
    }
    else
    {
        this->openSourceFileName = stack1OpenUi->openSourceLineEdit->text();
    }
}

bool MainWindow::saveSourceFileSlot()
{
    QString tempPath = QFileDialog::getSaveFileName(this, QStringLiteral("保存"), fileDialogPath, tr("Jpg(*.jpg);;All (*.*)"));
    if(!tempPath.isEmpty())
    {
        stack1SaveUi->saveSourceLineEdit->setText(tempPath);
        saveSourceFileName = tempPath;
        fileDialogPath = QFileInfo(tempPath).path();
        qCInfo(QLoggingCategory("custom")) << QString("Save source file's name is %1").arg(tempPath);
        return true;
    }
    else
    {
        qCInfo(QLoggingCategory("custom")) << QString("Give up open file, old save source file's name is %1").arg(tempPath);
        return false;
    }
}

bool MainWindow::configSlot()
{
    ui->stackedWidget->setCurrentIndex(1);
    return true;
}

bool MainWindow::saveResultFileSlot()
{
    QString tempPath = QFileDialog::getSaveFileName(this, QStringLiteral("保存"), fileDialogPath, tr("Jpg(*.jpg);;Video(*.mp4);;All (*.*)"));
    if(!tempPath.isEmpty())
    {
        ui->saveResultLineEdit->setText(tempPath);
        saveResultFileName = tempPath;
        fileDialogPath = QFileInfo(tempPath).path();
        qCInfo(QLoggingCategory("custom")) << QString("Save result file's name is %1").arg(tempPath);
        return true;
    }
    else
    {
        qCInfo(QLoggingCategory("custom")) << QString("Give up open file, old save result file's name is %1").arg(tempPath);
        return false;
    }
}

void MainWindow::saveResultLineEditChangedSlot()
{
    if(ui->saveResultLineEdit->hasFocus())
        return;
    QFileInfo fileInfo(ui->saveResultLineEdit->text());
    if(ui->saveResultLineEdit->text().isEmpty() || fileInfo.isDir())
    {
        myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("您输入的文件不存在！"),QStringLiteral("请您确认后重新输入！"));
        ui->saveResultLineEdit->setText(this->saveResultFileName);
        return ;
    }
    else if(fileInfo.exists())
    {
        int ret = QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("文件已存在，是否替換？"), QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
            QFile::remove(ui->saveResultLineEdit->text());
        }
        else if(ret == QMessageBox::No)
        {
            ui->saveResultLineEdit->setText(this->saveResultFileName);
            return ;
        }
    }
    this->saveResultFileName = ui->saveResultLineEdit->text();
}

void MainWindow::beginButtonClickSlot()
{
    qInfo("Function beginBuggonClickSlot() begin");
    if(this->openSourceFileName.isEmpty())
    {
        myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("打开文件路径出错！"), QStringLiteral("请检查路径！"));
        qWarning("openSourceFileName is Empty!");
        qInfo("Function beginBuggonClickSlot() end");
        return;
    }
    if((this->saveResultFileName.isEmpty() && this->saveResult))
    {
        myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("保存文件路径出错！"), QStringLiteral("请检查路径！"));
        qWarning("saveResultFileName is Empty and the saveResult is true!");
        qInfo("Function beginBuggonClickSlot() end");
        return;
    }
    if(camera.cameraState() && camera.getPath() == this->openSourceFileName)
    {
        timer->start(33);
        qInfo("continue to display!");
        qInfo("Function beginBuggonClickSlot() end");
        return ;
    }
    this->camera.setPath(this->openSourceFileName);
    if(!this->camera.openCamera())
    {
        myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("打开视频文件出错！"));
        qWarning("open video file failed!");
        qInfo("Function beginBuggonClickSlot() end");
        return;
    }
    frameIndex = 0;
    frameNum = camera.getFrameNum();
    timer->start(33);

    QFileInfo fileInfo(openSourceFileName);
    QString tempPath = fileInfo.path() + "/temp/";
    QDir dirName(tempPath);
    if(!dirName.exists())
    {
       bool ret = dirName.mkpath(tempPath);
       if(!ret)
       {
           qCritical("mkPath failed!");
           qInfo("Function beginBuggonClickSlot() end");
           return;
       }
    }
    tempJpgFileName = tempPath + fileInfo.baseName() + ".jpg";
    qCInfo(QLoggingCategory("custom")) << QString("tempJpgFileName is %1.").arg(tempJpgFileName);
    if(!this->saveResult)
    {
        tempSaveResultFileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/" + fileInfo.baseName() + ".jpg";
        this->addTransaction(new LocationTvTransaction(openSourceFileName, tempJpgFileName, tempSaveResultFileName));
    }
    else
    {
        this->addTransaction(new LocationTvTransaction(openSourceFileName, tempJpgFileName, saveResultFileName));
    }
    qInfo("addTransaction to subThread!");
    qInfo("Function beginBuggonClickSlot() end");
}

void MainWindow::endButtonClickSlot()
{
    timer->stop();
    this->progressBar->setVisible(false);
    ui->toolRtv->setEnabled(true);
    camera.closeCamera();
    qInfo("");
}

void MainWindow::pauseButtonClickSlot()
{
     timer->stop();
     qInfo("");
}

void MainWindow::connectButtonClickSlot()
{

}

void MainWindow::disconnectButtonClickSlot()
{

}

void MainWindow::saveSourceCheckBoxSlot(int checkState)
{
    this->saveSource = (bool)checkState;
    //this->saveSourceLineEdit->setFocusPolicy((saveSource ? Qt::StrongFocus : Qt::NoFocus));
    stack1SaveUi->saveSourceToolButton->setEnabled(saveSource);
}

void MainWindow::saveResultCheckBoxSlot(int checkState)
{
    this->saveResult = (bool)checkState;
    //ui->saveResultLineEdit->setFocusPolicy((saveResult ? Qt::StrongFocus : Qt::NoFocus));
    ui->saveResultToolButton->setEnabled(saveResult);
}

void MainWindow::updatePixmap()
{
    if(frameIndex < frameNum)
    {
        this->progressBar->setValue((int)((double)frameIndex/(double)frameNum*100.0));
        this->progressBar->setVisible(true);
        ui->toolRtv->setEnabled(false);
        frameIndex++;
//std::cout << "frameIndex is " << frameIndex << std::endl;
        QPixmap pixmap;
        bool ret = camera.readFrame(pixmap);
        if(!ret)
        {
            myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("读取视频文件出错！"));
            qCWarning(QLoggingCategory("custom")) << QString("readFrame failed, frameNum is %1, frameIndex is %2.").arg(frameNum).arg(frameIndex);
            endButtonClickSlot();
            return;
        }
        ui->inputLabel->setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
        endButtonClickSlot();
}

void MainWindow::addTransaction(Transaction * transact)
{
    thread.addTransaction(transact);
    stack2OpenUi->beginPushButton->setEnabled(false);
}

void MainWindow::allTransactionDone(QString string)
{
    stack2OpenUi->beginPushButton->setEnabled(true);
    //QImage image(saveResultFileName);
    QImage image("C:/Users/yuechip/Desktop/asf/output/7.jpg");
    ui->resultLabel->setPixmap(QPixmap::fromImage(image).scaled(this->size, Qt::KeepAspectRatio));
    ui->resultStringLabel->setText(string);
    if(!this->saveResult)
    {
        QFile::remove(this->tempSaveResultFileName);
    }
    QFile::remove(this->tempJpgFileName);
    //只看这里
    QMessageBox::information(this, "information", "information");
}

void MainWindow::readSetting()
{
    QSettings settings("YuCore", "num identify");
    restoreGeometry(settings.value("geometry").toByteArray());
    fileDialogPath = settings.value("fileDialogPath", "").toString();
    qCInfo(QLoggingCategory("custom")) << QString("fileDialogPath is %1.").arg(fileDialogPath);
    saveSource = settings.value("saveSource", false).toBool();
    saveResult = settings.value("saveResult", false).toBool();
    for(auto iterator = listPaths.begin(); iterator != listPaths.end(); iterator++)
    {
        QPushButton * pPB = *iterator;
        QString temp = settings.value(pPB->objectName(), "").toString();
//qDebug() << "in readSetting " << temp << pPB->objectName();
        if(temp != "")
        {
            mapPaths.insert(temp, pPB);
            mapComb[pPB]->path->setText(temp);
        }
    }
}

void MainWindow::writeSetting()
{
    QSettings settings("YuCore", "num identify");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("fileDialogPath",  fileDialogPath);
    settings.setValue("saveSource", saveSource);
    settings.setValue("saveResult", saveResult);

}

/*清除所有路径按键*/
void MainWindow::on_clearPathsPB_clicked()
{
    qInfo() << "clear all server paths";
    mapPaths.clear();
    for(auto iterator = mapComb.begin(); iterator != mapComb.end(); iterator++)
    {
        Comb * temp = iterator.value();
        temp->path->clear();
        temp->text->clear();
        QPushButton * pB = iterator.key();
        QSettings settings("YuCore", "num identify");
        settings.setValue(pB->objectName(), "");
    }
}
