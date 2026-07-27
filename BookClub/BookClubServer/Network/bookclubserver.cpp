#include "bookclubserver.h"
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QUuid>
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

// Helper function for serializing books with discount
QJsonArray serializeBookListWithDiscount(const QList<Book>& books) {
    QJsonArray arr;
    DiscountRepository discountRepo(&DatabaseManager::instance());
    QDateTime now = QDateTime::currentDateTime();
    for (const Book& b : books) {
        std::optional<Discount> disc;
        if (b.getDiscountId() > 0) {
            auto raw = discountRepo.findById(b.getDiscountId());
            if (raw.has_value() && raw->getIsActive() &&
                now >= raw->getStartDate() && now <= raw->getEndDate()) {
                disc = raw;
            }
        }
        arr.append(ModelSerializer::serializeBook(b, disc));
    }
    return arr;
}

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
    bookService            = new BookService(bookRepo, discountRepo, userService, pubRepo, this);
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

        Publisher testPub("nasher1", "1234", "default");   // ← securityAnswer
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
        action == "updateSecurityAnswer" ||
        action == "checkUsernameExists" || action == "verifySecurityAnswer") {
        handleAuth(handler, action, data);
    }
    // 2. Books
    else if (action == "getAllBooks" || action == "addBook" ||
             action == "updateBookPrice" ||
             action == "activateBook" || action == "deactivateBook" ||
             action == "removeBook" || action == "getPublisherBooks" ||
             action == "getBookDetails" || action == "getBookPdf" || action == "searchBooks" ||
             action == "applyDiscountToBook" || action == "removeDiscountFromBook" ||
             action == "updateBookDetails" || action == "getHomeData" || action == "getBooksByGenre") {
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
             action == "unblockUser" || action == "deleteUserByAdmin"||
             action == "adminRemoveComment" || action == "blockUser" ||
             action == "blockPublisher" || action == "unblockPublisher" ||
             action == "adminRemoveBook" || action == "adminUpdateBook") {
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

        auto optAdmin = adminService->loginAdmin(username, password);
        if (optAdmin.has_value()) {
            response["status"] = "success";
            response["role"] = "Admin";
            response["data"] = ModelSerializer::serializeAdmin(optAdmin.value());
            handler->setUserId(optAdmin->getId());
            handler->sendResponse(response);
            return;
        }

        auto optPub = publisherService->login(username, password);
        if (optPub.has_value()) {
            if (!optPub->getIsActive()) {
                response["status"] = "error";
                response["message"] = "Your publisher account has been blocked by the admin";
            } else {
                response["status"] = "success";
                response["role"] = "Publisher";
                response["data"] = ModelSerializer::serializePublisher(optPub.value());
                handler->setUserId(optPub->getId());
            }
            handler->sendResponse(response);
            return;
        }

        auto optUser = userService->loginUser(username, password);
        if (optUser.has_value()) {
            User user = optUser.value();
            if (!user.getIsActive()) {
                response["status"] = "error";
                response["message"] = "Your account has been blocked by the admin";
            } else {
                response["status"] = "success";
                response["role"] = "User";
                response["data"] = ModelSerializer::serializeUser(user);
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

        QList<Genre> favoriteGenres;
        QJsonArray genresArray = data.value("favoriteGenres").toArray();
        for (const auto& val : std::as_const(genresArray)) {
            favoriteGenres.append(static_cast<Genre>(val.toInt()));
        }

        QString resMsg = userService->registerUser(u, p, sa, favoriteGenres);

        if (resMsg == "SUCCESS") {
            response["status"] = "success";
            response["message"] = "Registration completed successfully";

            auto userOpt = userService->getUserByUsername(u);
            if (userOpt.has_value()) {
                response["userId"] = userOpt->getId();
                response["data"] = ModelSerializer::serializeUser(userOpt.value());
            }
        } else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "loginPublisher") {
        QString u = data.value("username").toString().trimmed();
        QString p = data.value("password").toString();
        auto result = publisherService->login(u, p);
        if (result.has_value()) {
            if (!result->getIsActive()) {
                response["status"] = "error";
                response["message"] = "Your publisher account has been blocked by the admin";
            } else {
                response["status"] = "success";
                response["role"] = "Publisher";
                response["data"] = ModelSerializer::serializePublisher(result.value());
                handler->setUserId(result->getId());
            }
        } else {
            response["status"] = "error";
            response["message"] = "Invalid credentials";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "registerPublisher") {
        QString u = data.value("username").toString().trimmed();
        QString p = data.value("password").toString();
        QString sa = data.value("securityAnswer").toString().trimmed();

        QString resMsg = publisherService->registerPublisher(u, p, sa);
        if (resMsg == "SUCCESS") {
            response["status"] = "success";
            response["message"] = "Publisher registration completed successfully";
        } else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "loginAdmin") {
        QString u = data.value("username").toString().trimmed();
        QString p = data.value("password").toString();
        auto result = adminService->loginAdmin(u, p);
        if (result.has_value()) {
            response["status"] = "success";
            response["role"] = "Admin";
            response["data"] = ModelSerializer::serializeAdmin(result.value());
            handler->setUserId(result->getId());
        } else {
            response["status"] = "error";
            response["message"] = "Invalid admin credentials";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "registerAdmin") {
        QString u = data.value("username").toString().trimmed();
        QString p = data.value("password").toString();
        QString sa = data.value("securityAnswer").toString().trimmed();

        QString resMsg = adminService->registerAdmin(u, p, sa);
        if (resMsg == "SUCCESS") {
            response["status"] = "success";
            response["message"] = "Admin registration completed successfully";
        } else {
            response["status"] = "error";
            response["message"] = resMsg;
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "resetPassword") {
        QString u = data.value("username").toString().trimmed();
        QString sa = data.value("securityAnswer").toString().trimmed();
        QString newP = data.value("newPassword").toString();
        QString role = data.value("role").toString().toLower();

        bool success = false;
        if (role == "publisher") {
            success = publisherService->resetPasswordWithSecurityAnswer(u, sa, newP);
        } else if (role == "admin") {
            success = adminService->resetPasswordWithSecurityAnswer(u, sa, newP);
        } else {
            success = userService->resetPasswordWithSecurityAnswer(u, sa, newP);
        }

        if (success) {
            response["status"] = "success";
            response["message"] = "Password has been reset successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Security answer verification failed or account not found";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "changePassword") {
        int id = data.value("id").toInt();
        if (id <= 0) id = data.value("userId").toInt();
        if (id <= 0) id = data.value("publisherId").toInt();

        QString oldPassword = data.value("oldPassword").toString();
        QString newPassword = data.value("newPassword").toString();
        QString role = data.value("role").toString().toLower();

        bool success = false;
        if (role == "publisher") {
            success = publisherService->changePublisherPassword(id, oldPassword, newPassword);
        } else if (role == "admin") {
            success = adminService->changeAdminPassword(id, oldPassword, newPassword);
        } else {
            success = userService->changeUserPassword(id, oldPassword, newPassword);
        }

        if (success) {
            response["status"] = "success";
            response["message"] = "Password changed successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Incorrect current password";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "changeUsername") {
        int id = data.value("id").toInt();
        if (id <= 0) id = data.value("userId").toInt();
        if (id <= 0) id = data.value("publisherId").toInt();

        QString newUsername = data.value("username").toString().trimmed();
        QString password = data.value("password").toString();
        QString role = data.value("role").toString().toLower();

        bool success = false;
        if (role == "publisher") {
            success = publisherService->changePublisherUsername(id, newUsername, password);
        } else if (role == "admin") {
            success = adminService->changeAdminUsername(id, newUsername, password);
        } else {
            success = userService->changeUserUsername(id, newUsername, password);
        }

        if (success) {
            response["status"] = "success";
            response["message"] = "Username updated successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Invalid password or username already exists";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "updateProfile") {
        int userId = data.value("userId").toInt();
        QJsonArray genresArray = data.value("favoriteGenres").toArray();
        QList<Genre> favoriteGenres;
        for (const auto& val : std::as_const(genresArray)) {
            favoriteGenres.append(static_cast<Genre>(val.toInt()));
        }

        bool success = userService->updateUserFavoriteGenres(userId, favoriteGenres);
        if (success) {
            response["status"] = "success";
            response["message"] = "Profile favorite genres updated successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to update favorite genres";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "updateSecurityAnswer") {
        int id = data.value("id").toInt();
        if (id <= 0) id = data.value("userId").toInt();
        if (id <= 0) id = data.value("publisherId").toInt();

        QString newAnswer = data.value("securityAnswer").toString().trimmed();
        QString password = data.value("password").toString();
        QString role = data.value("role").toString().toLower();

        bool success = false;
        if (role == "publisher") {
            success = publisherService->changeSecurityAnswer(id, newAnswer, password);
        } else if (role == "admin") {
            success = adminService->changeSecurityAnswer(id, newAnswer, password);
        } else {
            success = userService->changeSecurityAnswer(id, newAnswer, password);
        }

        if (success) {
            response["status"] = "success";
            response["message"] = "Security answer updated successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to update security answer";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "checkUsernameExists") {
        QString username = data.value("username").toString().trimmed();

        bool exists = false;
        QString foundRole;

        //  User
        auto userOpt = userService->getUserByUsername(username);
        if (userOpt.has_value()) {
            exists = true;
            foundRole = "user";
        }

        //  Publisher
        if (!exists) {
            auto pubOpt = publisherService->getPublisherByPublishername(username);
            if (pubOpt.has_value()) {
                exists = true;
                foundRole = "publisher";
            }
        }

        //  Admin
        if (!exists) {
            auto adminOpt = adminService->getAdminByUsername(username);
            if (adminOpt.has_value()) {
                exists = true;
                foundRole = "admin";
            }
        }

        if (exists) {
            QJsonObject dataObj;
            dataObj["role"] = foundRole;
            dataObj["username"] = username;

            response["status"] = "success";
            response["data"] = dataObj;
        } else {
            response["status"] = "error";
            response["message"] = "No account found with this username";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "verifySecurityAnswer") {
        QString username = data.value("username").toString().trimmed();
        QString answer = data.value("securityAnswer").toString().trimmed();
        QString role = data.value("role").toString().toLower();

        bool valid = false;

        if (role == "user") {
            valid = userService->verifyUserSecurityAnswer(username, answer);
        } else if (role == "publisher") {
            valid = publisherService->verifyPublisherSecurityAnswer(username, answer);
        } else if (role == "admin") {
            valid = adminService->verifyAdminSecurityAnswer(username, answer);
        }

        if (valid) {
            QJsonObject dataObj;
            dataObj["valid"] = true;

            response["status"] = "success";
            response["data"] = dataObj;
        } else {
            response["status"] = "error";
            response["message"] = "Incorrect security answer";
        }
        handler->sendResponse(response);
        return;
    }

    else if (action == "resetPassword") {
        QString username = data.value("username").toString().trimmed();
        QString securityAnswer = data.value("securityAnswer").toString().trimmed();
        QString newPassword = data.value("newPassword").toString();
        QString role = data.value("role").toString().toLower();

        bool success = false;
        if (role == "publisher") {
            success = publisherService->resetPasswordWithSecurityAnswer(username, securityAnswer, newPassword);
        } else if (role == "admin") {
            success = adminService->resetPasswordWithSecurityAnswer(username, securityAnswer, newPassword);
        } else {
            success = userService->resetPasswordWithSecurityAnswer(username, securityAnswer, newPassword);
        }

        if (success) {
            response["status"] = "success";
            response["message"] = "Password has been reset successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Security answer verification failed or account not found";
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

        QJsonArray arr;
        DiscountRepository discountRepo(&DatabaseManager::instance());
        for (const Book& b : std::as_const(books)) {
            std::optional<Discount> disc;
            if (b.getDiscountId() > 0) {
                disc = discountRepo.findById(b.getDiscountId());
            }
            QJsonObject obj = ModelSerializer::serializeBook(b, disc);
            auto publisherOpt = publisherService->getPublisherById(b.getPublisherId());
            if (publisherOpt.has_value()) {
                obj["publisherUsername"] = publisherOpt->getUsername();  // ← اضافه کن
            }
            arr.append(obj);
        }

        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "addBook") {
        int pubId = data.value("publisherId").toInt();
        Book book = ModelSerializer::deserializeBook(data);
        book.setId(-1);
        book.setPublisherId(pubId);
        book.setDiscountId(-1);

        QString coverB64 = data["coverImage"].toString();
        QString pdfB64 = data["pdfFile"].toString();

        if (!coverB64.isEmpty()) {
            QDir().mkpath("uploads/covers");
            QString coverFileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
            QFile coverFile("uploads/covers/" + coverFileName);
            if (coverFile.open(QIODevice::WriteOnly)) {
                coverFile.write(QByteArray::fromBase64(coverB64.toUtf8()));
                coverFile.close();
                book.setCoverImagePath("uploads/covers/" + coverFileName);
            }
        }

        if (!pdfB64.isEmpty()) {
            QDir().mkpath("uploads/pdfs");
            QString pdfFileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".pdf";
            QFile pdfFile("uploads/pdfs/" + pdfFileName);
            if (pdfFile.open(QIODevice::WriteOnly)) {
                pdfFile.write(QByteArray::fromBase64(pdfB64.toUtf8()));
                pdfFile.close();
                book.setPdfFilePath("uploads/pdfs/" + pdfFileName);
            }
        }

        bool success = publisherService->addNewBook(pubId, book);
        if (success) {
            response["status"] = "success";
            response["bookId"] = book.getId();
            response["message"] = "Book added successfully";
            response["data"] = ModelSerializer::serializeBook(book);
        } else {
            response["status"] = "error";
            response["message"] = "Failed to add book. Ensure all fields are valid.";
        }
    }

    else if (action == "getPublisherBooks") {
        int pubId = data.value("publisherId").toInt();
        QList<Book> books = bookService->getBooksByPublisher(pubId);

        QJsonArray arr;
        DiscountRepository discountRepo(&DatabaseManager::instance());
        for (const Book& b : std::as_const(books)) {
            std::optional<Discount> disc;
            if (b.getDiscountId() > 0) {
                disc = discountRepo.findById(b.getDiscountId());
            }
            QJsonObject obj = ModelSerializer::serializeBook(b, disc);
            auto publisherOpt = publisherService->getPublisherById(b.getPublisherId());
            if (publisherOpt.has_value()) {
                obj["publisherUsername"] = publisherOpt->getUsername();
            }
            arr.append(obj);
        }

        response["status"] = "success";
        response["data"] = arr;
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
    else if (action == "deactivateBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        bool success = publisherService->removeBook(pubId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to deactivate book.";
    }
    else if (action == "activateBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        bool success = publisherService->activateBook(pubId, bookId);
        response["status"] = success ? "success" : "error";
        if (!success) response["message"] = "Failed to activate book.";
    }
    else if (action == "getBookDetails") {
        int bookId = data.value("bookId").toInt();
        int userId = data.value("userId").toInt();

        auto bookOpt = bookService->getBookById(bookId);
        if (bookOpt.has_value()) {
            Book book = bookOpt.value();

            std::optional<Discount> disc;
            if (book.getDiscountId() > 0) {
                DiscountRepository discountRepo(&DatabaseManager::instance());
                auto raw = discountRepo.findById(book.getDiscountId());
                QDateTime now = QDateTime::currentDateTime();
                if (raw.has_value() && raw->getIsActive() &&
                    now >= raw->getStartDate() && now <= raw->getEndDate()) {
                    disc = raw;
                }
            }
            QJsonObject bookJson = ModelSerializer::serializeBook(book, disc);

            auto publisherOpt = publisherService->getPublisherById(book.getPublisherId());
            if (publisherOpt.has_value()) {
                bookJson["publisherUsername"] = publisherOpt->getUsername();
            }

            if (userId > 0) {
                bookJson["isPurchased"] = personalLibraryService->hasPurchased(userId, bookId);

                CartDetails cartDetails = shoppingCartService->getCartDetails(userId);
                bookJson["isInCart"] = cartDetails.bookIds.contains(bookId);

                QList<int> wishlist = personalLibraryService->getWishlist(userId);
                bookJson["isInWishlist"] = wishlist.contains(bookId);

                QList<int> favorites = personalLibraryService->getFaveBooks(userId);
                bookJson["isFavorite"] = favorites.contains(bookId);
            } else {
                bookJson["isPurchased"] = false;
                bookJson["isInCart"] = false;
                bookJson["isInWishlist"] = false;
                bookJson["isFavorite"] = false;
            }

            response["status"] = "success";
            response["data"] = bookJson;
        } else {
            response["status"] = "error";
            response["message"] = "Book not found";
        }
    }
    else if (action == "getBookPdf") {
        qDebug() << "=== getBookPdf START ===";

        int userId = data.value("userId").toInt();
        int bookId = data.value("bookId").toInt();
        qDebug() << "userId:" << userId << "bookId:" << bookId;

        if (!personalLibraryService) {
            qDebug() << "ERROR: personalLibraryService is NULL!";
            response["status"] = "error";
            response["message"] = "Server error: personalLibraryService not initialized";
            handler->sendResponse(response);
            return;
        }

        QList<int> purchasedIds = personalLibraryService->getPurchasedBooks(userId);
        qDebug() << "purchasedIds:" << purchasedIds;

        if (!purchasedIds.contains(bookId)) {
            qDebug() << "User has not purchased this book";
            response["status"] = "error";
            response["message"] = "You must purchase this book before reading it.";
            handler->sendResponse(response);
            return;
        }

        if (!bookService) {
            qDebug() << "ERROR: bookService is NULL!";
            response["status"] = "error";
            response["message"] = "Server error: bookService not initialized";
            handler->sendResponse(response);
            return;
        }

        auto bookOpt = bookService->getBookById(bookId);
        qDebug() << "bookOpt has_value:" << bookOpt.has_value();

        if (!bookOpt.has_value()) {
            qDebug() << "Book not found!";
            response["status"] = "error";
            response["message"] = "Book not found.";
            handler->sendResponse(response);
            return;
        }

        QString pdfPath = bookOpt->getPdfFilePath();
        qDebug() << "pdfPath:" << pdfPath;

        if (pdfPath.isEmpty()) {
            qDebug() << "PDF path is empty!";
            response["status"] = "error";
            response["message"] = "No PDF file available for this book.";
            handler->sendResponse(response);
            return;
        }

        QFile pdfFile(pdfPath);
        qDebug() << "File exists:" << pdfFile.exists();

        if (!pdfFile.exists()) {
            qDebug() << "PDF file does not exist!";
            response["status"] = "error";
            response["message"] = "PDF file not found on server.";
            handler->sendResponse(response);
            return;
        }

        if (!pdfFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open PDF file!";
            response["status"] = "error";
            response["message"] = "Failed to open PDF file on server.";
            handler->sendResponse(response);
            return;
        }

        qDebug() << "Reading PDF file...";
        QByteArray pdfData = pdfFile.readAll();
        qDebug() << "PDF size:" << pdfData.size() << "bytes";
        pdfFile.close();

        qDebug() << "Encoding to Base64...";
        QString pdfBase64 = QString::fromLatin1(pdfData.toBase64());
        qDebug() << "Base64 size:" << pdfBase64.size();

        QJsonObject payload;
        payload["pdfData"] = pdfBase64;
        payload["title"] = bookOpt->getTitle();
        payload["fileName"] = bookOpt->getTitle() + ".pdf";

        response["status"] = "success";
        response["data"] = payload;

        qDebug() << "=== getBookPdf SUCCESS ===";
        handler->sendResponse(response);
    }
    else if (action == "searchBooks") {
        QString query = data.value("query").toString().trimmed();
        QList<Book> found = bookService->search(query);

        QJsonArray arr;
        for (const Book& b : std::as_const(found)) {
            QJsonObject obj = ModelSerializer::serializeBook(b);
            auto publisherOpt = publisherService->getPublisherById(b.getPublisherId());
            if (publisherOpt.has_value()) {
                obj["publisherUsername"] = publisherOpt->getUsername();
            }
            arr.append(obj);
        }

        response["status"] = "success";
        response["data"] = arr;
    }
    else if (action == "applyDiscountToBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();
        double value = data.value("value").toDouble();
        int type = data.value("type").toInt();
        qint64 startDate = data.value("startDate").toVariant().toLongLong();
        qint64 endDate = data.value("endDate").toVariant().toLongLong();

        if (pubId <= 0 || bookId <= 0 || value <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid parameters";
        } else {
            Discount discount(
                value,
                static_cast<Discount::DiscountType>(type),
                QDateTime::fromSecsSinceEpoch(startDate),
                QDateTime::fromSecsSinceEpoch(endDate)
                );

            DiscountRepository discountRepo(&DatabaseManager::instance());
            if (discountRepo.save(discount)) {
                bool success = publisherService->applyDiscountToBook(pubId, bookId, discount.getId());
                if (success) {
                    response["status"] = "success";
                    response["message"] = "Discount applied successfully";
                    response["discountId"] = discount.getId();
                } else {
                    response["status"] = "error";
                    response["message"] = "Failed to apply discount to book";
                }
            } else {
                response["status"] = "error";
                response["message"] = "Failed to save discount";
            }
        }
    }
    else if (action == "removeDiscountFromBook") {
        int pubId = data.value("publisherId").toInt();
        int bookId = data.value("bookId").toInt();

        if (pubId <= 0 || bookId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid parameters";
        } else {
            bool success = publisherService->removeDiscountFromBook(pubId, bookId);
            if (success) {
                response["status"] = "success";
                response["message"] = "Discount removed successfully";
            } else {
                response["status"] = "error";
                response["message"] = "Failed to remove discount";
            }
        }
    }
    else if (action == "updateBookDetails") {
        Book book = ModelSerializer::deserializeBook(data);
        QString role = data.value("role").toString().toLower();
        int publisherId = data.value("publisherId").toInt();

        if (book.getId() <= 0 || book.getTitle().isEmpty()) {
            response["status"] = "error";
            response["message"] = "Invalid book data";
        } else {
            auto existingBook = bookService->getBookById(book.getId());

            QString coverB64 = data["coverImage"].toString();
            QString pdfB64 = data["pdfFile"].toString();

            if (!coverB64.isEmpty()) {
                QDir().mkpath("uploads/covers");
                QString coverFileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
                QFile coverFile("uploads/covers/" + coverFileName);
                if (coverFile.open(QIODevice::WriteOnly)) {
                    coverFile.write(QByteArray::fromBase64(coverB64.toUtf8()));
                    coverFile.close();
                    book.setCoverImagePath("uploads/covers/" + coverFileName);
                }
            } else if (existingBook.has_value()) {
                book.setCoverImagePath(existingBook->getCoverImagePath());
            }

            if (!pdfB64.isEmpty()) {
                QDir().mkpath("uploads/pdfs");
                QString pdfFileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".pdf";
                QFile pdfFile("uploads/pdfs/" + pdfFileName);
                if (pdfFile.open(QIODevice::WriteOnly)) {
                    pdfFile.write(QByteArray::fromBase64(pdfB64.toUtf8()));
                    pdfFile.close();
                    book.setPdfFilePath("uploads/pdfs/" + pdfFileName);
                }
            } else if (existingBook.has_value()) {
                book.setPdfFilePath(existingBook->getPdfFilePath());
            }

            bool success = false;
            if (role == "admin") {
                success = adminService->updateBookDetailsByAdmin(book);
            } else if (role == "publisher") {
                if (publisherId <= 0) {
                    response["status"] = "error";
                    response["message"] = "Invalid publisher ID";
                    handler->sendResponse(response);
                    return;
                }
                success = publisherService->updateBookDetailsByPublisher(publisherId, book);
            } else {
                response["status"] = "error";
                response["message"] = "Invalid role";
                handler->sendResponse(response);
                return;
            }

            response["status"] = success ? "success" : "error";
            response["message"] = success ? "Book updated successfully" : "Failed to update book";
            if (success) {
                response["data"] = ModelSerializer::serializeBook(book);
            }
        }
    }
    else if (action == "getHomeData") {
        int userId = data.value("userId").toInt();
        qDebug() << "=== getHomeData called with userId:" << userId;

        QList<Book> recommended = bookService->getRecommendedBooksForUser(userId);
        qDebug() << "=== Recommended books:" << recommended.size();

        QList<Book> popular = bookService->getPopularBooks();
        qDebug() << "=== Popular books:" << popular.size();

        QList<Book> newReleases = bookService->getNewReleases();
        qDebug() << "=== New releases:" << newReleases.size();

        QList<Book> bestSellers = bookService->getBestSellers();
        qDebug() << "=== Best sellers:" << bestSellers.size();

        QList<Book> freeBooks = bookService->getFreeBooks();
        qDebug() << "=== Free books:" << freeBooks.size();

        QJsonObject homeData;
        homeData["recommendedBooks"] = serializeBookListWithDiscount(recommended);
        homeData["popularBooks"] = serializeBookListWithDiscount(popular);
        homeData["newReleases"] = serializeBookListWithDiscount(newReleases);
        homeData["bestSellers"] = serializeBookListWithDiscount(bestSellers);
        homeData["freeBooks"] = serializeBookListWithDiscount(freeBooks);

        response["status"] = "success";
        response["data"] = homeData;
    }
    else if (action == "getBooksByGenre") {
        int genreId = data.value("genreId").toInt();
        Genre genre = static_cast<Genre>(genreId);
        QList<Book> books = bookService->getBooksByGenre(genre);
        response["status"] = "success";
        response["data"] = serializeBookListWithDiscount(books);
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
        response["status"] = "success";
        response["data"]   = ModelSerializer::serializeCartDetails(details);
    }

    else if (action == "checkout") {
        bool success = orderService->checkout(userId);
        if (success) {
            response["status"] = "success";
            response["message"] = "Checkout completed successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Checkout failed (empty cart or invalid items)";
        }
    }
    else if (action == "getOrderHistory") {
        QList<Order> orders = orderService->getOrderHistory(userId);
        response["status"] = "success";
        response["data"]   = ModelSerializer::serializeOrderList(orders);
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
    else if (action == "blockPublisher") {
        int targetId = data.value("targetId").toInt();
        if (targetId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid target ID";
        } else {
            bool success = adminService->blockPublisher(targetId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to block publisher or user not found.";
        }
    }
    else if (action == "unblockPublisher") {
        int targetId = data.value("targetId").toInt();
        if (targetId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid target ID";
        } else {
            bool success = adminService->unblockPublisher(targetId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to unblock publisher.";
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
    else if (action == "adminRemoveBook") {
        int bookId = data.value("bookId").toInt();
        if (bookId <= 0) {
            response["status"] = "error";
            response["message"] = "Invalid book ID";
        } else {
            bool success = adminService->removeBookByAdmin(bookId);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to remove book";
        }
    }
    else if (action == "adminUpdateBook") {
        Book book = ModelSerializer::deserializeBook(data);
        if (book.getId() <= 0 || book.getTitle().isEmpty()) {
            response["status"] = "error";
            response["message"] = "Invalid book data";
        } else {
            bool success = adminService->updateBookDetailsByAdmin(book);
            response["status"] = success ? "success" : "error";
            if (!success) response["message"] = "Failed to update book";
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