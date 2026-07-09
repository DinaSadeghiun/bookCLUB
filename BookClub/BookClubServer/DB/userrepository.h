#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "user.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>

class UserRepository {
private:
    QString connName; // Safer than QSqlDatabase&
    static constexpr char ROLE_USER[] = "User"; // Centralized role name

    User fromQuery(QSqlQuery& q) const;
    QList<Genre> loadUserGenres(int userId) const;

public:
    explicit UserRepository(const QString& connectionName);

    // may set id on insert, hence non-const
    bool save(User& u);
    bool remove(int id);
    std::optional<User> findById(int id);
    std::optional<User> findByUsername(const QString& username);
    std::optional<User> authenticate(const QString& username, const QString& password);
};

#endif
