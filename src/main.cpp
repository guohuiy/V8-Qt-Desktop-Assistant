#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "ui/mainwindow/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
