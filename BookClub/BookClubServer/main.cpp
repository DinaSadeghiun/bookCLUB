#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!DatabaseManager::instance().initDatabase()) {
        return -1;

    }
    MainWindow w;
    w.show();
    return QApplication::exec();
}
