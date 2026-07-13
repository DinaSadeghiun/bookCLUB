#include "DB/personallibraryrepository.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>
#include <QSet>

PersonalLibraryRepository::PersonalLibraryRepository(DatabaseManager* manager)
    : dbManager(manager) {}

QList<int> PersonalLibraryRepository::loadPurchasedBooks(int userId) const {
    QList<int> books;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM PurchasedBooks WHERE user_id = ?");
    q.addBindValue(userId);
    if (q.exec())
        while (q.next()) books.append(q.value(0).toInt());
    return books;
}

QList<int> PersonalLibraryRepository::loadWishlist(int userId) const {
    QList<int> books;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM Wishlist WHERE user_id = ?");
    q.addBindValue(userId);
    if (q.exec())
        while (q.next()) books.append(q.value(0).toInt());
    return books;
}

QList<QPair<QString, QList<int>>> PersonalLibraryRepository::loadShelves(int userId) const {
    QList<QPair<QString, QList<int>>> result;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, name FROM CustomShelves WHERE user_id = ?");
    q.addBindValue(userId);

    if (!q.exec()) return result;

    struct Row { int id; QString name; };
    QList<Row> rows;
    while (q.next())
        rows.append({q.value(0).toInt(), q.value(1).toString()});

    for (const auto& row : rows) {
        QList<int> bookIds;
        QSqlQuery bq(db);
        bq.prepare("SELECT book_id FROM ShelfBooks WHERE shelf_id = ?");
        bq.addBindValue(row.id);
        if (bq.exec())
            while (bq.next()) bookIds.append(bq.value(0).toInt());
        result.append({row.name, bookIds});
    }
    return result;
}

std::optional<PersonalLibrary> PersonalLibraryRepository::findByUserId(int userId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT person_id FROM Users WHERE person_id = :id");
    q.bindValue(":id", userId);
    if (!q.exec() || !q.next()) return std::nullopt;

    PersonalLibrary lib(userId);
    for (int id : loadPurchasedBooks(userId)) lib.addPurchasedBook(id);
    for (int id : loadWishlist(userId)) lib.addToWishlist(id);
    for (const auto& [name, bookIds] : loadShelves(userId)) {
        auto* shelf = lib.createShelf(name);
        if (shelf) for (int bid : bookIds) shelf->addBook(bid);
    }
    return lib;
}

int PersonalLibraryRepository::resolveShelfId(int userId, const QString& name) const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id FROM CustomShelves WHERE user_id = :uid AND name = :name");
    q.bindValue(":uid", userId);
    q.bindValue(":name", name);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

bool PersonalLibraryRepository::save(const PersonalLibrary& lib) {
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) return false;

    int userId = lib.getUserId();

    // 1. Sync Wishlist (Diff logic)
    QSet<int> dbWish, memWish;
    QSqlQuery gwq(db);
    gwq.prepare("SELECT book_id FROM Wishlist WHERE user_id = ?");
    gwq.addBindValue(userId);
    if (gwq.exec()) while (gwq.next()) dbWish.insert(gwq.value(0).toInt());
    for (int id : lib.getWishlist()) memWish.insert(id);

    for (int bid : memWish) {
        if (dbWish.contains(bid)) continue;
        QSqlQuery q(db);
        q.prepare("INSERT INTO Wishlist (user_id, book_id, added_at) VALUES (?, ?, ?)");
        q.addBindValue(userId); q.addBindValue(bid); q.addBindValue(QDateTime::currentSecsSinceEpoch());
        if (!q.exec()) { db.rollback(); return false; }
    }
    for (int bid : dbWish) {
        if (memWish.contains(bid)) continue;
        QSqlQuery q(db);
        q.prepare("DELETE FROM Wishlist WHERE user_id = ? AND book_id = ?");
        q.addBindValue(userId); q.addBindValue(bid);
        if (!q.exec()) { db.rollback(); return false; }
    }

    // 2. Sync Shelves
    QSqlQuery gsq(db);
    gsq.prepare("SELECT name FROM CustomShelves WHERE user_id = ?");
    gsq.addBindValue(userId);
    if (!gsq.exec()) { db.rollback(); return false; }

    QList<QString> dbShelfNames;
    while (gsq.next()) dbShelfNames.append(gsq.value(0).toString());

    for (const QString& name : dbShelfNames) {
        if (lib.findShelf(name)) continue;
        QSqlQuery q(db);
        q.prepare("DELETE FROM CustomShelves WHERE user_id = ? AND name = ?");
        q.addBindValue(userId); q.addBindValue(name);
        if (!q.exec()) { db.rollback(); return false; }
    }

    for (auto* shelf : lib.getCustomShelves()) {
        QSqlQuery ins(db);
        ins.prepare("INSERT OR IGNORE INTO CustomShelves (user_id, name) VALUES (?, ?)");
        ins.addBindValue(userId); ins.addBindValue(shelf->getShelfName());
        if (!ins.exec()) { db.rollback(); return false; }

        int shelfId = resolveShelfId(userId, shelf->getShelfName());
        QSqlQuery clr(db);
        clr.prepare("DELETE FROM ShelfBooks WHERE shelf_id = ?");
        clr.addBindValue(shelfId);
        if (!clr.exec()) { db.rollback(); return false; }

        for (int bid : shelf->getBookIds()) {
            QSqlQuery q(db);
            q.prepare("INSERT INTO ShelfBooks (shelf_id, book_id) VALUES (?, ?)");
            q.addBindValue(shelfId); q.addBindValue(bid);
            if (!q.exec()) { db.rollback(); return false; }
        }
    }

    return db.commit();
}

bool PersonalLibraryRepository::addPurchasedBook(int userId, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO PurchasedBooks (user_id, book_id, purchased_at) VALUES (?, ?, ?)");
    q.addBindValue(userId); q.addBindValue(bookId); q.addBindValue(QDateTime::currentSecsSinceEpoch());
    return q.exec();
}
