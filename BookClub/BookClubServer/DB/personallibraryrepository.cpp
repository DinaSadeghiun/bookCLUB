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

QList<int> PersonalLibraryRepository::loadFaveBooks(int userId) const {
    // ===== LOG =====
    qDebug() << "=== loadFaveBooks START ===";
    qDebug() << "userId:" << userId;
    // ===============

    QList<int> faveList;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT book_id FROM Favorites WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            faveList.append(query.value(0).toInt());
        }
        // ===== LOG =====
        qDebug() << "faveList IDs:" << faveList;
        qDebug() << "faveList count:" << faveList.size();
        // ===============
    } else {
        qWarning() << "Failed to load favorites for user" << userId << ":" << query.lastError().text();
    }

    // ===== LOG =====
    qDebug() << "=== loadFaveBooks END ===";
    // ===============

    return faveList;
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
    if (!q.exec() || !q.next()) {
        return std::nullopt;
    }

    PersonalLibrary lib(userId);

    for (int id : loadPurchasedBooks(userId)) lib.addPurchasedBook(id);

    for (int id : loadWishlist(userId)) lib.addToWishlist(id);

    QList<int> favorites = loadFaveBooks(userId);
    qDebug() << "favorites loaded:" << favorites.size() << "books";
    for (int id : std::as_const(favorites)) lib.addToFaveBooks(id);

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

    // Sync Wishlist
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

    // Sync Favorites
    QSet<int> dbFave, memFave;
    QSqlQuery gfq(db);
    gfq.prepare("SELECT book_id FROM Favorites WHERE user_id = ?");
    gfq.addBindValue(userId);
    if (gfq.exec()) while (gfq.next()) dbFave.insert(gfq.value(0).toInt());
    for (int id : lib.getFaveBooks()) memFave.insert(id);

    for (int bid : memFave) {
        if (dbFave.contains(bid)) continue;
        QSqlQuery q(db);
        q.prepare("INSERT INTO Favorites (user_id, book_id, added_at) VALUES (?, ?, ?)");
        q.addBindValue(userId); q.addBindValue(bid); q.addBindValue(QDateTime::currentSecsSinceEpoch());
        if (!q.exec()) { db.rollback(); return false; }
    }
    for (int bid : dbFave) {
        if (memFave.contains(bid)) continue;
        QSqlQuery q(db);
        q.prepare("DELETE FROM Favorites WHERE user_id = ? AND book_id = ?");
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

bool PersonalLibraryRepository::addToWishlist(int userId, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO Wishlist (user_id, book_id, added_at) VALUES (:userId, :bookId, :addedAt)");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":addedAt", QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "Failed to add book to wishlist:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PersonalLibraryRepository::removeFromWishlist(int userId, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Wishlist WHERE user_id = :userId AND book_id = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);

    if (!query.exec()) {
        qWarning() << "Failed to remove book from wishlist:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool PersonalLibraryRepository::addToFaveBooks(int userId, int bookId) {
    qDebug() << "=== Repository addToFaveBooks - userId:" << userId << "bookId:" << bookId;

    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO Favorites (user_id, book_id, added_at) VALUES (:userId, :bookId, :addedAt)");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":addedAt", QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
        qDebug() << "ERROR: addToFaveBooks query failed:" << query.lastError().text();
        return false;
    }

    int affected = query.numRowsAffected();
    qDebug() << "addToFaveBooks affected rows:" << affected;

    return true;
}

bool PersonalLibraryRepository::removeFromFaveBooks(int userId, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Favorites WHERE user_id = :userId AND book_id = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);

    if (!query.exec()) {
        qWarning() << "Failed to remove book from Favorites:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool PersonalLibraryRepository::addShelf(int userId, const QString& shelfName) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO CustomShelves (user_id, name) VALUES (:userId, :name)");
    query.bindValue(":userId", userId);
    query.bindValue(":name", shelfName.trimmed());

    if (!query.exec()) {
        qWarning() << "Failed to add shelf:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PersonalLibraryRepository::removeShelf(int userId, const QString& shelfName) {
    QSqlDatabase db = dbManager->getDatabase();

    int shelfId = resolveShelfId(userId, shelfName);
    if (shelfId == -1) return false;

    db.transaction();

    QSqlQuery deleteBooksQuery(db);
    deleteBooksQuery.prepare("DELETE FROM ShelfBooks WHERE shelf_id = :shelfId");
    deleteBooksQuery.bindValue(":shelfId", shelfId);
    if (!deleteBooksQuery.exec()) {
        db.rollback();
        qWarning() << "Failed to clear shelf books before removing shelf:" << deleteBooksQuery.lastError().text();
        return false;
    }

    QSqlQuery deleteShelfQuery(db);
    deleteShelfQuery.prepare("DELETE FROM CustomShelves WHERE id = :shelfId");
    deleteShelfQuery.bindValue(":shelfId", shelfId);
    if (!deleteShelfQuery.exec()) {
        db.rollback();
        qWarning() << "Failed to delete shelf:" << deleteShelfQuery.lastError().text();
        return false;
    }

    db.commit();
    return true;
}

bool PersonalLibraryRepository::addBookToShelf(int userId, const QString& shelfName, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    int shelfId = resolveShelfId(userId, shelfName);
    if (shelfId == -1) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO ShelfBooks (shelf_id, book_id) VALUES (:shelfId, :bookId)");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);

    if (!query.exec()) {
        qWarning() << "Failed to add book to shelf:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PersonalLibraryRepository::removeBookFromShelf(int userId, const QString& shelfName, int bookId) {
    QSqlDatabase db = dbManager->getDatabase();
    int shelfId = resolveShelfId(userId, shelfName);
    if (shelfId == -1) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM ShelfBooks WHERE shelf_id = :shelfId AND book_id = :bookId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);

    if (!query.exec()) {
        qWarning() << "Failed to remove book from shelf:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool PersonalLibraryRepository::renameShelf(int userId, const QString& oldName, const QString& newName) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE CustomShelves SET name = :newName WHERE user_id = :userId AND name = :oldName");
    query.bindValue(":newName", newName.trimmed());
    query.bindValue(":userId", userId);
    query.bindValue(":oldName", oldName.trimmed());
    return query.exec();
}

