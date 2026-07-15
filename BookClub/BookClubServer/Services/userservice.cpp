#include "Services/userservice.h"
#include "DB/userrepository.h"

UserService::UserService(UserRepository* repository)
    : userRepo(repository) {}

//auth management
QString UserService::registerUser(const QString& username,
                                  const QString& password,
                                  const QString& securityAnswer,
                                  double initialBalance)
{
    QString trimmedUser = username.trimmed();
    QString trimmedAns = securityAnswer.trimmed();

    if (trimmedUser.isEmpty() || password.isEmpty() || trimmedAns.isEmpty()) {
        return "EMPTY_FIELDS";
    }

    if (initialBalance < 0.0) {
        return "INVALID_BALANCE";
    }

    auto existingUser = userRepo->findByUsername(trimmedUser);
    if (existingUser.has_value()) {
        return "USERNAME_TAKEN";
    }

    User newUser(trimmedUser, password, trimmedAns, initialBalance);

    if (userRepo->save(newUser)) {
        return "SUCCESS";
    } else {
        return "DATABASE_ERROR";
    }
}

std::optional<User> UserService::loginUser(const QString& username, const QString& password) {
    QString trimmedUser = username.trimmed();
    if (trimmedUser.isEmpty() || password.isEmpty()) {
        return std::nullopt;
    }

    auto userOpt = userRepo->authenticate(trimmedUser, password);

    if (userOpt && userOpt->canLogin()) {
        return userOpt;
    }

    return std::nullopt;
}


//password mng
bool UserService::resetPasswordWithSecurityAnswer(const QString& username,
                                                  const QString& securityAnswer,
                                                  const QString& newPassword)
{
    QString trimmedUser = username.trimmed();
    if (trimmedUser.isEmpty()) {
        return false;
    }

    auto userOpt = userRepo->findByUsername(trimmedUser);
    if (!userOpt.has_value()) {
        return false;
    }

    if (!userOpt->changePasswordWithSecurityAnswer(securityAnswer, newPassword)) {
        return false;
    }

    return userRepo->save(*userOpt);
}

bool UserService::changeUserPassword(int userId, const QString& oldPassword, const QString& newPassword) {
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (!userOpt->changePassword(oldPassword, newPassword)) {
        return false;
    }

    return userRepo->save(*userOpt);
}

bool UserService::changeUserUsername(int userId, const QString& newUsername, const QString& password) {
    QString trimmedNewUser = newUsername.trimmed();
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    auto existingUser = userRepo->findByUsername(trimmedNewUser);
    if (existingUser && existingUser->getId() != userId) {
        return false;
    }

    if (!userOpt->changeUsername(trimmedNewUser, password)) {
        return false;
    }

    return userRepo->save(*userOpt);
}

//balance mng
bool UserService::depositBalance(int userId, double amount) {
    if (amount <= 0.0) {
        return false;
    }

    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->deposit(amount)) {
        return userRepo->save(*userOpt);
    }
    return false;
}

bool UserService::withdrawBalance(int userId, double amount) {
    if (amount <= 0.0) {
        return false;
    }

    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->withdraw(amount)) {
        return userRepo->save(*userOpt);
    }
    return false;
}

double UserService::getWalletBalance(int userId) const {
    auto userOpt = userRepo->findById(userId);
    return userOpt ? userOpt->getWalletBalance() : 0.0;
}

//genres
bool UserService::updateUserFavoriteGenres(int userId, const QList<Genre>& genres) {
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->setFavoriteGenres(genres)) {
        return userRepo->save(*userOpt);
    }
    return false;
}

QList<Genre> UserService::getUserFavoriteGenres(int userId) const {
    auto userOpt = userRepo->findById(userId);
    return userOpt ? userOpt->getFavoriteGenres() : QList<Genre>();
}
