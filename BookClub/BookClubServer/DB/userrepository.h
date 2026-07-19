#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "user.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>

inline const QString ROLE_USER = "User";
class DatabaseManager;

class UserRepository {
public:
    explicit UserRepository(DatabaseManager* manager);

    bool save(User& u);
    bool remove(int id);
    QList<User> findAll() const;
    std::optional<User> findById(int id);
    QList<User> searchUsers(const QString& query);
    std::optional<User> findByUsername(const QString& username);
    std::optional<User> authenticate(const QString& username, const QString& password);

private:
    DatabaseManager* dbManager;

    User fromQuery(QSqlQuery& q) const;
    QList<Genre> loadUserGenres(int userId) const;
};

#endif
