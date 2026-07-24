#include <QCoreApplication>
#include "Network/bookclubserver.h"
#include "DB/databasemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ۱. مقداردهی اولیه دیتابیس و جدول‌ها
    /*DatabaseManager db = DatabaseManager::instance();
    if (!db.initDatabase("bookclub.db")) {
        qDebug() << "Failed to open database!";
        return -1;
    }*/

    // ۲. اجرای سرور روی پورت 8080
    BookClubServer* server = new BookClubServer(&a);
    if (!server->startServer(8080)) {
        qDebug() << "Failed to start server!";
        return -1;
    }

    qDebug() << "Server started successfully on port 8080.";

    return a.exec();
}
