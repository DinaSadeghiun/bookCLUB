#include "Services/userservice.h"
#include "DB/userrepository.h"

UserService::UserService(UserRepository* repository, QObject* parent)
    : QObject(parent), userRepo(repository)
{
    Q_ASSERT(userRepo != nullptr);
}


//auth management
QString UserService::registerUser(const QString& username, const QString& password,
                                  const QString& securityAnswer, double initialBalance,
                                  const QList<Genre>& favoriteGenres)
{
    if (!userRepo) {
        return "DATABASE_ERROR";
    }
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

    User newUser(trimmedUser, password, trimmedAns, initialBalance, favoriteGenres);

    if (!favoriteGenres.isEmpty() && (favoriteGenres.size() < 1 || favoriteGenres.size() > 3)) {
        return "INVALID_GENRE_COUNT";
    }

    if (userRepo->save(newUser)) {
        emit userRegistered(newUser.getId());
        return "SUCCESS";
    } else {
        return "DATABASE_ERROR";
    }
}

std::optional<User> UserService::loginUser(const QString& username, const QString& password) {
    if (!userRepo) {
        return std::nullopt;
    }
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
    if (!userRepo) {
        return false;
    }
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

    if (userRepo->save(*userOpt)) {
        emit userCredentialsChanged(userOpt->getId());
        return true;
    }
    return false;
}

std::optional<User> UserService::getUserById(int userId) const {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return std::nullopt;
    }
    return userRepo->findById(userId);
}

std::optional<User> UserService::getUserByUsername(const QString& username) const {
    if (!userRepo) {
        return std::nullopt;
    }
    return userRepo->findByUsername(username.trimmed());
}

bool UserService::changeUserPassword(int userId, const QString& oldPassword, const QString& newPassword) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return false;
    }
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (!userOpt->changePassword(oldPassword, newPassword)) {
        return false;
    }

    if (userRepo->save(*userOpt)) {
        emit userCredentialsChanged(userId);
        return true;
    }
    return false;
}

bool UserService::changeUserUsername(int userId, const QString& newUsername, const QString& password) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return false;
    }
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

    if (userRepo->save(*userOpt)) {
        emit userCredentialsChanged(userId);
        return true;
    }
    return false;
}

bool UserService::changeSecurityAnswer(int userId, const QString& newAnswer) {
    if (userId <= 0 || newAnswer.trimmed().isEmpty()) {
        return false;
    }
    auto userOpt = userRepo->findById(userId);
    if (!userOpt.has_value()) {
        return false;
    }
    User user = userOpt.value();
    user.setSecurityAnswer(newAnswer.trimmed());
    if (userRepo->save(user)) {
        emit userCredentialsChanged(userId);
        return true;
    }
    return false;
}


bool UserService::deleteUser(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return false;
    }
    return userRepo->remove(userId);
}


//balance mng
bool UserService::depositBalance(int userId, double amount) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || amount <= 0.0 || !userRepo) {
        return false;
    }
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->deposit(amount)) {
        if (userRepo->save(*userOpt)) {
            emit walletBalanceChanged(userId, userOpt->getWalletBalance());
            return true;
        }
    }
    return false;
}

bool UserService::withdrawBalance(int userId, double amount) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || amount <= 0.0 || !userRepo) {
        return false;
    }

    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->withdraw(amount)) {
        if (userRepo->save(*userOpt)) {
            emit walletBalanceChanged(userId, userOpt->getWalletBalance());
            return true;
        }
    }
    return false;
}

double UserService::getWalletBalance(int userId) const {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return 0.0;
    }
    auto userOpt = userRepo->findById(userId);
    return userOpt ? userOpt->getWalletBalance() : 0.0;
}

//genres
bool UserService::updateUserFavoriteGenres(int userId, const QList<Genre>& genres) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return false;
    }
    auto userOpt = userRepo->findById(userId);
    if (!userOpt) {
        return false;
    }

    if (userOpt->setFavoriteGenres(genres)) {
        if (userRepo->save(*userOpt)) {
            emit favoriteGenresChanged(userId);
            return true;
        }
    }
    return false;
}

QList<Genre> UserService::getUserFavoriteGenres(int userId) const {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !userRepo) {
        return QList<Genre>();
    }
    auto userOpt = userRepo->findById(userId);
    return userOpt ? userOpt->getFavoriteGenres() : QList<Genre>();
}
