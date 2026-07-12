#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "user.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>

inline const QString ROLE_USER = "User";

class UserRepository {
public:
    explicit UserRepository();

    bool save(User& u);
    bool remove(int id);
    std::optional<User> findById(int id);
    std::optional<User> findByUsername(const QString& username);
    std::optional<User> authenticate(const QString& username, const QString& password);

private:
    QSqlDatabase db;

    User fromQuery(QSqlQuery& q) const;
    QList<Genre> loadUserGenres(int userId) const;
};

#endif
