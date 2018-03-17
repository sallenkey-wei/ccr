#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    QString proAlexNetPath;
    QString proCRNNPath;
    QString proCTPNPath;
    QString proDeepLabPath;
    QString proEASTPath;
public slots:
    int ConfigDialog::exec();

private slots:
    void on_aLexNetPB_clicked();

    void on_cRNNPB_clicked();

    void on_cTPNPB_clicked();

    void on_deepLabPB_clicked();

    void on_eASTPB_clicked();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
