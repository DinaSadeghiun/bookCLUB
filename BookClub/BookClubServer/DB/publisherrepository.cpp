#include "publisherrepository.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

PublisherRepository::PublisherRepository(DatabaseManager* manager)
    : dbManager(manager) {}

Publisher PublisherRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    QString username = Person::decryptData(q.value("username").toString());
    QString pwHash = q.value("password_hash").toString();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool active = q.value("is_active").toBool();
    QString secA = Person::decryptData(q.value("security_answer").toString());

    double revenue = q.value("revenue").toDouble();

    Publisher pub(id, username, pwHash, created, active, secA);
    pub.setRevenue(revenue);
    return pub;
}

bool PublisherRepository::save(Publisher& pub) {
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) return false;

    bool ok = saveInternal(pub);
    if (!ok) {
        db.rollback();
        return false;
    }
    return db.commit();
}

bool PublisherRepository::saveInternal(Publisher& pub) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    bool isInsert = (pub.getId() == -1);

    if (isInsert) {
        q.prepare("INSERT INTO Persons (username, password_hash, role, created_at, security_answer, is_active)"
                  " VALUES (:uname, :hash, :role, :created, :sec_a, :active)");
        q.bindValue(":uname", Person::encryptData(pub.getUsername()));
        q.bindValue(":hash", pub.getPasswordHash());
        q.bindValue(":role", ROLE_PUBLISHER);
        q.bindValue(":created", pub.getCreatedAt().toSecsSinceEpoch());
        q.bindValue(":sec_a", Person::encryptData(pub.getSecurityAnswer()));
        q.bindValue(":active", pub.getIsActive() ? 1 : 0);

        if (!q.exec()) {
            qDebug() << "Publisher save (Persons) failed:" << q.lastError().text();
            return false;
        }
        pub.setId(q.lastInsertId().toInt());

        q.prepare("INSERT INTO Publishers (person_id, revenue) VALUES (:pid, :rev)");
        q.bindValue(":pid", pub.getId());
        q.bindValue(":rev", pub.getRevenue());

        if (!q.exec()) {
            return false;
        }
    } else {
        q.prepare("UPDATE Persons SET username=:uname, password_hash=:hash, "
                  "security_answer=:sec_a, is_active=:active WHERE id=:id AND role=:role");
        q.bindValue(":uname", Person::encryptData(pub.getUsername()));
        q.bindValue(":hash", pub.getPasswordHash());
        q.bindValue(":sec_a", Person::encryptData(pub.getSecurityAnswer()));
        q.bindValue(":active", pub.getIsActive() ? 1 : 0);
        q.bindValue(":id", pub.getId());
        q.bindValue(":role", ROLE_PUBLISHER);

        if (!q.exec()) {
            return false;
        }

        q.prepare("UPDATE Publishers SET revenue=:rev WHERE person_id=:pid");
        q.bindValue(":rev", pub.getRevenue());
        q.bindValue(":pid", pub.getId());

        if (!q.exec()) {
            return false;
        }
    }

    return true;
}

QList<Publisher> PublisherRepository::findAll() const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    q.prepare("SELECT p.*, pub.revenue FROM Persons p "
              "JOIN Publishers pub ON p.id = pub.person_id "
              "WHERE p.role = :role");

    q.bindValue(":role", ROLE_PUBLISHER);

    QList<Publisher> result;
    if (q.exec()) {
        while (q.next()) {
            result.append(fromQuery(q));
        }
    }
    return result;
}

QList<Publisher> PublisherRepository::searchPublishers(const QString& queryStr) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    q.prepare("SELECT p.*, pub.revenue FROM Persons p "
              "JOIN Publishers pub ON p.id = pub.person_id "
              "WHERE p.role = :role AND p.username = :uname");

    q.bindValue(":role", ROLE_PUBLISHER);
    q.bindValue(":uname", Person::encryptData(queryStr));

    QList<Publisher> result;
    if (q.exec()) {
        while (q.next()) {
            result.append(fromQuery(q));
        }
    }
    return result;
}


std::optional<Publisher> PublisherRepository::findById(int id) const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT p.*, pub.revenue FROM Persons p "
              "JOIN Publishers pub ON p.id = pub.person_id "
              "WHERE p.id = :id AND p.role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_PUBLISHER);

    if (q.exec() && q.next()) {
        return fromQuery(q);
    }
    return std::nullopt;
}

std::optional<Publisher> PublisherRepository::findByUsername(const QString& username) const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT p.*, pub.revenue FROM Persons p "
              "JOIN Publishers pub ON p.id = pub.person_id "
              "WHERE p.username = :uname AND p.role = :role");
    q.bindValue(":uname", Person::encryptData(username));
    q.bindValue(":role", ROLE_PUBLISHER);

    if (q.exec() && q.next()) {
        return fromQuery(q);
    }
    return std::nullopt;
}

std::optional<Publisher> PublisherRepository::authenticate(const QString& username, const QString& password) {
    auto pubOpt = findByUsername(username);
    if (pubOpt && pubOpt->verifyPassword(password)) {
        return pubOpt;
    }
    return std::nullopt;
}


bool PublisherRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("DELETE FROM Persons WHERE id = :id AND role = :role");
    q.bindValue(":id", id);
    q.bindValue(":role", ROLE_PUBLISHER);
    return q.exec() && q.numRowsAffected() > 0;
}