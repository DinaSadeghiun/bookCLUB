#include "bookclubserver.h"
#include <QJsonArray>
#include <QDebug>

// Services
#include "Services/userservice.h"
#include "Services/bookservice.h"
#include "Services/commentservice.h"
#include "Services/shoppingcartservice.h"
#include "Services/orderservice.h"
#include "Services/publisherservice.h"
#include "Services/adminservice.h"
#include "Services/personallibraryservice.h"
#include "Services/notificationservice.h"

// Repositories & DB
#include "DB/databasemanager.h"
#include "DB/userrepository.h"
#include "DB/bookrepository.h"
#include "DB/commentrepository.h"
#include "DB/shoppingcartrepository.h"
#include "DB/orderrepository.h"
#include "DB/publisherrepository.h"
#include "DB/adminrepository.h"
#include "DB/personallibraryrepository.h"
#include "DB/notificationrepository.h"
#include "DB/discountrepository.h"

BookClubServer::BookClubServer(QObject* parent)
    : QTcpServer(parent) {
    initializeServices();
}

BookClubServer::~BookClubServer() {
    qDeleteAll(clients);
}

void BookClubServer::initializeServices() {
    DatabaseManager* db = &DatabaseManager::instance();
    db->initDatabase("bookclub.db");

    auto* userRepo         = new UserRepository(db);
    auto* bookRepo         = new BookRepository(db);
    auto* commentRepo      = new CommentRepository(db);
    auto* orderRepo        = new OrderRepository(db);
    auto* libraryRepo      = new PersonalLibraryRepository(db);
    auto* notificationRepo = new NotificationRepository(db);
    auto* discountRepo     = new DiscountRepository(db);
    auto* cartRepo         = new ShoppingCartRepository(db);
    auto* pubRepo          = new PublisherRepository(db);
    auto* adminRepo        = new AdminRepository(db);

    userService            = new UserService(userRepo, this);
    bookService            = new BookService(bookRepo, discountRepo, this);
    commentService         = new CommentService(commentRepo, bookRepo, this);
    shoppingCartService    = new ShoppingCartService(cartRepo, bookService, this);
    orderService           = new OrderService(orderRepo, shoppingCartService, userService, libraryRepo, this);
    publisherService       = new PublisherService(pubRepo, bookRepo, discountRepo, this);
    adminService           = new AdminService(adminRepo, userRepo, pubRepo, bookRepo, commentRepo, this);
    personalLibraryService = new PersonalLibraryService(libraryRepo, bookRepo, this);
    notificationService    = new NotificationService(notificationRepo, this);

    connect(notificationService, &NotificationService::notificationReceived,
            this, &BookClubServer::onNotificationReceived);
}

bool BookClubServer::startServer(quint16 port) {
    if (!this->listen(QHostAddress::Any, port)) {
        qWarning() << "Server failed to listen on port" << port;
        return false;
    }
    qDebug() << "BookClub Server listening on port" << port;
    return true;
}

void BookClubServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* socket = new QTcpSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        ClientHandler* handler = new ClientHandler(socket, this);
        clients.append(handler);

        connect(handler, &ClientHandler::requestReceived, this, &BookClubServer::onRequestReceived);
        connect(handler, &ClientHandler::disconnected, this, &BookClubServer::onClientDisconnected);
        qDebug() << "New connection setup. Descriptor:" << socketDescriptor;
    } else {
        delete socket;
    }
}

void BookClubServer::onClientDisconnected(ClientHandler* handler) {
    clients.removeAll(handler);
    qDebug() << "Client connection closed.";
}

void BookClubServer::onRequestReceived(ClientHandler* handler, const QJsonObject& request) {
    routeRequest(handler, request);
}

void BookClubServer::routeRequest(ClientHandler* handler, const QJsonObject& request) {
    QString action = request.value("action").toString();
    QJsonObject data = request.value("data").toObject();

    // Auth
    if (action == "login" || action == "register" ||
        action == "loginPublisher" || action == "registerPublisher" ||
        action == "loginAdmin" || action == "registerAdmin" ||
        action == "resetPassword") {
        handleAuth(handler, action, data);
    }
    // Books
    else if (action == "getAllBooks" || action == "addBook" ||
             action == "updateBookPrice" || action == "removeBook" ||
             action == "getBookDetails" || action == "searchBooks") {
        handleBooks(handler, action, data);
    }
    // Comments
    else if (action == "addComment" || action == "getComments" || action == "removeComment") {
        handleComments(handler, action, data);
    }
    // Cart & Orders
    else if (action == "addToCart" || action == "removeFromCart" ||
             action == "getCart" || action == "checkout" || action == "getOrderHistory") {
        handleCartAndOrder(handler, action, data);
    }
    // Personal Library
    else if (action == "getWishlist" || action == "addToWishlist" ||
             action == "removeFromWishlist" || action == "getPurchasedBooks" ||
             // Shelves
             action == "createShelf" || action == "deleteShelf" ||
             action == "addBookToShelf" || action == "removeBookFromShelf" ||
             action == "getBooksInShelf" || action == "getShelfNames") {
        handlePersonalLibrary(handler, action, data);
    }
    // Admin Actions
    else if (action == "getAllUsers" || action == "getAllPublishers" ||
             action == "blockUser" || action == "unblockUser" ||
             action == "deleteUserByAdmin" || action == "adminRemoveComment") {
        handleAdminActions(handler, action, data);
    }
    else {
        QJsonObject response;
        response["action"] = action;
        response["status"] = "error";
        response["message"] = "Action not supported";
        handler->sendResponse(response);
    }
}

void BookClubServer::handleAuth(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "login") {
        QString u = data.value("username").toString();
        QString p = data.value("password").toString();
        auto result = userService->loginUser(u, p);
        if (result.has_value()) {
            handler->setUserId(result->getId());
            response["status"] = "success";
            QJsonObject userDetails;
            userDetails["userId"] = result->getId();
            userDetails["username"] = result->getUsername();
            userDetails["role"] = result->getRole();
            userDetails["balance"] = result->getWalletBalance();
            response["data"] = userDetails;
        } else {
            response["status"] = "error";
            response["message"] = "Invalid username or password";
        }
    }
    else if (action == "register") {
        QString u = data.value("username").toString();
        QString p = data.value("password").toString();
        QString sa = data.value("securityAnswer").toString();
        double balance = data.value("initialBalance").toDouble(0.0);

        QString resMsg = userService->registerUser(u, p, sa, balance);
        if (resMsg.contains("success", Qt::CaseInsensitive) || resMsg.isEmpty()) {
            response["status"] = "success";
            response["message"] = "Registration completed successfully";
        } else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
    }
    else if (action == "loginPublisher") {
        QString u = data.value("username").toString();
        QString p = data.value("password").toString();
        auto result = publisherService->login(u, p);
        if (result.has_value()) {
            handler->setUserId(result->getId());
            response["status"] = "success";
            QJsonObject pubDetails;
            pubDetails["publisherId"] = result->getId();
            pubDetails["username"] = result->getUsername();
            pubDetails["companyName"] = result->getCompanyName();
            pubDetails["role"] = result->getRole();
            response["data"] = pubDetails;
        } else {
            response["status"] = "error";
            response["message"] = "Invalid credentials";
        }
    }
    else if (action == "registerPublisher") {
        QString u = data.value("username").toString();
        QString p = data.value("password").toString();
        QString comp = data.value("companyName").toString();
        QString sa = data.value("securityAnswer").toString("1");

        QString resMsg = publisherService->registerPublisher(u, p, comp, sa);
        if (resMsg.contains("success", Qt::CaseInsensitive) || resMsg.isEmpty()) {
            response["status"] = "success";
        } else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
    }
    else if (action == "loginAdmin") {
        QString u = data.value("username").toString();
        QString p = data.value("password").toString();
        auto result = adminService->loginAdmin(u, p);
        if (result.has_value()) {
            handler->setUserId(result->getId());
            response["status"] = "success";
            QJsonObject adminDetails;
            adminDetails["adminId"] = result->getId();
            adminDetails["username"] = result->getUsername();
            adminDetails["role"] = result->getRole();
            response["data"] = adminDetails;
        } else {
            response["status"] = "error";
            response["message"] = "Invalid admin credentials";
        }
    }
    else if (action == "resetPassword") {
        QString u = data.value("username").toString();
        QString sa = data.value("securityAnswer").toString();
        QString newP = data.value("newPassword").toString();
        bool success = userService->resetPasswordWithSecurityAnswer(u, sa, newP);
        response["status"] = success ? "success" : "error";
    }

    handler->sendResponse(response);
}

void BookClubServer::handleBooks(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "getAllBooks") {
        QList<Book> books = bookService->getAllAvailableBooks();
        QJsonArray booksArr;
        for (const Book& b : std::as_const(books)) {
            QJsonObject bo;
            bo["id"] = b.getId();
            bo["title"] = b.getTitle();
            bo["author"] = b.getAuthor();
            bo["price"] = b.getPrice();
            bo["publisherId"] = b.getPublisherId();
            bo["averageRating"] = b.getAverageRating();
            booksArr.append(bo);
        }
        response["status"] = "success";
        response["data"] = booksArr;
    }
    else if (action == "addBook") {
        int pubId = data.value("publisherId").toInt();
        QString title = data.value("title").toString();
        QString author = data.value("author").toString();
        double price = data.value("price").toDouble();
        Genre genre = static_cast<Genre>(data.value("genre").toInt(0));

        Book book(title, author, price, genre, pubId);
        bool success = publisherService->addNewBook(pubId, book);
        if (success) {
            response["status"] = "success";
            response["bookId"] = book.getId();
        } else {
            response["status"] = "error";
            response["message"] = "Failed to add book";
        }
    }
    else if (action == "updateBookPrice") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        double price = data.value("price").toDouble();
        bool success = publisherService->updateBookPrice(pubId, bookId, price);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "removeBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        bool success = publisherService->removeBook(pubId, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "getBookDetails") {
        int bookId = data.value("bookId").toInt();
        auto bookOpt = bookService->getBookById(bookId);
        if (bookOpt.has_value()) {
            response["status"] = "success";
            QJsonObject bo;
            bo["id"] = bookOpt->getId();
            bo["title"] = bookOpt->getTitle();
            bo["author"] = bookOpt->getAuthor();
            bo["price"] = bookOpt->getPrice();
            bo["publisherId"] = bookOpt->getPublisherId();
            bo["averageRating"] = bookOpt->getAverageRating();
            bo["genre"] = static_cast<int>(bookOpt->getGenre());
            response["data"] = bo;
        } else {
            response["status"] = "error";
            response["message"] = "Book not found";
        }
    }
    else if (action == "searchBooks") {
        QString query = data.value("query").toString();
        QList<Book> found = bookService->search(query);
        QJsonArray booksArr;
        for (const Book& b : std::as_const(found)) {
            QJsonObject bo;
            bo["id"] = b.getId();
            bo["title"] = b.getTitle();
            bo["author"] = b.getAuthor();
            bo["price"] = b.getPrice();
            booksArr.append(bo);
        }
        response["status"] = "success";
        response["data"] = booksArr;
    }

    handler->sendResponse(response);
}

void BookClubServer::handleComments(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "addComment") {
        int userId = data.value("userId").toInt();
        int bookId = data.value("bookId").toInt();
        QString text = data.value("text").toString();
        double rating = data.value("rating").toDouble();

        Comment comment(userId, bookId, text, rating);
        bool success = commentService->addComment(comment);
        if (success) {
            response["status"] = "success";
        } else {
            response["status"] = "error";
        }
    }
    else if (action == "getComments") {
        int bookId = data.value("bookId").toInt();
        QList<Comment> list = commentService->getCommentsByBook(bookId);
        QJsonArray arr;
        for (const Comment& c : std::as_const(list)) {
            QJsonObject co;
            co["id"] = c.getId();
            co["userId"] = c.getUserId();
            co["text"] = c.getText();
            co["rating"] = c.getRating();
            arr.append(co);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "removeComment") {
        int commentId = data.value("commentId").toInt();
        bool success = commentService->removeComment(commentId);
        response["status"] = success ? "success" : "error";
    }

    handler->sendResponse(response);
}

void BookClubServer::handleCartAndOrder(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "addToCart") {
        int userId = data.value("userId").toInt();
        int bookId = data.value("bookId").toInt();
        bool success = shoppingCartService->addBookToCart(userId, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "removeFromCart") {
        int userId = data.value("userId").toInt();
        int bookId = data.value("bookId").toInt();
        bool success = shoppingCartService->removeBookFromCart(userId, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "getCart") {
        int userId = data.value("userId").toInt();
        CartDetails details = shoppingCartService->getCartDetails(userId);

        QJsonObject detailsJson;
        detailsJson["itemsCount"] = details.itemsCount;
        detailsJson["rawTotalPrice"] = details.rawTotalPrice;
        detailsJson["totalDiscountAmount"] = details.totalDiscountAmount;
        detailsJson["finalPriceToPay"] = details.finalPriceToPay;

        QJsonArray itemsArr;
        for (int bookId : std::as_const(details.bookIds)) {
            itemsArr.append(bookId);
        }
        detailsJson["bookIds"] = itemsArr;

        response["status"] = "success";
        response["data"] = detailsJson;
    }
    else if (action == "checkout") {
        int userId = data.value("userId").toInt();
        bool success = orderService->checkout(userId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "getOrderHistory") {
        int userId = data.value("userId").toInt();
        QList<Order> orders = orderService->getOrderHistory(userId);
        QJsonArray ordersArr;
        for (const Order& o : std::as_const(orders)) {
            QJsonObject oo;
            oo["orderId"] = o.getId();
            oo["userId"] = o.getUserId();
            oo["totalPrice"] = o.getFinalPrice();
            oo["orderDate"] = o.getOrderDate().toString(Qt::ISODate);

            QJsonArray booksInOrder;
            for (int bid : o.getBookIds()) {
                booksInOrder.append(bid);
            }
            oo["bookIds"] = booksInOrder;
            ordersArr.append(oo);
        }
        response["status"] = "success";
        response["data"] = ordersArr;
    }

    handler->sendResponse(response);
}

void BookClubServer::handlePersonalLibrary(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    int userId = data.value("userId").toInt();

    if (action == "getWishlist") {
        QList<int> wishlist = personalLibraryService->getWishlist(userId);
        QJsonArray arr;
        for (int bid : std::as_const(wishlist)) arr.append(bid);
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "addToWishlist") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->addToWishlist(userId, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "removeFromWishlist") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->removeFromWishlist(userId, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "getPurchasedBooks") {
        QList<int> purchased = personalLibraryService->getPurchasedBooks(userId);
        QJsonArray arr;
        for (int bid : std::as_const(purchased)) arr.append(bid);
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "createShelf") {
        QString name = data.value("shelfName").toString();
        bool success = personalLibraryService->createShelf(userId, name);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "deleteShelf") {
        QString name = data.value("shelfName").toString();
        bool success = personalLibraryService->deleteShelf(userId, name);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "addBookToShelf") {
        QString name = data.value("shelfName").toString();
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->addBookToShelf(userId, name, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "removeBookFromShelf") {
        QString name = data.value("shelfName").toString();
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->removeBookFromShelf(userId, name, bookId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "getBooksInShelf") {
        QString name = data.value("shelfName").toString();
        QList<int> books = personalLibraryService->getBooksInShelf(userId, name);
        QJsonArray arr;
        for (int bid : std::as_const(books)) arr.append(bid);
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "getShelfNames") {
        QList<QString> shelves = personalLibraryService->getShelfNames(userId);
        QJsonArray arr;
        for (const QString& s : std::as_const(shelves)) arr.append(s);
        response["status"] = "success";
        response["data"] = arr;
    }

    handler->sendResponse(response);
}

void BookClubServer::handleAdminActions(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "getAllUsers") {
        QList<User> list = adminService->getAllUsers();
        QJsonArray arr;
        for (const User& u : std::as_const(list)) {
            QJsonObject uo;
            uo["id"] = u.getId();
            uo["username"] = u.getUsername();
            uo["role"] = u.getRole();
            uo["isBlocked"] = !u.getIsActive();
            arr.append(uo);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "getAllPublishers") {
        QList<Publisher> list = adminService->getAllPublishers();
        QJsonArray arr;
        for (const Publisher& p : std::as_const(list)) {
            QJsonObject po;
            po["id"] = p.getId();
            po["username"] = p.getUsername();
            po["companyName"] = p.getCompanyName();
            arr.append(po);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "blockUser") {
        int targetId = data.value("targetId").toInt();
        bool success = adminService->blockUser(targetId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "unblockUser") {
        int targetId = data.value("targetId").toInt();
        bool success = adminService->unblockUser(targetId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "deleteUserByAdmin") {
        int targetId = data.value("targetId").toInt();
        bool success = adminService->deleteUserAccount(targetId);
        response["status"] = success ? "success" : "error";
    }
    else if (action == "adminRemoveComment") {
        int commentId = data.value("commentId").toInt();
        bool success = adminService->removeCommentByAdmin(commentId);
        response["status"] = success ? "success" : "error";
    }

    handler->sendResponse(response);
}

void BookClubServer::onNotificationReceived(int recipientId, const Notification& notification) {
    QJsonObject response;
    response["action"] = "newNotification";

    QJsonObject notificationDetails;
    notificationDetails["id"] = notification.getId();
    notificationDetails["message"] = notification.getMessage();
    notificationDetails["type"] = static_cast<int>(notification.getType());
    notificationDetails["relatedBookId"] = notification.getRelatedBookId();
    notificationDetails["createdAt"] = notification.getCreatedAt().toString(Qt::ISODate);

    response["data"] = notificationDetails;

    for (ClientHandler* client : std::as_const(clients)) {
        if (client->getUserId() == recipientId) {
            client->sendResponse(response);
            break;
        }
    }
}
