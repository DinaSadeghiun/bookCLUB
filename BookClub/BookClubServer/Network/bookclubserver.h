#ifndef BOOKCLUBSERVER_H
#define BOOKCLUBSERVER_H

#include <QTcpServer>
#include <QList>
#include <QJsonObject>
#include "clienthandler.h"
#include "Notification.h"

class Book;
class UserService;
class BookService;
class CommentService;
class ShoppingCartService;
class OrderService;
class PublisherService;
class AdminService;
class PersonalLibraryService;
class NotificationService;

class BookClubServer : public QTcpServer {
    Q_OBJECT
public:
    explicit BookClubServer(QObject* parent = nullptr);
    ~BookClubServer() override;

    bool startServer(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onRequestReceived(ClientHandler* handler, const QJsonObject& request);
    void onClientDisconnected(ClientHandler* handler);
    void onNotificationReceived(int recipientId, const Notification& notification);

private:
    void initializeServices();
    void routeRequest(ClientHandler* handler, const QJsonObject& request);

    // Action
    void handleAuth(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handleBooks(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handleComments(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handleCartAndOrder(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handlePersonalLibrary(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handleAdminActions(ClientHandler* handler, const QString& action, const QJsonObject& data);
    void handleNotifications(ClientHandler* handler, const QString& action, const QJsonObject& data);

    QList<ClientHandler*> clients;

    UserService* userService = nullptr;
    BookService* bookService = nullptr;
    CommentService* commentService = nullptr;
    ShoppingCartService* shoppingCartService = nullptr;
    OrderService* orderService = nullptr;
    PublisherService* publisherService = nullptr;
    AdminService* adminService = nullptr;
    PersonalLibraryService* personalLibraryService = nullptr;
    NotificationService* notificationService = nullptr;
};

#endif
