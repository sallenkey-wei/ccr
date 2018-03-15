#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSize>
#include "transactionthread.h"
#include "camera.h"

class QTimer;
class QStackedLayout;
class QLineEdit;
class QToolButton;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QGroupBox;

namespace Ui {
class MainWindow;
class GroupBoxForLocation;
class GroupBoxForOnline;
class WidgetForOpenSource;
class WidgetForSaveSource;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initPixmapLabelSize();
private slots:
    bool serverConnectSlot();
    bool openSourceFileSlot();
    void openSourceLineEditChangedSlot();
    void saveResultLineEditChangedSlot();
    bool saveSourceFileSlot();
    bool saveResultFileSlot();
    bool configSlot();
    void switchWidgetSlot(int);
    void beginButtonClickSlot();
    void pauseButtonClickSlot();
    void endButtonClickSlot();
    void connectButtonClickSlot();
    void disconnectButtonClickSlot();
    void ltvClickedSlot();
    void rtvClickedSlot();
    void saveSourceCheckBoxSlot(int);
    void saveResultCheckBoxSlot(int);
    void updatePixmap();
    void allTransactionDone(QString);

signals:
    void switchWidgeted(int);

private:
    Ui::MainWindow *ui;
    QStackedLayout * stackedLayout, * stackedLayout2;

    QProgressBar * progressBar;

    QGroupBox * groupBoxForLocation;
    Ui::GroupBoxForLocation * stack2OpenUi;//groupBoxForLocationUi;

    QGroupBox * groupBoxForOnline;
    Ui::GroupBoxForOnline * stack2SaveUi;//groupBoxForOnlineUi;

    QWidget * widgetForOpenSource;
    Ui::WidgetForOpenSource * stack1OpenUi;

    QWidget * widgetForSaveSource;
    Ui::WidgetForSaveSource * stack1SaveUi;

    QTimer * timer;

    QString openSourceFileName = QString::null;
    QString saveSourceFileName = QString::null;
    QString tempJpgFileName = QString::null;
    QString saveResultFileName = QString::null;
    QString tempSaveResultFileName = QString::null;

    bool saveSource = false;
    bool saveResult = false;
    QString fileDialogPath = QString::null;

    Camera camera;
    long frameNum = 0;
    long frameIndex = 0;

    QSize size;
    TransactionThread thread;

private:
    void setUpStackedLayout();
    void setUpStatusBar();
    void setUpStackedLayout2();
    void initUiState();
    void initConnection();
    void closeEvent(QCloseEvent * event);
    void addTransaction(Transaction * transact);
    void readSetting();
    void writeSetting();
};

#endif // MAINWINDOW_H
