#include "configdialog.h"
#include "ui_configdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <mainwindow.h>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

int ConfigDialog::exec()
{
    ui->aLexNetLE->setText(this->proAlexNetPath);
    ui->cRNNLE->setText(this->proCRNNPath);
    ui->cTPNLE->setText(this->proCTPNPath);
    ui->deepLabLE->setText(this->proDeepLabPath);
    ui->eASTLE->setText(this->proEASTPath);
    return QDialog::exec();
}


void ConfigDialog::on_aLexNetPB_clicked()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), proAlexNetPath,tr("py(*.py)"));
    if(!tempPath.isEmpty())
    {
        this->proAlexNetPath = tempPath;
        ui->aLexNetLE->setText(tempPath);
        QSettings settings("YuCore", "num identify");
        settings.setValue("proAlexNetPath", proAlexNetPath);
    }
}

void ConfigDialog::on_cRNNPB_clicked()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), proCRNNPath, tr("py (*.py)"));
    if(!tempPath.isEmpty())
    {
        this->proCRNNPath = tempPath;
        ui->cRNNLE->setText(tempPath);
        QSettings settings("YuCore", "num identify");
        settings.setValue("proCRNNPath", proCRNNPath);
    }

}

void ConfigDialog::on_cTPNPB_clicked()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), proCTPNPath, tr("py (*.py)"));
    if(!tempPath.isEmpty())
    {
        this->proCTPNPath = tempPath;
        ui->cTPNLE->setText(tempPath);
        QSettings settings("YuCore", "num identify");
        settings.setValue("proCTPNPath", proCTPNPath);
    }

}

void ConfigDialog::on_deepLabPB_clicked()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), proDeepLabPath, tr("py (*.py)"));
    if(!tempPath.isEmpty())
    {
        this->proDeepLabPath = tempPath;
        ui->deepLabLE->setText(tempPath);
        QSettings settings("YuCore", "num identify");
        settings.setValue("proDeepLabPath", proDeepLabPath);
    }

}

void ConfigDialog::on_eASTPB_clicked()
{
    QString tempPath = QFileDialog::getOpenFileName(this, QStringLiteral("打开"), proEASTPath, tr("py (*.py)"));
    if(!tempPath.isEmpty())
    {
        this->proEASTPath = tempPath;
        ui->eASTLE->setText(tempPath);
        QSettings settings("YuCore", "num identify");
        settings.setValue("proEASTPath", proEASTPath);
    }

}
