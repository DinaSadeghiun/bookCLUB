#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // This opens the server management window
    MainWindow w;
    w.show();

    return a.exec();
}
