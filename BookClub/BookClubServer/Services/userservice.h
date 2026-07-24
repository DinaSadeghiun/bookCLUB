#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <QObject>
#include <QString>
#include <QList>
#include <optional>
#include "user.h"
#include "genre.h"

// Forward declaration
class UserRepository;

class UserService : public QObject{
    Q_OBJECT
private:
    UserRepository* userRepo;

public:
    explicit UserService(UserRepository* repository, QObject* parent = nullptr);

    // Authentication & Registration
    QString registerUser(const QString& username, const QString& password,
                         const QString& securityAnswer,
                         const QList<Genre>& favoriteGenres = {});

    std::optional<User> loginUser(const QString& username, const QString& password);

    bool resetPasswordWithSecurityAnswer(const QString& username,
                                         const QString& securityAnswer,
                                         const QString& newPassword);

    std::optional<User> getUserById(int userId) const;
    std::optional<User> getUserByUsername(const QString& username) const;

    // Account Management
    bool changeUserPassword(int userId, const QString& oldPassword, const QString& newPassword);
    bool changeUserUsername(int userId, const QString& newUsername, const QString& password);
    bool changeSecurityAnswer(int userId, const QString& newAnswer, const QString& password);

    // Genres Preference Management
    bool updateUserFavoriteGenres(int userId, const QList<Genre>& genres);
    QList<Genre> getUserFavoriteGenres(int userId) const;

signals:
    void userRegistered(int userId);
    void userCredentialsChanged(int userId);
    void favoriteGenresChanged(int userId);

};

#endif
