#include "mymessagebox.h"
#include <QWidget>
#include <QString>

int myMessageBox(QWidget *parent, QMessageBox::Icon icon, QString title, QString text, QString informativeText)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok, QStringLiteral("确定"));
    msgBox.setButtonText(QMessageBox::Cancel, QStringLiteral("取消"));
    return msgBox.exec();
}
