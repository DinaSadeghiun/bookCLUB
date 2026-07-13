#include "AdminRepository.h"
#include "databasemanager.h"
#include "person.h"
#include "Admin.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

AdminRepository::AdminRepository(DatabaseManager* manager)
    : dbManager(manager) {}


Admin AdminRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    QString username = Person::decryptData(q.value("username").toString());
    QString passwordHash = q.value("password_hash").toString();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool active = q.value("is_active").toBool();
    QString secA = Person::decryptData(q.value("security_answer").toString());

    return Admin(id, username, passwordHash, created, active, secA);
}

bool AdminRepository::save(Admin& a) {
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) {
        qCritical() << "Failed to start transaction:" << db.lastError().text();
        return false;
    }

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
            qCritical() << "Admin Insert Failed:" << q.lastError().text();
            db.rollback();
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
            qCritical() << "Admin Update Failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }
    return db.commit();
}

bool AdminRepository::ensureDefaultAdmin() {
    QSqlDatabase db = dbManager->getDatabase();
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
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) {
        qCritical() << "Failed to start transaction:" << db.lastError().text();
        return false;
    }
    QSqlQuery q(db);

    q.prepare("DELETE FROM Persons WHERE id = :id AND role = :role");
    q.bindValue(":id", adminId);
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec()) {
        if (q.numRowsAffected() > 0) {
            return db.commit();
        } else {
            qDebug() << "No admin found with ID:" << adminId << "or role mismatch.";
            db.rollback();
            return false;
        }
    } else {
        qCritical() << "Admin Remove Failed:" << q.lastError().text();
        db.rollback();
        return false;
    }
}

std::optional<Admin> AdminRepository::findById(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    // Explicitly select necessary columns
    q.prepare("SELECT id, username, password_hash, created_at, is_active, security_answer FROM Persons WHERE id = :id AND role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec() && q.next()) return fromQuery(q);
    if (q.lastError().isValid()) {
        qCritical() << "findById failed:" << q.lastError().text();
    }
    return std::nullopt;
}

std::optional<Admin> AdminRepository::findByUsername(const QString& username) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    // Explicitly select necessary columns
    q.prepare("SELECT id, username, password_hash, created_at, is_active, security_answer FROM Persons WHERE username = :uname AND role = :role");
    q.bindValue(":uname", Person::encryptData(username));
    q.bindValue(":role", ROLE_ADMIN);

    if (q.exec() && q.next()) return fromQuery(q);
    if (q.lastError().isValid()) {
        qCritical() << "findByUsername failed:" << q.lastError().text();
    }
    return std::nullopt;
}

std::optional<Admin> AdminRepository::authenticate(const QString& username, const QString& password) {
    auto adminOpt = findByUsername(username);
    if (adminOpt && adminOpt->verifyPassword(password)) {
        return adminOpt;
    }
    return std::nullopt;
}
