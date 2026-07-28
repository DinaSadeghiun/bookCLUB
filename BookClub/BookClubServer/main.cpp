#include <QCoreApplication>
#include "Network/bookclubserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // port 8080
    BookClubServer* server = new BookClubServer(&a);
    if (!server->startServer(8080)) {
        qDebug() << "Failed to start server!";
        return -1;
    }

    qDebug() << "Server started successfully on port 8080.";

    return a.exec();
}
