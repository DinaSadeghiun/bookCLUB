#include "UserRepository.h"
#include "databasemanager.h"
#include "person.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

UserRepository::UserRepository(DatabaseManager* manager)
    : dbManager(manager) {}


User UserRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();

    QString username = Person::decryptData(q.value("username").toString());
    QString password = q.value("password_hash").toString();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool active = q.value("is_active").toBool();
    QString secA = Person::decryptData(q.value("security_answer").toString());
    double balance = q.value("wallet_balance").toDouble();

    QList<Genre> genres = loadUserGenres(id);

    return User(id, username, password, created, active, secA, balance, genres);
}

QList<Genre> UserRepository::loadUserGenres(int userId) const {
    QList<Genre> genres;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT genre FROM UserGenres WHERE user_id = ?");
    q.addBindValue(userId);

    if (q.exec()) {
        while (q.next()) {
            genres.append(static_cast<Genre>(q.value(0).toInt()));
        }
    }
    return genres;
}

bool UserRepository::save(User& u) {
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    bool isInsert = (u.getId() == -1);

    if (isInsert) {
        // Persons
        q.prepare("INSERT INTO Persons (username, password_hash, role, created_at, security_answer, is_active)"
                  " VALUES (:uname, :hash, :role, :created, :sec_a, :active)");
        q.bindValue(":uname", Person::encryptData(u.getUsername()));
        q.bindValue(":hash", u.getPasswordHash());
        q.bindValue(":role", ROLE_USER);
        q.bindValue(":created", u.getCreatedAt().toSecsSinceEpoch());
        q.bindValue(":sec_a", Person::encryptData(u.getSecurityAnswer()));
        q.bindValue(":active", u.getIsActive() ? 1 : 0);

        if (!q.exec()) {
            db.rollback();
            return false;
        }
        u.setId(q.lastInsertId().toInt());

        // Users
        q.prepare("INSERT INTO Users (person_id, wallet_balance) VALUES (:pid, :wallet)");
        q.bindValue(":pid", u.getId());
        q.bindValue(":wallet", u.getWalletBalance());
        if (!q.exec()) {
            db.rollback();
            return false;
        }
    } else {
        // Persons
        q.prepare("UPDATE Persons SET username=:uname, password_hash=:hash,"
                  " security_answer=:sec_a, is_active=:active WHERE id=:id AND role=:role");
        q.bindValue(":uname", Person::encryptData(u.getUsername()));
        q.bindValue(":hash", u.getPasswordHash());
        q.bindValue(":sec_a", Person::encryptData(u.getSecurityAnswer()));
        q.bindValue(":active", u.getIsActive() ? 1 : 0);
        q.bindValue(":id", u.getId());
        q.bindValue(":role", ROLE_USER);
        if (!q.exec()) {
            db.rollback();
            return false;
        }

        // Users
        q.prepare("UPDATE Users SET wallet_balance=:wallet WHERE person_id=:pid");
        q.bindValue(":wallet", u.getWalletBalance());
        q.bindValue(":pid", u.getId());
        if (!q.exec()) {
            db.rollback();
            return false;
        }
    }

    QSqlQuery dq(db);
    dq.prepare("DELETE FROM UserGenres WHERE user_id = ?");
    dq.addBindValue(u.getId());
    if (!dq.exec()) { db.rollback(); return false; }

    for (auto g : u.getFavoriteGenres()) {
        QSqlQuery iq(db);
        iq.prepare("INSERT INTO UserGenres (user_id, genre) VALUES (:uid, :genre)");
        iq.bindValue(":uid", u.getId());
        iq.bindValue(":genre", static_cast<int>(g));
        if (!iq.exec()) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}

std::optional<User> UserRepository::findById(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT p.*, u.wallet_balance FROM Persons p "
              "JOIN Users u ON p.id = u.person_id "
              "WHERE p.id = :id AND p.role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_USER);

    if (q.exec() && q.next()) {
        return fromQuery(q);
    }
    return std::nullopt;
}

std::optional<User> UserRepository::findByUsername(const QString& username) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT p.*, u.wallet_balance FROM Persons p "
              "JOIN Users u ON p.id = u.person_id "
              "WHERE p.username = :uname AND p.role = :role");
    q.bindValue(":uname", Person::encryptData(username));
    q.bindValue(":role", ROLE_USER);

    if (q.exec() && q.next()) {
        return fromQuery(q);
    }
    return std::nullopt;
}

bool UserRepository::remove(int userId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("DELETE FROM Persons WHERE id = :id AND role = :role");
    q.bindValue(":id", userId);
    q.bindValue(":role", ROLE_USER);
    return q.exec() && q.numRowsAffected() > 0;
}

std::optional<User> UserRepository::authenticate(const QString& username, const QString& password) {
    auto userOpt = findByUsername(username);
    if (userOpt && userOpt->verifyPassword(password)) {
        return userOpt;
    }
    return std::nullopt;
}
