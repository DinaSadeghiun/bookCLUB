#include "discountrepository.h"
#include "databasemanager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

DiscountRepository::DiscountRepository(DatabaseManager* manager)
    : dbManager(manager) {}

bool DiscountRepository::save(Discount& discount) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    if (discount.getId() == -1) {
        q.prepare("INSERT INTO Discounts (value, type, start_date, end_date, is_active) "
                  "VALUES (:value, :type, :start_date, :end_date, :is_active)");
    } else {
        q.prepare("UPDATE Discounts SET value = :value, type = :type, "
                  "start_date = :start_date, end_date = :end_date, is_active = :is_active "
                  "WHERE id = :id");
        q.bindValue(":id", discount.getId());
    }

    q.bindValue(":value", discount.getValue());
    q.bindValue(":type", static_cast<int>(discount.getType()));
    q.bindValue(":start_date", discount.getStartDate().toSecsSinceEpoch());
    q.bindValue(":end_date", discount.getEndDate().toSecsSinceEpoch());
    q.bindValue(":is_active", discount.getIsActive() ? 1 : 0);

    if (!q.exec()) {
        qCritical() << "Discount save failed:" << q.lastError().text();
        return false;
    }

    if (discount.getId() == -1) {
        discount.setId(q.lastInsertId().toInt());
    }
    return true;
}

bool DiscountRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("DELETE FROM Discounts WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec() && q.numRowsAffected() > 0;
}

std::optional<Discount> DiscountRepository::findById(int id) const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, value, type, start_date, end_date, is_active FROM Discounts WHERE id = :id");
    q.bindValue(":id", id);

    if (q.exec() && q.next()) {
        return Discount(
            q.value("id").toInt(),
            q.value("value").toDouble(),
            static_cast<Discount::DiscountType>(q.value("type").toInt()),
            QDateTime::fromSecsSinceEpoch(q.value("start_date").toLongLong()),
            QDateTime::fromSecsSinceEpoch(q.value("end_date").toLongLong()),
            q.value("is_active").toInt() == 1
            );
    }
    return std::nullopt;
}

QList<Discount> DiscountRepository::findAllActive() const {
    QList<Discount> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, value, type, start_date, end_date, is_active FROM Discounts WHERE is_active = 1");

    if (!q.exec()) {
        qCritical() << "findAllActive failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        list.append(Discount(
            q.value("id").toInt(),
            q.value("value").toDouble(),
            static_cast<Discount::DiscountType>(q.value("type").toInt()),
            QDateTime::fromSecsSinceEpoch(q.value("start_date").toLongLong()),
            QDateTime::fromSecsSinceEpoch(q.value("end_date").toLongLong()),
            q.value("is_active").toInt() == 1
            ));
    }
    return list;
}

QList<Discount> DiscountRepository::findAll() const {
    QList<Discount> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, value, type, start_date, end_date, is_active FROM Discounts");

    if (!q.exec()) {
        qCritical() << "findAll failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        list.append(Discount(
            q.value("id").toInt(),
            q.value("value").toDouble(),
            static_cast<Discount::DiscountType>(q.value("type").toInt()),
            QDateTime::fromSecsSinceEpoch(q.value("start_date").toLongLong()),
            QDateTime::fromSecsSinceEpoch(q.value("end_date").toLongLong()),
            q.value("is_active").toInt() == 1
            ));
    }
    return list;
}
