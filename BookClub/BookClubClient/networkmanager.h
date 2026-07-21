#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>

class NetworkManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager() override;

    bool isConnected() const;

public slots:
    void connectToServer(const QString& host, quint16 port = 12345);
    void disconnectFromServer();

    // Auth
    void login(const QString& username, const QString& password);
    void registerUser(const QString& username, const QString& password, const QString& securityAnswer, double initialBalance);
    void loginPublisher(const QString& username, const QString& password);
    void registerPublisher(const QString& username, const QString& password, const QString& companyName, const QString& securityAnswer);
    void loginAdmin(const QString& username, const QString& password);
    void resetPassword(const QString& username, const QString& securityAnswer, const QString& newPassword);

    //Books
    void getAllBooks();
    void addBook(int publisherId, const QString& title, const QString& author, double price, int genre);
    void updateBookPrice(int publisherId, int bookId, double price);
    void removeBook(int publisherId, int bookId);
    void getBookDetails(int bookId);
    void searchBooks(const QString& query);

    // Comments
    void addComment(int userId, int bookId, const QString& text, double rating);
    void getComments(int bookId);
    void removeComment(int commentId);

    // Cart & Orders
    void addToCart(int userId, int bookId);
    void removeFromCart(int userId, int bookId);
    void getCart(int userId);
    void checkout(int userId);
    void getOrderHistory(int userId);

    //Personal Library
    void getWishlist(int userId);
    void addToWishlist(int userId, int bookId);
    void removeFromWishlist(int userId, int bookId);
    void getPurchasedBooks(int userId);
    void createShelf(int userId, const QString& shelfName);
    void deleteShelf(int userId, const QString& shelfName);
    void addBookToShelf(int userId, const QString& shelfName, int bookId);
    void removeBookFromShelf(int userId, const QString& shelfName, int bookId);
    void getBooksInShelf(int userId, const QString& shelfName);
    void getShelfNames(int userId);

    //Admin Actions
    void getAllUsers();
    void getAllPublishers();
    void blockUser(int targetId);
    void unblockUser(int targetId);
    void deleteUserByAdmin(int targetId);
    void adminRemoveComment(int commentId);

signals:
    void connectionChanged(bool connected);
    void errorOccurred(const QString& message);

    void responseReceived(const QString& action, const QString& status, const QJsonObject& data);
    void rawResponseReceived(const QJsonObject& response);
    void notificationReceived(const QJsonObject& notificationDetails);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    void sendRequest(const QString& action, const QJsonObject& data);

    QTcpSocket* socket;
    QByteArray buffer;
};

#endif
