#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <QString>
#include <QList>
#include <optional>
#include "user.h"
#include "genre.h"

// Forward declaration
class UserRepository;

class UserService {
private:
    UserRepository* userRepo;

public:
    explicit UserService(UserRepository* repository);

    // Authentication & Registration
    QString registerUser(const QString& username,
                         const QString& password,
                         const QString& securityAnswer,
                         double initialBalance = 0.0);

    std::optional<User> loginUser(const QString& username, const QString& password);

    bool resetPasswordWithSecurityAnswer(const QString& username,
                                         const QString& securityAnswer,
                                         const QString& newPassword);

    // Account Management
    bool changeUserPassword(int userId, const QString& oldPassword, const QString& newPassword);
    bool changeUserUsername(int userId, const QString& newUsername, const QString& password);


    // Wallet Operations
    bool depositBalance(int userId, double amount);
    bool withdrawBalance(int userId, double amount);
    double getWalletBalance(int userId) const;

    // Genres Preference Management
    bool updateUserFavoriteGenres(int userId, const QList<Genre>& genres);
    QList<Genre> getUserFavoriteGenres(int userId) const;
};

#endif
