#include <QCoreApplication>
#include "Network/bookclubserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BookClubServer* server = new BookClubServer(&a);

    quint16 port = 12345;
    if (!server->startServer(port)) {
        return -1;
    }

    return a.exec();
}
