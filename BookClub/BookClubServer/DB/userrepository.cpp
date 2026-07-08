#include "UserRepository.h"
#include "Person.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>


UserRepository::UserRepository(const QString& connectionName)
    : connName(connectionName) {}

User UserRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    QList<Genre> genres = loadUserGenres(id);

    QString username = Person::decryptData(q.value("username").toString());
    QString password = q.value("password_hash").toString();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool active = q.value("is_active").toBool();
    QString secA = Person::decryptData(q.value("security_answer").toString());
    double balance = q.value("wallet_balance").toDouble();

    return User(id, username, password, created, active, secA, balance, genres);
}


QList<Genre> UserRepository::loadUserGenres(int userId) const {
    QList<Genre> genres;
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT genre FROM UserGenres WHERE user_id = ?");
    q.addBindValue(userId);

    if (q.exec()) {
        while (q.next()) {
            genres.append(static_cast<Genre>(q.value(0).toInt()));
        }
    } else {
        qDebug() << "Load User Genres Failed:" << q.lastError().text();
    }
    return genres;
}

bool UserRepository::save(User& u) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);

    if (!db.isOpen()) {
        qDebug() << "Database is not open! Connection name:" << (connName.isEmpty() ? "default" : connName);
        return false;
    }

    if (!db.transaction()) {
        qDebug() << "Transaction failed to start:" << db.lastError().text();
        return false;
    }

    QSqlQuery q(db);
    if (u.getId() == -1) {
        q.prepare("INSERT INTO Persons (username, password_hash, role, created_at, security_answer, is_active)"
                  " VALUES (:uname, :hash, :role, :created, :sec_a, :active)");
        q.bindValue(":uname", Person::encryptData(u.getUsername()));
        q.bindValue(":hash", u.getPasswordHash());
        q.bindValue(":role", ROLE_USER);
        q.bindValue(":created", u.getCreatedAt().toSecsSinceEpoch());
        q.bindValue(":sec_a", Person::encryptData(u.getSecurityAnswer()));
        q.bindValue(":active", u.getIsActive() ? 1 : 0);
        //genres??
        if (!q.exec()) {
            qDebug() << "Persons Insert Failed (User):" << q.lastError().text();
            db.rollback();
            return false;
        }
        u.setId(q.lastInsertId().toInt());

        q.prepare("INSERT INTO Users (person_id, wallet_balance) VALUES (:pid, :wallet)");
        q.bindValue(":pid", u.getId());
        q.bindValue(":wallet", u.getWalletBalance());
        if (!q.exec()) {
            qDebug() << "Users Insert Failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
    } else {
        // Update logic
        q.prepare("UPDATE Persons SET username=:uname, password_hash=:hash, security_answer=:sec_a, is_active=:active WHERE id=:id AND role=:role");
        q.bindValue(":uname", Person::encryptData(u.getUsername()));
        q.bindValue(":hash", u.getPasswordHash());
        q.bindValue(":sec_a", Person::encryptData(u.getSecurityAnswer()));
        q.bindValue(":active", u.getIsActive() ? 1 : 0);
        q.bindValue(":id", u.getId());
        q.bindValue(":role", "User");
        if (!q.exec()) {
            qDebug() << "Persons Update Failed";
            db.rollback();
            return false;
        }
        q.prepare("UPDATE Users SET wallet_balance=:wallet WHERE person_id=:pid");
        q.bindValue(":wallet", u.getWalletBalance());
        q.bindValue(":pid", u.getId());
        if (!q.exec()) {
            qDebug() << "Users Update Failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Save Genres
    q.prepare("DELETE FROM UserGenres WHERE user_id = ?");
    q.addBindValue(u.getId());
    q.exec();

    for (auto g : u.getFavoriteGenres()) {
        q.prepare("INSERT INTO UserGenres (user_id, genre) VALUES (:uid, :genre)");
        q.bindValue(":uid", u.getId());
        q.bindValue(":genre", static_cast<int>(g));
        if (!q.exec()) {
            qDebug() << "Genre Insert Failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        qDebug() << "Commit failed:" << db.lastError().text();
        return false;
    }
    return true;
}

std::optional<User> UserRepository::findById(int id) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT p.*, u.wallet_balance FROM Persons p "
              "JOIN Users u ON p.id = u.person_id "
              "WHERE p.id = :id AND p.role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_USER);

    if (q.exec()) {
        if (q.next()) {
            return fromQuery(q);
        }
    } else {
        qDebug() << "findById Query Failed:" << q.lastError().text();
    }
    return std::nullopt;
}

std::optional<User> UserRepository::findByUsername(const QString& username) {
    QSqlDatabase db = connName.isEmpty() ? QSqlDatabase::database() : QSqlDatabase::database(connName);
    QSqlQuery q(db);
    q.prepare("SELECT p.*, u.wallet_balance FROM Persons p "
              "JOIN Users u ON p.id = u.person_id "
              "WHERE p.username = :uname AND p.role = :role");
    q.bindValue(":uname", Person::encryptData(username));
    q.bindValue(":role", ROLE_USER);

    if (q.exec()) {
        if (q.next()) {
            return fromQuery(q);
        }
    } else {
        qDebug() << "findByUsername Query Failed:" << q.lastError().text();
    }
    return std::nullopt;
}

//remove
bool UserRepository::remove(int userId) {
    if (userId <= 0) return false;

    QSqlDatabase db = connName.isEmpty()
                          ? QSqlDatabase::database()
                          : QSqlDatabase::database(connName);

    QSqlQuery query(db);
    query.prepare("DELETE FROM Persons WHERE id = :userId");
    query.bindValue(":userId", userId);
    return query.exec() && query.numRowsAffected() > 0;
}


//auth
std::optional<User> UserRepository::authenticate(const QString& username, const QString& password) {
    auto userOpt = findByUsername(username);

    if (userOpt) {
        if (userOpt->verifyPassword(password)) {
            return userOpt;
        }
    }

    return std::nullopt;
}

