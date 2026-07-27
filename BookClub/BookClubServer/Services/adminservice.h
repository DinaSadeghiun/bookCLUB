#ifndef ADMINSERVICE_H
#define ADMINSERVICE_H

#include <QString>
#include <QList>
#include <optional>
#include <QObject>

#include "Admin.h"
#include "user.h"
#include "Publisher.h"
#include "Book.h"
#include "Comment.h"

#include "DB/adminrepository.h"
#include "DB/userrepository.h"
#include "DB/publisherrepository.h"
#include "DB/bookrepository.h"
#include "DB/commentrepository.h"

class AdminService : public QObject {
    Q_OBJECT

private:
    AdminRepository* adminRepo;
    UserRepository* userRepo;
    PublisherRepository* pubRepo;
    BookRepository* bookRepo;
    CommentRepository* commentRepo;

public:
    explicit AdminService(AdminRepository* aRepo,
                          UserRepository* uRepo,
                          PublisherRepository* pRepo,
                          BookRepository* bRepo,
                          CommentRepository* cRepo,
                          QObject* parent = nullptr);

    //Auth
    QString registerAdmin(const QString& username, const QString& password, const QString& securityAnswer);
    std::optional<Admin> loginAdmin(const QString& username, const QString& password);
    bool verifyAdminSecurityAnswer(const QString& username, const QString& answer) const;
    bool resetPasswordWithSecurityAnswer(const QString& username, const QString& answer, const QString& newPassword);
    std::optional<Admin> getAdminByUsername(const QString& username) const;


    //Profile
    bool changeAdminPassword(int adminId, const QString& oldPassword, const QString& newPassword);
    bool changeAdminUsername(int adminId, const QString& newUsername, const QString& password);
    bool changeSecurityAnswer(int adminId, const QString& newAnswer, const QString& password);

    //Users and Publisher
    QList<User> getAllUsers() const;
    QList<Publisher> getAllPublishers() const;
    QList<User> searchUsers(const QString& query) const;
    QList<Publisher> searchPublishers(const QString& query) const;
    std::optional<User> getUserDetails(int userId) const;
    std::optional<Publisher> getPublisherDetails(int publisherId) const;


    bool blockUser(int userId);
    bool unblockUser(int userId);
    bool blockPublisher(int publisherId);
    bool unblockPublisher(int publisherId);
    bool deleteUserAccount(int userId);
    bool deletePublisherAccount(int publisherId);
    bool setAccountStatus(int personId, bool active);

    //Books
    QList<Book> getAllBooks() const;
    bool removeBookByAdmin(int bookId);
    bool updateBookDetailsByAdmin(const Book& book);

    //Comments
    QList<Comment> getAllComments() const;
    bool removeCommentByAdmin(int commentId);

signals:
    void adminRegistered(int adminId);
    void adminCredentialsChanged(int adminId);
    void userStatusChanged(int userId, bool isActive);
    void userDeleted(int userId);
    void publisherStatusChanged(int publisherId, bool isActive);
    void publisherDeleted(int publisherId);
    void bookRemovedByAdmin(int bookId);
    void bookUpdatedByAdmin(int bookId);
    void commentRemovedByAdmin(int commentId);
};

#endif
