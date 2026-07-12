#ifndef ADMINREPOSITORY_H
#define ADMINREPOSITORY_H

#include "Admin.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>
inline const QString ROLE_ADMIN = "Admin";

class AdminRepository {
private:
    QSqlDatabase db;

    Admin fromQuery(QSqlQuery& q) const;

public:
    explicit AdminRepository();

    bool save(Admin& a);
    bool remove(int id);
    std::optional<Admin> findById(int id);
    std::optional<Admin> findByUsername(const QString& username);
    std::optional<Admin> authenticate(const QString& username, const QString& password);

    bool ensureDefaultAdmin();
};

#endif
