#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>
#include <QAbstractSocket>

class NetworkManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged);

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    bool isConnected() const;

    Q_INVOKABLE void connectToServer(const QString& host, quint16 port);
    Q_INVOKABLE void disconnectFromServer();

    // Auth
    Q_INVOKABLE void login(const QString& username, const QString& password);
    Q_INVOKABLE void registerUser(const QString& username, const QString& password, const QString& securityAnswer, const QVariantList& favoriteGenres);
    Q_INVOKABLE void loginPublisher(const QString& username, const QString& password);
    Q_INVOKABLE void registerPublisher(const QString& username, const QString& password, const QString& securityAnswer);
    Q_INVOKABLE void loginAdmin(const QString& username, const QString& password);
    Q_INVOKABLE void changeUsername(int id, const QString& newUsername, const QString& password, const QString& role);
    Q_INVOKABLE void resetPassword(const QString& username, const QString& securityAnswer, const QString& newPassword);
    Q_INVOKABLE void updateProfile(int userId, const QVariantList& favoriteGenres);
    Q_INVOKABLE void updateSecurityAnswer(int userId, const QString& securityAnswer, const QString& password, const QString& role);
    Q_INVOKABLE void changePassword(int id, const QString& oldPassword, const QString& newPassword, const QString& role);
    Q_INVOKABLE void checkUsernameExists(const QString& username);
    Q_INVOKABLE  void verifySecurityAnswer(const QString& username, const QString& role, const QString& answer);
    // Books Operations
    Q_INVOKABLE void getReadingProgress(int userId, int bookId);
    Q_INVOKABLE void saveReadingProgress(int userId, int bookId, int page);
    Q_INVOKABLE void getBookPdf(int userId, int bookId);
    Q_INVOKABLE QString saveBase64ToCache(const QString& base64Data, const QString& fileName);
    Q_INVOKABLE void getBooksByGenre(int genreId);
    Q_INVOKABLE void getAllBooks();
    Q_INVOKABLE void addBook(int publisherId, const QString& title, const QString& author,
                             double price, const QString& genre, const QString& description,
                             const QString& coverPath, const QString& pdfPath);
    Q_INVOKABLE void updateBookDetails(int bookId, const QString& title, const QString& author,
                                       double price, const QString& genre, const QString& description,
                                       const QString& coverImagePath, const QString& pdfFilePath,
                                       const QString& role, int publisherId = -1);
    Q_INVOKABLE void getPublisherBooks(int publisherId);
    Q_INVOKABLE void getSalesStats(int publisherId);
    Q_INVOKABLE void updateBookPrice(int publisherId, int bookId, double price);
    Q_INVOKABLE void removeBook(int publisherId, int bookId);
    Q_INVOKABLE void deactivateBook(int publisherId, int bookId);
    Q_INVOKABLE void activateBook(int publisherId, int bookId);
    Q_INVOKABLE void getBookDetails(int userId, int bookId);

    Q_INVOKABLE void searchBooks(const QString& query);
    Q_INVOKABLE void getHomeData(int userId);

    // Discount
    Q_INVOKABLE void applyDiscountToBook(int publisherId, int bookId, double value, int type, qint64 startDate, qint64 endDate);
    Q_INVOKABLE void removeDiscountFromBook(int publisherId, int bookId);

    // Comments
    Q_INVOKABLE void addComment(int userId, int bookId, const QString& text, double rating);
    Q_INVOKABLE void getComments(int bookId);
    Q_INVOKABLE void removeComment(int commentId);
    Q_INVOKABLE void editComment(int commentId, int userId, int bookId, const QString& text, double rating);

    // Cart & Orders
    Q_INVOKABLE void addToCart(int userId, int bookId);
    Q_INVOKABLE void removeFromCart(int userId, int bookId);
    Q_INVOKABLE void getCart(int userId);
    Q_INVOKABLE void checkout(int userId);
    Q_INVOKABLE void getOrderHistory(int userId);

    // Personal Library
    Q_INVOKABLE void getWishlist(int userId);
    Q_INVOKABLE void addToWishlist(int userId, int bookId);
    Q_INVOKABLE void removeFromWishlist(int userId, int bookId);
    Q_INVOKABLE void getFavorites(int userId);
    Q_INVOKABLE void addToFavorites(int userId, int bookId);
    Q_INVOKABLE void removeFromFavorites(int userId, int bookId);
    Q_INVOKABLE void isInFavorites(int userId, int bookId);
    Q_INVOKABLE void getPurchasedBooks(int userId);
    Q_INVOKABLE void createShelf(int userId, const QString& shelfName);
    Q_INVOKABLE void deleteShelf(int userId, const QString& shelfName);
    Q_INVOKABLE void addBookToShelf(int userId, const QString& shelfName, int bookId);
    Q_INVOKABLE void removeBookFromShelf(int userId, const QString& shelfName, int bookId);
    Q_INVOKABLE void getBooksInShelf(int userId, const QString& shelfName);
    Q_INVOKABLE void getShelfNames(int userId);
    Q_INVOKABLE void moveBookBetweenShelves(int userId, int bookId, const QString& fromShelf, const QString& toShelf);

    // Admin Actions
    Q_INVOKABLE void getAllUsers();
    Q_INVOKABLE void getAllPublishers();
    Q_INVOKABLE void blockUser(int targetId);
    Q_INVOKABLE void unblockUser(int targetId);
    Q_INVOKABLE void deleteUserByAdmin(int targetId);
    Q_INVOKABLE void adminRemoveComment(int commentId);
    Q_INVOKABLE void searchUsers(const QString& query);
    Q_INVOKABLE void searchPublishers(const QString& query);

    // Notification Actions
    Q_INVOKABLE void getNotifications(int recipientId);
    Q_INVOKABLE void markNotificationAsRead(int notificationId, int recipientId);
    Q_INVOKABLE void markAllNotificationsAsRead(int recipientId);
    Q_INVOKABLE void deleteNotification(int notificationId, int recipientId);

signals:
    void connectionChanged(bool isConnected);
    void errorOccurred(const QString& errorMessage);
    void responseReceived(const QString& action, const QString& status, const QVariant& data);
    void rawResponseReceived(const QJsonObject& response);
    void notificationReceived(const QJsonObject& data);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    void sendRequest(const QString& action, const QJsonObject& data);
    QByteArray readFileAsBase64(const QString& filePath);

    QTcpSocket* socket;
    QByteArray buffer;
};

#endif // NETWORKMANAGER_H
