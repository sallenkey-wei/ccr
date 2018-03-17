#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_groupboxforlocation.h"
#include "ui_groupboxforonline.h"
#include "ui_widgetforopensource.h"
#include "ui_widgetforsavesource.h"
#include "common.h"
#include <QLoggingCategory>
#include <QMessageLogger>
#include "processmgmt.h"

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
    timer(new QTimer),
    processMgmt(new ProcessMgmt(this)),
    configDialog(new ConfigDialog(this))
{
    ui->setupUi(this);
    stack2OpenUi->setupUi(groupBoxForLocation);
    stack2SaveUi->setupUi(groupBoxForOnline);
    stack1OpenUi->setupUi(widgetForOpenSource);
    stack1SaveUi->setupUi(widgetForSaveSource);
    setUpStackedLayout();
    setUpStackedLayout2();
    setUpStatusBar();

    initConnection();
    readSetting();
    initUiState();

    ui->menuBar->hide();
    ui->saveResultLineEdit->setFocusPolicy(Qt::NoFocus);
    stack1SaveUi->saveSourceLineEdit->setFocusPolicy(Qt::NoFocus);
    stack1OpenUi->openSourceLineEdit->setFocusPolicy(Qt::NoFocus);





    configDialog->proAlexNetPath = processMgmt->proAlexNetPath;
    configDialog->proCRNNPath= processMgmt->proCRNNPath;
    configDialog->proCTPNPath = processMgmt->proCTPNPath;
    configDialog->proDeepLabPath = processMgmt->proDeepLabPath;
    configDialog->proEASTPath = processMgmt->proEASTPath;

    connect(this->processMgmt, &ProcessMgmt::proCrash, this, &MainWindow::crashHandle);
    connect(this->processMgmt, &ProcessMgmt::proRecover, this, &MainWindow::recoverHandle);

}

MainWindow::~MainWindow()
{
    writeSetting();
    delete ui;
    delete stack2OpenUi;
    delete stack2SaveUi;
    delete stack1OpenUi;
    delete stack1SaveUi;
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

    connect(this, &MainWindow::switchWidgeted, this, &MainWindow::switchWidgetSlot);

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
    emit switchWidgeted(1);
}

void MainWindow::rtvClickedSlot()
{
    emit switchWidgeted(0);
}

void MainWindow::switchWidgetSlot(int index)
{
    stackedLayout->setCurrentIndex(index);
    stackedLayout2->setCurrentIndex(index);
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
    this->configDialog->exec();
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
    processMgmt->proAlexNetPath = settings.value("proAlexNetPath", "").toString();
    processMgmt->proCRNNPath = settings.value("proCRNNPath", "").toString();
    processMgmt->proCTPNPath = settings.value("proCTPNPath", "").toString();
    processMgmt->proDeepLabPath = settings.value("proDeepLabPath", "").toString();
    processMgmt->proEASTPath = settings.value("proEASTPath", "").toString();

}

void MainWindow::writeSetting()
{
    QSettings settings("YuCore", "num identify");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("fileDialogPath",  fileDialogPath);
    settings.setValue("saveSource", saveSource);
    settings.setValue("saveResult", saveResult);

}

void MainWindow::on_actionTurnOn_triggered()
{
    processMgmt->proAlexNetPath = configDialog->proAlexNetPath;
    processMgmt->proCRNNPath = configDialog->proCRNNPath;
    processMgmt->proCTPNPath = configDialog->proCTPNPath;
    processMgmt->proDeepLabPath = configDialog->proDeepLabPath;
    processMgmt->proEASTPath = configDialog->proEASTPath;

    if(processMgmt->proAlexNetPath.isEmpty() || processMgmt->proCRNNPath.isEmpty() || processMgmt->proCTPNPath.isEmpty() \
            || processMgmt->proDeepLabPath.isEmpty() || processMgmt->proEASTPath.isEmpty())
    {
            myMessageBox(this, QMessageBox::Warning, QStringLiteral("警告"), QStringLiteral("本地服务路径为空！"));
            return;

    }

    processMgmt->startProcess();
}

void MainWindow::on_actionTurnOff_triggered()
{
    processMgmt->closeAllProcess();
}

void MainWindow::crashHandle(QString& message)
{
    QIcon icon("./images/red.ico");
    ui->ledPlay->setIcon(icon);
}

void MainWindow::recoverHandle()
{
    QIcon icon("./images/green.ico");
    ui->ledPlay->setIcon(icon);
}
