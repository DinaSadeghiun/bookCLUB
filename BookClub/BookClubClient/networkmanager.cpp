#include "networkmanager.h"
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>


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
                                  const QVariantList& favoriteGenres)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    data["securityAnswer"] = securityAnswer;

    QJsonArray genresArray;
    for (const QVariant& genre : favoriteGenres) {
        genresArray.append(genre.toInt());
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

void NetworkManager::registerPublisher(const QString& username, const QString& password, const QString& securityAnswer)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    data["securityAnswer"] = securityAnswer;
    sendRequest("registerPublisher", data);
}
void NetworkManager::changePassword(int id, const QString& oldPassword, const QString& newPassword, const QString& role)
{
    QJsonObject data;
    data["id"] = id;
    data["oldPassword"] = oldPassword;
    data["newPassword"] = newPassword;
    data["role"] = role;
    sendRequest("changePassword", data);
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

void NetworkManager::updateProfile(int userId, const QVariantList& favoriteGenres)
{
    QJsonObject data;
    data["userId"] = userId;

    QJsonArray genresArray;
    for (const QVariant& genre : favoriteGenres) {
        genresArray.append(genre.toInt());
    }
    data["favoriteGenres"] = genresArray;

    sendRequest("updateProfile", data);
}

void NetworkManager::updateSecurityAnswer(int userId, const QString& securityAnswer, const QString& password, const QString& role)
{
    QJsonObject data;
    data["id"] = userId;
    data["securityAnswer"] = securityAnswer;
    data["password"] = password;
    data["role"] = role;
    sendRequest("updateSecurityAnswer", data);
}
void NetworkManager::checkUsernameExists(const QString& username)
{
    QJsonObject data;
    data["username"] = username.trimmed();
    sendRequest("checkUsernameExists", data);
}

void NetworkManager::verifySecurityAnswer(const QString& username, const QString& role, const QString& answer)
{
    QJsonObject data;
    data["username"] = username.trimmed();
    data["role"] = role.toLower();
    data["securityAnswer"] = answer.trimmed();
    sendRequest("verifySecurityAnswer", data);
}
// Books Operations
void NetworkManager::getAllBooks()
{
    qDebug() << "Client: Attempting to send getAllBooks request...";
    sendRequest("getAllBooks", QJsonObject());
}
QByteArray NetworkManager::readFileAsBase64(const QString& filePath)
{
    if (filePath.isEmpty())
        return QByteArray();

    QString localPath = filePath;
    if (localPath.startsWith("file:")) {
        localPath = QUrl(filePath).toLocalFile();
    }

    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << localPath;
        return QByteArray();
    }

    return file.readAll().toBase64();
}
void NetworkManager::addBook(int publisherId, const QString& title, const QString& author,
                             double price, const QString& genre, const QString& description,
                             const QString& coverPath, const QString& pdfPath) {

    QString coverBase64 = QString::fromUtf8(readFileAsBase64(coverPath));
    QString pdfBase64 = QString::fromUtf8(readFileAsBase64(pdfPath));

    QJsonObject data;
    data["publisherId"] = publisherId;
    data["title"] = title;
    data["author"] = author;
    data["price"] = price;
    data["genre"] = genre;
    data["description"] = description;

    data["coverImage"] = coverBase64;
    data["pdfFile"] = pdfBase64;

    sendRequest("addBook", data);
}
void NetworkManager::getBookPdf(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("getBookPdf", data);
}

QString NetworkManager::saveBase64ToCache(const QString& base64Data, const QString& fileName)
{
    if (base64Data.isEmpty() || fileName.isEmpty()) {
        emit errorOccurred("Base64 data or file name is empty.");
        return QString();
    }

    QByteArray fileData = QByteArray::fromBase64(base64Data.toUtf8());
    if (fileData.isEmpty()) {
        emit errorOccurred("Failed to decode base64 data.");
        return QString();
    }

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheDir.isEmpty()) {
        emit errorOccurred("Cache directory is not available.");
        return QString();
    }

    QDir dir;
    if (!dir.mkpath(cacheDir)) {
        emit errorOccurred("Failed to create cache directory.");
        return QString();
    }

    QString fullPath = cacheDir + "/" + fileName;

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit errorOccurred("Failed to save file: " + fullPath);
        return QString();
    }

    file.write(fileData);
    file.close();

    return fullPath;
}

void NetworkManager::getBooksByGenre(int genreId) {
    QJsonObject data;
    data["genreId"] = genreId;
    sendRequest("getBooksByGenre", data);}
void NetworkManager::updateBookDetails(int bookId, const QString& title, const QString& author,
                                       double price, const QString& genre, const QString& description,
                                       const QString& coverPath, const QString& pdfPath,
                                       const QString& role, int publisherId)
{
    QJsonObject data;
    data["id"] = bookId;
    data["title"] = title;
    data["author"] = author;
    data["price"] = price;
    data["genre"] = genre;
    data["description"] = description;
    data["role"] = role;

    if (publisherId > 0) {
        data["publisherId"] = publisherId;
    }

    // ===== تبدیل فایل‌ها به Base64 (مثل addBook) =====
    QString coverBase64 = QString::fromUtf8(readFileAsBase64(coverPath));
    QString pdfBase64 = QString::fromUtf8(readFileAsBase64(pdfPath));

    data["coverImage"] = coverBase64;
    data["pdfFile"] = pdfBase64;

    sendRequest("updateBookDetails", data);
}

void NetworkManager::getPublisherBooks(int publisherId) {
    QJsonObject data;
    data["publisherId"] = publisherId;
    sendRequest("getPublisherBooks", data);
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

void NetworkManager::deactivateBook(int publisherId, int bookId) {
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    sendRequest("deactivateBook", data);
}

void NetworkManager::activateBook(int publisherId, int bookId) {
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    sendRequest("activateBook", data);
}

void NetworkManager::getBookDetails(int userId, int bookId)
{
    QJsonObject data;
    data["userId"] = userId;
    data["bookId"] = bookId;
    sendRequest("getBookDetails", data);
}


void NetworkManager::searchBooks(const QString& query)
{
    QJsonObject data;
    data["query"] = query;
    sendRequest("searchBooks", data);
}
void NetworkManager::getHomeData(int userId) {
    QJsonObject data;
    data["userId"] = userId;
    sendRequest("getHomeData", data);
}
// Discount
void NetworkManager::applyDiscountToBook(int publisherId, int bookId, double value, int type, qint64 startDate, qint64 endDate)
{
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    data["value"] = value;
    data["type"] = type;  // 0 = Percentage, 1 = FixedAmount
    data["startDate"] = startDate;
    data["endDate"] = endDate;
    sendRequest("applyDiscountToBook", data);
}

void NetworkManager::removeDiscountFromBook(int publisherId, int bookId)
{
    QJsonObject data;
    data["publisherId"] = publisherId;
    data["bookId"] = bookId;
    sendRequest("removeDiscountFromBook", data);
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

        // --- این خط را برای دیباگ اضافه کنید ---
        qDebug() << "RAW SERVER RESPONSE LINE:" << line;
        // ------------------------------------

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
            QVariant data = response.value("data").toVariant();   // <-- به‌جای .toObject()

            qDebug() << "Parsed Response -> Action:" << action << "Status:" << status;

            if (action == "newNotification") {
                emit notificationReceived(data.toJsonObject());   // این یکی مطمئنیم همیشه object هست
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
