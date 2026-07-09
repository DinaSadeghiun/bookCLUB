#include "AdminRepository.h"
#include "person.h"
#include "Admin.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

AdminRepository::AdminRepository(const QString& connectionName)
    : connName(connectionName) {}

Admin AdminRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    QString username = Person::decryptData(q.value("username").toString());
    QString password = q.value("password_hash").toString();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool active = q.value("is_active").toBool();
    QString secA = Person::decryptData(q.value("security_answer").toString());

    return Admin(id, username, password, created, active, secA);
}

bool AdminRepository::save(Admin& a) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);

    if (a.getId() == -1) {
        q.prepare("INSERT INTO Persons (username, password_hash, role, created_at, security_answer, is_active)"
                  " VALUES (:uname, :hash, :role, :created, :sec_a, :active)");
        q.bindValue(":uname", Person::encryptData(a.getUsername()));
        q.bindValue(":hash", a.getPasswordHash());
        q.bindValue(":role", ROLE_ADMIN);
        q.bindValue(":created", a.getCreatedAt().toSecsSinceEpoch());
        q.bindValue(":sec_a", Person::encryptData(a.getSecurityAnswer()));
        q.bindValue(":active", a.getIsActive() ? 1 : 0);

        if (!q.exec()) {
            qDebug() << "Admin Insert Failed:" << q.lastError().text();
            return false;
        }
        a.setId(q.lastInsertId().toInt());
    } else {
        q.prepare("UPDATE Persons SET username=:uname, password_hash=:hash, security_answer=:sec_a, is_active=:active "
                  "WHERE id=:id AND role=:role");
        q.bindValue(":uname", Person::encryptData(a.getUsername()));
        q.bindValue(":hash", a.getPasswordHash());
        q.bindValue(":sec_a", Person::encryptData(a.getSecurityAnswer()));
        q.bindValue(":active", a.getIsActive() ? 1 : 0);
        q.bindValue(":id", a.getId());
        q.bindValue(":role", ROLE_ADMIN);

        if (!q.exec()) {
            qDebug() << "Admin Update Failed:" << q.lastError().text();
            return false;
        }
    }
    return true;
}

bool AdminRepository::ensureDefaultAdmin() {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM Persons WHERE role = :role");
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec() && q.next() && q.value(0).toInt() == 0) {
        qDebug() << "No Admin found, creating default...";
        Admin admin("admin", "admin123", "1");
        return save(admin);
    }
    return true;
}

bool AdminRepository::remove(int adminId) {
    if (adminId <= 0) return false;
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery query(db);
    query.prepare("DELETE FROM Persons WHERE id = :id AND role = :role");
    query.bindValue(":id", adminId);
    query.bindValue(":role", ROLE_ADMIN);
    return query.exec() && query.numRowsAffected() > 0;
}

std::optional<Admin> AdminRepository::findById(int id) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM Persons WHERE id = :id AND role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec() && q.next()) return fromQuery(q);
    return std::nullopt;
}

std::optional<Admin> AdminRepository::findByUsername(const QString& username) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM Persons WHERE username = :uname AND role = :role");
    q.bindValue(":uname", Person::encryptData(username));
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec() && q.next()) return fromQuery(q);
    return std::nullopt;
}

std::optional<Admin> AdminRepository::authenticate(const QString& username, const QString& password) {
    auto adminOpt = findByUsername(username);
    if (adminOpt && adminOpt->verifyPassword(password)) {
        return adminOpt;
    }
    return std::nullopt;
}
