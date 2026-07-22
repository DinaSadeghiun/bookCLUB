#include "networkmanager.h"
#include <QJsonDocument>
#include <QDebug>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);

    // Qt 6 connection for socket errors
    connect(socket, &QTcpSocket::errorOccurred, this, &NetworkManager::onError);
}

NetworkManager::~NetworkManager()
{
    disconnectFromServer();
}

bool NetworkManager::isConnected() const
{
    return socket && socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::connectToServer(const QString& host, quint16 port)
{
    if (isConnected()) {
        return;
    }
    socket->connectToHost(host, port);
}

void NetworkManager::disconnectFromServer()
{
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->disconnectFromHost();
    }
}

// Auth
void NetworkManager::login(const QString& username, const QString& password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    sendRequest("login", data);
}

void NetworkManager::registerUser(const QString& username, const QString& password,
                                  const QString& securityAnswer,
                                  const QList<int>& favoriteGenres, double initialBalance)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    data["securityAnswer"] = securityAnswer;
    data["initialBalance"] = initialBalance;

    QJsonArray genresArray;
    for (int genre : favoriteGenres) {
        genresArray.append(genre);
    }
    data["favoriteGenres"] = genresArray;

    sendRequest("register", data);
}

void NetworkManager::loginPublisher(const QString& username, const QString& password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    sendRequest("loginPublisher", data);
}

void NetworkManager::registerPublisher(const QString& username, const QString& password, const QString& companyName, const QString& securityAnswer)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    data["companyName"] = companyName;
    data["securityAnswer"] = securityAnswer;
    sendRequest("registerPublisher", data);
}

void NetworkManager::loginAdmin(const QString& username, const QString& password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    sendRequest("loginAdmin", data);
}

void NetworkManager::changeUsername(int id, const QString& newUsername, const QString& password, const QString& role)
{
    QJsonObject data;
    data["id"] = id;
    data["username"] = newUsername;
    data["password"] = password;
    data["role"] = role;
    sendRequest("changeUsername", data);
}

void NetworkManager::resetPassword(const QString& username, const QString& securityAnswer, const QString& newPassword)
{
    QJsonObject data;
    data["username"] = username;
    data["securityAnswer"] = securityAnswer;
    data["newPassword"] = newPassword;
    sendRequest("resetPassword", data);
}

void NetworkManager::updateProfile(int userId, const QList<int>& favoriteGenres)
{
    QJsonObject data;
    data["userId"] = userId;
    QJsonArray genresArray;
    for (int genre : favoriteGenres) {
        genresArray.append(genre);
    }
    data["favoriteGenres"] = genresArray;
    sendRequest("updateProfile", data);
}

void NetworkManager::updateSecurityAnswer(int userId, const QString& securityAnswer, const QString& role)
{
    QJsonObject data;
    data["id"] = userId;
    data["securityAnswer"] = securityAnswer;
    data["role"] = role;
    sendRequest("updateSecurityAnswer", data);
}

// Books Operations
void NetworkManager::getAllBooks()
{
    qDebug() << "Client: Attempting to send getAllBooks request...";
    sendRequest("getAllBooks", QJsonObject());
}

void NetworkManager::addBook(int publisherId, const QString& title, const QString& author, double price, int genre)
{
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["title"] = title;
    data["author"] = author;
    data["price"] = price;
    data["genre"] = genre;
    sendRequest("addBook", data);
}

void NetworkManager::updateBookPrice(int publisherId, int bookId, double price)
{
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    data["price"] = price;
    sendRequest("updateBookPrice", data);
}

void NetworkManager::removeBook(int publisherId, int bookId)
{
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    sendRequest("removeBook", data);
}

void NetworkManager::getBookDetails(int bookId)
{
    QJsonObject data;
    data["bookId"] = bookId;
    sendRequest("getBookDetails", data);
}

void NetworkManager::searchBooks(const QString& query)
{
    QJsonObject data;
    data["query"] = query;
    sendRequest("searchBooks", data);
}


// Comments
void NetworkManager::addComment(int userId, int bookId, const QString& text, double rating)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    data["text"] = text;
    data["rating"] = rating;
    sendRequest("addComment", data);
}

void NetworkManager::getComments(int bookId)
{
    QJsonObject data;
    data["bookId"] = bookId;
    sendRequest("getComments", data);
}

void NetworkManager::removeComment(int commentId)
{
    QJsonObject data;
    data["commentId"] = commentId;
    sendRequest("removeComment", data);
}

void NetworkManager::editComment(int commentId, int userId, int bookId, const QString& text, double rating)
{
    QJsonObject data;
    data["id"] = commentId;
    data["userId"] = userId;
    data["bookId"] = bookId;
    data["text"] = text;
    data["rating"] = rating;
    sendRequest("editComment", data);
}


// Cart & Orders
void NetworkManager::addToCart(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("addToCart", data);
}

void NetworkManager::removeFromCart(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("removeFromCart", data);
}

void NetworkManager::getCart(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getCart", data);
}

void NetworkManager::checkout(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("checkout", data);
}

void NetworkManager::getOrderHistory(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getOrderHistory", data);
}

// Personal Library
// wishlist
void NetworkManager::getWishlist(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getWishlist", data);
}

void NetworkManager::addToWishlist(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("addToWishlist", data);
}

void NetworkManager::removeFromWishlist(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("removeFromWishlist", data);
}
// Favorites
void NetworkManager::getFavorites(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getFavorites", data);
}

void NetworkManager::addToFavorites(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("addToFavorites", data);
}

void NetworkManager::removeFromFavorites(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("removeFromFavorites", data);
}

void NetworkManager::isInFavorites(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("isInFavorites", data);
}

void NetworkManager::getPurchasedBooks(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getPurchasedBooks", data);
}
//shelves
void NetworkManager::createShelf(int userId, const QString& shelfName)
{
    QJsonObject data;
    data["userId"] = userId;
    data["shelfName"] = shelfName;
    sendRequest("createShelf", data);
}

void NetworkManager::deleteShelf(int userId, const QString& shelfName)
{
    QJsonObject data;
    data["userId"] = userId;
    data["shelfName"] = shelfName;
    sendRequest("deleteShelf", data);
}

void NetworkManager::addBookToShelf(int userId, const QString& shelfName, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["shelfName"] = shelfName;
    data["bookId"] = bookId;
    sendRequest("addBookToShelf", data);
}

void NetworkManager::removeBookFromShelf(int userId, const QString& shelfName, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["shelfName"] = shelfName;
    data["bookId"] = bookId;
    sendRequest("removeBookFromShelf", data);
}

void NetworkManager::getBooksInShelf(int userId, const QString& shelfName)
{
    QJsonObject data;
    data["userId"] = userId;
    data["shelfName"] = shelfName;
    sendRequest("getBooksInShelf", data);
}

void NetworkManager::getShelfNames(int userId)
{
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getShelfNames", data);
}

void NetworkManager::moveBookBetweenShelves(int userId, int bookId, const QString& fromShelf, const QString& toShelf)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    data["fromShelf"] = fromShelf;
    data["toShelf"] = toShelf;
    sendRequest("moveBookBetweenShelves", data);
}

// Admin Actions
void NetworkManager::getAllUsers()
{
    sendRequest("getAllUsers", QJsonObject());
}

void NetworkManager::getAllPublishers()
{
    sendRequest("getAllPublishers", QJsonObject());
}

void NetworkManager::blockUser(int targetId)
{
    QJsonObject data;
    data["targetId"] = targetId;
    sendRequest("blockUser", data);
}

void NetworkManager::unblockUser(int targetId)
{
    QJsonObject data;
    data["targetId"] = targetId;
    sendRequest("unblockUser", data);
}

void NetworkManager::deleteUserByAdmin(int targetId)
{
    QJsonObject data;
    data["targetId"] = targetId;
    sendRequest("deleteUser", data);
}

void NetworkManager::adminRemoveComment(int commentId)
{
    QJsonObject data;
    data["commentId"] = commentId;
    sendRequest("adminRemoveComment", data);
}

void NetworkManager::searchUsers(const QString& query)
{
    QJsonObject data;
    data["query"] = query;
    sendRequest("searchUsers", data);
}

void NetworkManager::searchPublishers(const QString& query)
{
    QJsonObject data;
    data["query"] = query;
    sendRequest("searchPublishers", data);
}

//Notification Actions
void NetworkManager::getNotifications(int recipientId)
{
    QJsonObject data;
    data["recipientId"] = recipientId;
    sendRequest("getNotifications", data);
}
void NetworkManager::markNotificationAsRead(int notificationId, int recipientId)
{
    QJsonObject data;
    data["notificationId"] = notificationId;
    data["recipientId"] = recipientId;
    sendRequest("markNotificationAsRead", data);
}
void NetworkManager::markAllNotificationsAsRead(int recipientId)
{
    QJsonObject data;
    data["recipientId"] = recipientId;
    sendRequest("markAllNotificationsAsRead", data);
}
void NetworkManager::deleteNotification(int notificationId, int recipientId)
{
    QJsonObject data;
    data["notificationId"] = notificationId;
    data["recipientId"] = recipientId;
    sendRequest("deleteNotification", data);
}

// Core Socket Communication & Handling
void NetworkManager::sendRequest(const QString& action, const QJsonObject& data)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server!");
        return;
    }

    QJsonObject request;
    request["action"] = action;
    request["data"] = data;

    QJsonDocument doc(request);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact) + "\n";

    socket->write(bytes);
    socket->flush();
}

void NetworkManager::onReadyRead()
{
    buffer.append(socket->readAll());

    while (buffer.contains('\n')) {
        int index = buffer.indexOf('\n');
        QByteArray line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        if (line.isEmpty()) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON Parse error on client side:" << parseError.errorString();
            continue;
        }

        if (doc.isObject()) {
            QJsonObject response = doc.object();
            emit rawResponseReceived(response);

            QString action = response.value("action").toString();
            QString status = response.value("status").toString();
            QJsonObject data = response.value("data").toObject();
            if (action == "newNotification") {
                emit notificationReceived(data);
            } else {
                emit responseReceived(action, status, data);
            }
        }
    }
}

void NetworkManager::onConnected()
{
    emit connectionChanged(true);
}

void NetworkManager::onDisconnected()
{
    emit connectionChanged(false);
}

void NetworkManager::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorOccurred(socket->errorString());
}
