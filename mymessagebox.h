#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QMessageBox>

class QWidget;
class QString;

int myMessageBox(QWidget *parent, QMessageBox::Icon icon, QString title, QString text, QString informativeText = QString::null);

#endif // MYMESSAGEBOX_H
