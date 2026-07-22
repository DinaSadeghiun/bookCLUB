#include "adminservice.h"

AdminService::AdminService(AdminRepository* aRepo,
                           UserRepository* uRepo,
                           PublisherRepository* pRepo,
                           BookRepository* bRepo,
                           CommentRepository* cRepo,
                           QObject* parent)
    : QObject(parent), adminRepo(aRepo), userRepo(uRepo),
    pubRepo(pRepo), bookRepo(bRepo), commentRepo(cRepo)
{
    Q_ASSERT(adminRepo != nullptr);
    Q_ASSERT(userRepo != nullptr);
    Q_ASSERT(pubRepo != nullptr);
    Q_ASSERT(bookRepo != nullptr);
    Q_ASSERT(commentRepo != nullptr);
}

// Auth
QString AdminService::registerAdmin(const QString& username, const QString& password, const QString& securityAnswer) {
    if (!adminRepo) {
        return "DATABASE_ERROR";
    }

    if (username.trimmed().isEmpty() || password.trimmed().isEmpty() || securityAnswer.trimmed().isEmpty()) {
        return "EMPTY_FIELDS";
    }

    if (adminRepo->findByUsername(username).has_value()) {
        return "USERNAME_TAKEN";
    }

    Admin newAdmin(username, password, securityAnswer);

    if (adminRepo->save(newAdmin)) {
        emit adminRegistered(newAdmin.getId());
        return "SUCCESS";
    }

    return "DATABASE_ERROR";
}

std::optional<Admin> AdminService::loginAdmin(const QString& username, const QString& password) {
    auto adminOpt = adminRepo->findByUsername(username);
    if (!adminRepo) {
        return std::nullopt;
    }

    if (adminOpt && adminOpt->verifyPassword(password)) {
        return adminOpt;
    }
    return std::nullopt;
}

bool AdminService::resetPasswordWithSecurityAnswer(const QString& username,
                                                   const QString& answer,
                                                   const QString& newPassword)
{
    auto trimmedUsername = username.trimmed();
    auto trimmedAnswer = answer.trimmed();
    auto trimmedNewPassword = newPassword.trimmed();

    if (!adminRepo) {
        return false;
    }

    if (trimmedUsername.isEmpty() || trimmedAnswer.isEmpty() || trimmedNewPassword.isEmpty()) {
        return false;
    }

    auto adminOpt = adminRepo->findByUsername(trimmedUsername);
    if (!adminOpt) {
        return false;
    }

    if (adminOpt->changePasswordWithSecurityAnswer(trimmedAnswer, trimmedNewPassword)) {
        if (adminRepo->save(*adminOpt)) {
            emit adminCredentialsChanged(adminOpt->getId());
            return true;
        }
    }
    return false;
}

// Profile
bool AdminService::changeAdminPassword(int adminId, const QString& oldPassword, const QString& newPassword) {
    auto adminOpt = adminRepo->findById(adminId);
    if (!adminOpt) return false;

    if (!adminOpt->changePassword(oldPassword, newPassword)) return false;
    return adminRepo->save(*adminOpt);
}

bool AdminService::changeAdminUsername(int adminId, const QString& newUsername, const QString& password) {
    auto adminOpt = adminRepo->findById(adminId);
    if (!adminOpt) return false;

    if (!adminOpt->changeUsername(newUsername, password)) return false;
    return adminRepo->save(*adminOpt);
}

bool AdminService::changeSecurityAnswer(int adminId, const QString& newAnswer) {
    auto adminOpt = adminRepo->findById(adminId);
    if (!adminOpt) return false;

    adminOpt->setSecurityAnswer(newAnswer);
    return adminRepo->save(*adminOpt);
}

QList<User> AdminService::getAllUsers() const {
    if (!userRepo) return {};
    return userRepo->findAll();
}

QList<Publisher> AdminService::getAllPublishers() const {
    if (!pubRepo) return {};
    return pubRepo->findAll();
}

QList<User> AdminService::searchUsers(const QString& query) const {
    if (!userRepo || query.trimmed().isEmpty()) {
        return getAllUsers();
    }
    return userRepo->searchUsers(query.trimmed());
}

QList<Publisher> AdminService::searchPublishers(const QString& query) const {
    if (!pubRepo || query.trimmed().isEmpty()) {
        return getAllPublishers();
    }
    return pubRepo->searchPublishers(query.trimmed());
}


std::optional<User> AdminService::getUserDetails(int userId) const {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return std::nullopt;
    }
    return userRepo->findById(userId);
}

std::optional<Publisher> AdminService::getPublisherDetails(int publisherId) const {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo) {
        return std::nullopt;
    }
    return pubRepo->findById(publisherId);
}

bool AdminService::blockUser(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) return false;
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) return false;

    userOpt->setIsActive(false);
    if (userRepo->save(*userOpt)) {
        emit userStatusChanged(userId, false);
        return true;
    }
    return false;
}

bool AdminService::unblockUser(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) return false;
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) return false;

    userOpt->setIsActive(true);
    if (userRepo->save(*userOpt)) {
        emit userStatusChanged(userId, true);
        return true;
    }
    return false;
}

bool AdminService::deleteUserAccount(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) return false;
    if (userRepo->remove(userId)) {
        emit userDeleted(userId);
        return true;
    }
    return false;
}

bool AdminService::deletePublisherAccount(int publisherId) {
    Q_ASSERT(publisherId > 0);
    if (publisherId <= 0 || !pubRepo) return false;
    if (pubRepo->remove(publisherId)) {
        emit publisherDeleted(publisherId);
        return true;
    }
    return false;
}

bool AdminService::setAccountStatus(int personId, bool active) {
    Q_ASSERT(personId > 0);
    if (personId <= 0 || !userRepo || !pubRepo) return false;
    auto userOpt = userRepo->findById(personId);
    if (userOpt) {
        userOpt->setIsActive(active);
        if (userRepo->save(*userOpt)) {
            emit userStatusChanged(personId, active);
            return true;
        }
        return false;
    }

    auto pubOpt = pubRepo->findById(personId);
    if (pubOpt) {
        pubOpt->setIsActive(active);
        if (pubRepo->save(*pubOpt)) {
            emit publisherStatusChanged(personId, active);
            return true;
        }
        return false;
    }

    return false;
}

QList<Book> AdminService::getAllBooks() const {
    if (!bookRepo) return {};
    return bookRepo->findAll();
}

bool AdminService::removeBookByAdmin(int bookId) {
    Q_ASSERT(bookId > 0);
    if (bookId <= 0 || !bookRepo) return false;
    if (bookRepo->remove(bookId)) {
        emit bookRemovedByAdmin(bookId);
        return true;
    }
    return false;
}

bool AdminService::updateBookDetailsByAdmin(const Book& book) {
    if (book.getId() <= 0 || !bookRepo) return false;

    auto bookOpt = bookRepo->findById(book.getId());
    if (!bookOpt) return false;

    bookOpt->setTitle(book.getTitle());
    bookOpt->setAuthor(book.getAuthor());
    bookOpt->setPrice(book.getPrice());
    bookOpt->setGenre(book.getGenre());
    bookOpt->setDescription(book.getDescription());
    bookOpt->setCoverImagePath(book.getCoverImagePath());
    bookOpt->setPdfFilePath(book.getPdfFilePath());

    return bookRepo->save(*bookOpt);
}

QList<Comment> AdminService::getAllComments() const {
    if (!commentRepo) return {};
    return commentRepo->findAll();
}

bool AdminService::removeCommentByAdmin(int commentId) {
    Q_ASSERT(commentId > 0);
    if (commentId <= 0 || !commentRepo) return false;
    if (commentRepo->remove(commentId)) {
        emit commentRemovedByAdmin(commentId);
        return true;
    }
    return false;
}
