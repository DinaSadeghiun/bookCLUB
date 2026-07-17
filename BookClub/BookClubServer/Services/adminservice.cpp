#include "adminservice.h"

AdminService::AdminService(AdminRepository* aRepo,
                           UserRepository* uRepo,
                           PublisherRepository* pRepo,
                           BookRepository* bRepo,
                           CommentRepository* cRepo,
                           QObject* parent)
    : QObject(parent), adminRepo(aRepo), userRepo(uRepo), pubRepo(pRepo), bookRepo(bRepo), commentRepo(cRepo) {}

QString AdminService::registerAdmin(const QString& username, const QString& password, const QString& securityAnswer) {
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

QList<User> AdminService::getAllUsers() const {
    return userRepo->findAll();
}

QList<Publisher> AdminService::getAllPublishers() const {
    return pubRepo->findAll();
}

std::optional<User> AdminService::getUserDetails(int userId) const {
    return userRepo->findById(userId);
}

std::optional<Publisher> AdminService::getPublisherDetails(int publisherId) const {
    return pubRepo->findById(publisherId);
}

bool AdminService::blockUser(int userId) {
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
    if (userRepo->remove(userId)) {
        emit userDeleted(userId);
        return true;
    }
    return false;
}

bool AdminService::deletePublisherAccount(int publisherId) {
    if (pubRepo->remove(publisherId)) {
        emit publisherDeleted(publisherId);
        return true;
    }
    return false;
}

bool AdminService::setAccountStatus(int personId, bool active) {
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
    return bookRepo->findAll();
}

bool AdminService::removeBookByAdmin(int bookId) {
    if (bookRepo->remove(bookId)) {
        emit bookRemovedByAdmin(bookId);
        return true;
    }
    return false;
}

bool AdminService::updateBookDetailsByAdmin(int bookId, const QString& title, double price) {
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt) return false;

    bookOpt->setTitle(title.trimmed());
    bookOpt->setPrice(price);

    if (bookRepo->save(*bookOpt)) {
        emit bookUpdatedByAdmin(bookId);
        return true;
    }
    return false;
}

QList<Comment> AdminService::getAllComments() const {
    return commentRepo->findAll();
}

bool AdminService::removeCommentByAdmin(int commentId) {
    if (commentRepo->remove(commentId)) {
        emit commentRemovedByAdmin(commentId);
        return true;
    }
    return false;
}
