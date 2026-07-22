#include "bookclubserver.h"
#include <QJsonArray>
#include <QDebug>
#include "modelserializer.h"
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
    publisherService       = new PublisherService(pubRepo, bookRepo, discountRepo, this);
    orderService           = new OrderService(orderRepo, shoppingCartService,
                                    userService, libraryRepo, bookRepo, publisherService, this);
    adminService           = new AdminService(adminRepo, userRepo, pubRepo, bookRepo, commentRepo, this);
    personalLibraryService = new PersonalLibraryService(libraryRepo, bookRepo, this);
    notificationService    = new NotificationService(notificationRepo, this);

    //Creating default admin
    Admin defaultAdmin("admin", "admin123", "system");
    if (!adminRepo->findByUsername("admin").has_value()) {
        adminRepo->save(defaultAdmin);
        qDebug() << "Default admin created: admin / admin123";
    }

    connect(notificationService, &NotificationService::notificationReceived,
            this, &BookClubServer::onNotificationReceived);

    QList<Book> checkBooks = bookService->getAllAvailableBooks();
    if (checkBooks.isEmpty()) {
        qDebug() << "Database is empty. Injecting test data...";

        Publisher testPub("nasher1", "1234", "Ghoghnoos Pub", "Tehran");
        pubRepo->save(testPub);
        int pubId = testPub.getId();
        if(pubId == -1) pubId = 1;

        Book b1("Modern C++", "Bjarne Stroustrup", 150000, Genre::History, pubId);
        Book b2("The Little Prince", "Antoine de Saint-Exupéry", 85000, Genre::Biography, pubId);

        bookRepo->save(b1);
        bookRepo->save(b2);

        qDebug() << "Test data injected! 1 Publisher and 2 Books added.";
    } else {
        qDebug() << "Database already has" << checkBooks.size() << "books.";
    }


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

    // 1. Auth & Profile
    if (action == "login" || action == "register" ||
        action == "loginPublisher" || action == "registerPublisher" ||
        action == "loginAdmin" || action == "registerAdmin" ||
        action == "resetPassword" || action == "updateProfile" ||
        action == "changePassword" || action == "changeUsername" ||
        action == "updateSecurityAnswer") {
        handleAuth(handler, action, data);
    }
    // 2. Books
    else if (action == "getAllBooks" || action == "addBook" ||
             action == "updateBookPrice" || action == "removeBook" ||
             action == "getBookDetails" || action == "searchBooks") {
        handleBooks(handler, action, data);
    }
    // 3. Comments & Ratings
    else if (action == "addComment" || action == "editComment" ||
             action == "getComments" || action == "removeComment") {
        handleComments(handler, action, data);
    }
    // 4. Cart & Orders
    else if (action == "addToCart" || action == "removeFromCart" ||
             action == "getCart" || action == "checkout" || action == "getOrderHistory") {
        handleCartAndOrder(handler, action, data);
    }
    // 5. Personal Library & Shelves
    else if (action == "getWishlist" || action == "addToWishlist" ||
             action == "removeFromWishlist" || action == "getPurchasedBooks" ||
             action == "getFavorites" || action == "addToFavorites" ||
             action == "removeFromFavorites" || action == "isInFavorites" ||
             action == "createShelf" || action == "deleteShelf" || action == "renameShelf" ||
             action == "addBookToShelf" || action == "removeBookFromShelf" ||
             action == "moveBookBetweenShelves" || action == "getBooksInShelf" ||
             action == "getShelfNames") {
        handlePersonalLibrary(handler, action, data);
    }
    // 6. Admin Actions
    else if (action == "getAllUsers" || action == "getAllPublishers" ||
             action == "searchUsers" || action == "searchPublishers" ||
             action == "unblockUser" || action == "deleteUserByAdmin" ||
             action == "adminRemoveComment" || action == "blockUser") {
        handleAdminActions(handler, action, data);
    }
    // 7. Notifications
    else if (action == "getNotifications" || action == "markNotificationAsRead" ||
             action == "markAllNotificationsAsRead" || action == "deleteNotification") {
        handleNotifications(handler, action, data);
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
        QString username = data["username"].toString().trimmed();
        QString password = data["password"].toString().trimmed();

        // Admin
        auto optAdmin = adminService->loginAdmin(username, password);
        if (optAdmin.has_value()) {
            response["status"] = "success";
            response["role"] = "Admin";
            QJsonObject adminData;
            adminData["userId"] = optAdmin->getId();
            adminData["username"] = optAdmin->getUsername();
            adminData["role"] = "Admin";
            response["data"] = adminData;
            handler->setUserId(optAdmin->getId());
            handler->sendResponse(response);
            return;
        }

        // Publisher
        auto optPub = publisherService->login(username, password);
        if (optPub.has_value()) {
            if (!optPub->getIsActive()) {
                response["status"] = "error";
                response["message"] = "Your publisher account has been blocked by the admin";
            } else {
                response["status"] = "success";
                response["role"] = "Publisher";
                QJsonObject pubData;
                pubData["userId"] = optPub->getId();
                pubData["username"] = optPub->getUsername();
                pubData["companyName"] = optPub->getCompanyName();
                pubData["role"] = "Publisher";
                response["data"] = pubData;
                handler->setUserId(optPub->getId());
            }
            handler->sendResponse(response);
            return;
        }

        // User
        auto optUser = userService->loginUser(username, password);
        if (optUser.has_value()) {
            User user = optUser.value();
            if (!user.getIsActive()) {
                response["status"] = "error";
                response["message"] = "Your account has been blocked by the admin";
            } else {
                response["status"] = "success";
                response["role"] = "User";
                QJsonObject userData;
                userData["userId"] = user.getId();
                userData["username"] = user.getUsername();
                userData["role"] = "User";
                userData["balance"] = user.getWalletBalance();
                response["data"] = userData;
                handler->setUserId(user.getId());
            }
            handler->sendResponse(response);
            return;
        }

        response["status"] = "error";
        response["message"] = "Invalid username or password";
        handler->sendResponse(response);
        return;
    }

    else if (action == "register") {
        QString u = data.value("username").toString().trimmed();
        QString p = data.value("password").toString();
        QString sa = data.value("securityAnswer").toString().trimmed();
        double balance = data.value("initialBalance").toDouble(0.0);

        QList<Genre> favoriteGenres;
        QJsonArray genresArray = data.value("favoriteGenres").toArray();
        for (const auto& val : std::as_const(genresArray)) {
            favoriteGenres.append(static_cast<Genre>(val.toInt()));
        }

        QString resMsg = userService->registerUser(u, p, sa, balance, favoriteGenres);

        if (resMsg == "SUCCESS") {
            response["status"] = "success";
            response["message"] = "Registration completed successfully";

            auto userOpt = userService->getUserByUsername(u);
            if (userOpt.has_value()) {
                response["userId"] = userOpt->getId();
            }
        }  else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
        handler->sendResponse(response);
        return;
    }
}

void BookClubServer::handleBooks(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "getAllBooks") {
        QList<Book> books = bookService->getAllAvailableBooks();
        qDebug() << "Server: Found" << books.size() << "books in database.";

        response["status"] = "success";
        response["data"] = ModelSerializer::serializeBookList(books);
    }
    else if (action == "addBook") {
        int pubId = data.value("publisherId").toInt();
        Book book = ModelSerializer::deserializeBook(data);
        book.setPublisherId(pubId);

        bool success = publisherService->addNewBook(pubId, book);
        if (success) {
            response["status"] = "success";
            response["bookId"] = book.getId();
            response["message"] = "Book added successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to add book. Ensure all fields are valid.";
        }
    }
    else if (action == "updateBookPrice") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        double price = data.value("price").toDouble();

        bool success = publisherService->updateBookPrice(pubId, bookId, price);
        if (success) {
            response["status"] = "success";
            response["message"] = "Book price updated successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to update book price. Owner verification failed or invalid price.";
        }
    }
    else if (action == "removeBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();

        bool success = publisherService->removeBook(pubId, bookId);
        if (success) {
            response["status"] = "success";
            response["message"] = "Book removed successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to remove book. Owner verification failed.";
        }
    }
    else if (action == "getBookDetails") {
        int bookId = data.value("bookId").toInt();
        auto bookOpt = bookService->getBookById(bookId);
        if (bookOpt.has_value()) {
            response["status"] = "success";
            response["data"] = ModelSerializer::serializeBook(*bookOpt);
        } else {
            response["status"] = "error";
            response["message"] = "Book not found";
        }
    }
    else if (action == "searchBooks") {
        QString query = data.value("query").toString().trimmed();
        QList<Book> found = bookService->search(query);

        response["status"] = "success";
        response["data"] = ModelSerializer::serializeBookList(found);
    }
    else {
        response["status"] = "error";
        response["message"] = "Unknown book action";
    }

    handler->sendResponse(response);
}


void BookClubServer::handleComments(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    if (action == "addComment") {
        Comment comment = ModelSerializer::deserializeComment(data);
        comment.setId(-1);

        bool success = commentService->addComment(comment);
        if (success) {
            response["status"] = "success";
            response["commentId"] = comment.getId();
            response["message"] = "Comment added successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to add comment. Validation failed.";
        }
    }
    else if (action == "editComment") {
        int commentId = data.value("id").toInt();
        if (commentId <= 0) {
            commentId = data.value("commentId").toInt();
        }

        auto existingOpt = commentService->getCommentById(commentId);
        if (!existingOpt.has_value()) {
            response["status"] = "error";
            response["message"] = "Comment not found";
        } else {
            Comment comment = ModelSerializer::deserializeComment(data);
            comment.setId(commentId);
            comment.setBookId(existingOpt->getBookId());

            bool success = commentService->addComment(comment);
            if (success) {
                response["status"] = "success";
                response["message"] = "Comment updated successfully";
            } else {
                response["status"] = "error";
                response["message"] = "Failed to update comment. Validation failed.";
            }
        }
    }
    else if (action == "getComments") {
        int bookId = data.value("bookId").toInt();
        QList<Comment> list = commentService->getCommentsByBook(bookId);

        response["status"] = "success";
        response["data"] = ModelSerializer::serializeCommentList(list);
    }
    else if (action == "removeComment") {
        int commentId = data.value("commentId").toInt();

        bool success = commentService->removeComment(commentId);
        if (success) {
            response["status"] = "success";
            response["message"] = "Comment removed successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to remove comment. Comment not found or invalid ID.";
        }
    }
    else {
        response["status"] = "error";
        response["message"] = "Unknown comment action";
    }

    handler->sendResponse(response);
}


void BookClubServer::handleCartAndOrder(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    int userId = data.value("userId").toInt();
    if (userId <= 0) {
        response["status"] = "error";
        response["message"] = "Invalid user ID";
        handler->sendResponse(response);
        return;
    }

    if (action == "addToCart") {
        int bookId = data.value("bookId").toInt();
        if (bookId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid book ID";
        } else {
            bool success = shoppingCartService->addBookToCart(userId, bookId);
            if (success) {
                response["status"] = "success";
                response["message"] = "Book added to cart successfully";
            } else {
                response["status"] = "error";
                response["message"] = "Failed to add book to cart";
            }
        }
    }
    else if (action == "removeFromCart") {
        int bookId = data.value("bookId").toInt();
        if (bookId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid book ID";
        } else {
            bool success = shoppingCartService->removeBookFromCart(userId, bookId);
            if (success) {
                response["status"] = "success";
                response["message"] = "Book removed from cart successfully";
            } else {
                response["status"] = "error";
                response["message"] = "Failed to remove book from cart";
            }
        }
    }
    else if (action == "getCart") {
        CartDetails details = shoppingCartService->getCartDetails(userId);

        QJsonObject cartObj;
        QJsonArray booksArr;
        for (int id : std::as_const(details.bookIds)) {
            booksArr.append(id);
        }
        cartObj["bookIds"] = booksArr;
        cartObj["itemsCount"] = details.itemsCount;
        cartObj["rawTotalPrice"] = details.rawTotalPrice;
        cartObj["totalDiscountAmount"] = details.totalDiscountAmount;
        cartObj["finalPriceToPay"] = details.finalPriceToPay;

        response["status"] = "success";
        response["data"] = cartObj;
    }
    else if (action == "checkout") {
        bool success = orderService->checkout(userId);
        if (success) {
            response["status"] = "success";
            response["message"] = "Checkout completed successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Checkout failed (insufficient balance or empty cart)";
        }
    }
    else if (action == "getOrderHistory") {
        QList<Order> orders = orderService->getOrderHistory(userId);
        QJsonArray ordersArr;

        for (const auto& order : std::as_const(orders)) {
            QJsonObject orderObj;
            orderObj["orderId"] = order.getId();
            orderObj["userId"] = order.getUserId();
            orderObj["rawTotalPrice"] = order.getRawPrice();
            orderObj["totalDiscountAmount"] = order.getDiscountAmount();
            orderObj["finalPriceToPay"] = order.getFinalPrice();
            orderObj["orderDate"] = order.getOrderDate().toString(Qt::ISODate);

            QJsonArray bookIdsArr;
            for (int bId : order.getBookIds()) {
                bookIdsArr.append(bId);
            }
            orderObj["bookIds"] = bookIdsArr;

            ordersArr.append(orderObj);
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
    if (userId <= 0) {
        response["status"] = "error";
        response["message"] = "Invalid user ID";
        handler->sendResponse(response);
        return;
    }

    if (action == "getWishlist") {
        QList<int> wishlist = personalLibraryService->getWishlist(userId);
        QJsonArray arr;
        for (int id : std::as_const(wishlist)) {
            arr.append(id);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "addToWishlist") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->addToWishlist(userId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to add book to wishlist";
    }
    else if (action == "removeFromWishlist") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->removeFromWishlist(userId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to remove book from wishlist";
    }
    else if (action == "getFavorites") {
        QList<int> favorites = personalLibraryService->getFaveBooks(userId);
        QJsonArray arr;
        for (int id : std::as_const(favorites)) {
            arr.append(id);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "addToFavorites") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->addToFaveBooks(userId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to add book to favorites (ensure you purchased this book)";
    }
    else if (action == "removeFromFavorites") {
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->removeFromFaveBooks(userId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to remove book from favorites";
    }
    else if (action == "isInFavorites") {
        int bookId = data.value("bookId").toInt();
        bool isFave = personalLibraryService->isInFaveBooks(userId, bookId);
        response["status"] = "success";
        response["isInFavorites"] = isFave;
    }
    else if (action == "getPurchasedBooks") {
        QList<int> purchased = personalLibraryService->getPurchasedBooks(userId);
        QJsonArray arr;
        for (int id : std::as_const(purchased)) {
            arr.append(id);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "createShelf") {
        QString name = data.value("shelfName").toString().trimmed();
        if (name.isEmpty()) {
            response["status"] = "error";
            response["message"] = "Shelf name cannot be empty";
        } else {
            bool success = personalLibraryService->createShelf(userId, name);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to create shelf (it might already exist)";
        }
    }
    else if (action == "deleteShelf") {
        QString name = data.value("shelfName").toString().trimmed();
        bool success = personalLibraryService->deleteShelf(userId, name);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to delete shelf";
    }
    else if (action == "addBookToShelf") {
        QString name = data.value("shelfName").toString().trimmed();
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->addBookToShelf(userId, name, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to add book to shelf";
    }
    else if (action == "removeBookFromShelf") {
        QString name = data.value("shelfName").toString().trimmed();
        int bookId = data.value("bookId").toInt();
        bool success = personalLibraryService->removeBookFromShelf(userId, name, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to remove book from shelf";
    }
    else if (action == "getBooksInShelf") {
        QString name = data.value("shelfName").toString().trimmed();
        QList<int> books = personalLibraryService->getBooksInShelf(userId, name);
        QJsonArray arr;
        for (int id : std::as_const(books)) {
            arr.append(id);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "getShelfNames") {
        QList<QString> shelves = personalLibraryService->getShelfNames(userId);
        QJsonArray arr;
        for (const QString& name : std::as_const(shelves)) {
            arr.append(name);
        }
        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "renameShelf") {
        QString oldName = data.value("oldName").toString().trimmed();
        QString newName = data.value("newName").toString().trimmed();
        if (oldName.isEmpty() || newName.isEmpty()) {
            response["status"] = "error";
            response["message"] = "Old and new shelf names cannot be empty";
        } else {
            bool success = personalLibraryService->renameShelf(userId, oldName, newName);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to rename shelf (ensure old shelf exists and new name is unique)";
        }
    }
    else if (action == "moveBookBetweenShelves") {
        int bookId = data.value("bookId").toInt();
        QString fromShelf = data.value("fromShelf").toString().trimmed();
        QString toShelf = data.value("toShelf").toString().trimmed();

        bool success = personalLibraryService->moveBookBetweenShelves(userId, bookId, fromShelf, toShelf);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to move book between shelves";
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
    else if (action == "searchUsers") {
        QString query = data.value("query").toString().trimmed();
        QList<User> list = adminService->searchUsers(query);
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
    else if (action == "searchPublishers") {
        QString query = data.value("query").toString().trimmed();
        QList<Publisher> list = adminService->searchPublishers(query);
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
        if (targetId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid target ID";
        } else {
            bool success = adminService->blockUser(targetId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to block user or user not found.";
        }
    }
    else if (action == "unblockUser") {
        int targetId = data.value("targetId").toInt();
        if (targetId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid target ID";
        } else {
            bool success = adminService->unblockUser(targetId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to unblock user.";
        }
    }
    else if (action == "deleteUserByAdmin") {
        int targetId = data.value("targetId").toInt();
        if (targetId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid target ID";
        } else {
            bool success = adminService->deleteUserAccount(targetId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to delete user account.";
        }
    }
    else if (action == "adminRemoveComment") {
        int commentId = data.value("commentId").toInt();
        if (commentId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid comment ID";
        } else {
            bool success = adminService->removeCommentByAdmin(commentId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Comment not found or could not be deleted.";
        }
    }

    handler->sendResponse(response);
}

void BookClubServer::handleNotifications(ClientHandler* handler, const QString& action, const QJsonObject& data) {
    QJsonObject response;
    response["action"] = action;

    int recipientId = data.value("recipientId").toInt();
    if (recipientId <= 0) {
        recipientId = handler->getUserId();
    }

    if (recipientId <= 0) {
        response["status"] = "error";
        response["message"] = "Invalid recipient ID";
        handler->sendResponse(response);
        return;
    }

    if (action == "getNotifications") {
        QList<Notification> list = notificationService->getNotificationsForeRecipient(recipientId);
        response["status"] = "success";
        response["data"] = ModelSerializer::serializeNotificationList(list);
    }
    else if (action == "markNotificationAsRead") {
        int notificationId = data.value("notificationId").toInt();
        if (notificationId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid notification ID";
        } else {
            bool success = notificationService->markAsRead(notificationId, recipientId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to mark notification as read";
        }
    }
    else if (action == "markAllNotificationsAsRead") {
        bool success = notificationService->markAllAsRead(recipientId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to mark all notifications as read";
    }
    else if (action == "deleteNotification") {
        int notificationId = data.value("notificationId").toInt();
        if (notificationId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid notification ID";
        } else {
            bool success = notificationService->deleteNotification(notificationId, recipientId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to delete notification";
        }
    }
    else {
        response["status"] = "error";
        response["message"] = "Unknown notification action";
    }

    handler->sendResponse(response);
}


void BookClubServer::onNotificationReceived(int recipientId, const Notification& notification) {
    QJsonObject response;
    response["action"] = "newNotification";
    response["data"] = ModelSerializer::serializeNotification(notification);

    for (ClientHandler* client : std::as_const(clients)) {
        if (client->getUserId() == recipientId) {
            client->sendResponse(response);
            break;
        }
    }
}
