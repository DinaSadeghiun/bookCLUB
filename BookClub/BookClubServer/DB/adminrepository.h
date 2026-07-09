#ifndef ADMINREPOSITORY_H
#define ADMINREPOSITORY_H

#include "Admin.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>

class AdminRepository {
private:
    QString connName;
    static constexpr char ROLE_ADMIN[] = "Admin";

    Admin fromQuery(QSqlQuery& q) const;

public:
    explicit AdminRepository(const QString& connectionName = "");

    bool save(Admin& a);
    bool remove(int id);
    std::optional<Admin> findById(int id);
    std::optional<Admin> findByUsername(const QString& username);
    std::optional<Admin> authenticate(const QString& username, const QString& password);

    bool ensureDefaultAdmin();
};

#endif
