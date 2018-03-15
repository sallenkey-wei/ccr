#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "log.h"
#include <QApplication>
#include <QSize>

void outputMessage(QtMsgType, const QMessageLogContext &, const QString &);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QLoggingCategory::setFilterRules("qt.custom.log.debug=true");
    //qSetMessagePattern("[%{type}] [%{time yyyy-MM-dd hh:mm:ss ddd}] [%{file} Line:%{line}] Function:%{function}() %{message}");

    //QLoggingCategory category("qt.custom.log");
    //qDebug("hello");
    //qCDebug(category) << "hello";

    logInit();
    MainWindow w;
    w.show();
    w.initPixmapLabelSize();

    return a.exec();
}

