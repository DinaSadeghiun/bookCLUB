#include "shoppingcartrepository.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

ShoppingCartRepository::ShoppingCartRepository(DatabaseManager* manager)
    : dbManager(manager) {}

ShoppingCart ShoppingCartRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    int userId = q.value("user_id").toInt();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());

    ShoppingCart cart(userId);
    cart.setId(id);
    return cart;
}

bool ShoppingCartRepository::save(ShoppingCart& cart) {
    qDebug() << "=== ShoppingCartRepository::save called - cartId:" << cart.getId() << "userId:" << cart.getUserId();
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) {
        qDebug() << "ERROR: Failed to start transaction!";
        return false;
    }
    qDebug() << "Transaction started";

    QSqlQuery q(db);
    if (cart.getId() == -1) {
        q.prepare("INSERT INTO ShoppingCarts (user_id, created_at) VALUES (?, ?)");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());

        if (!q.exec()) {
            qDebug() << "ERROR: INSERT failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
        cart.setId(q.lastInsertId().toInt());
        qDebug() << "New cart inserted with ID:" << cart.getId();
    } else {
        q.prepare("UPDATE ShoppingCarts SET user_id = ?, created_at = ? WHERE id = ?");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());
        q.addBindValue(cart.getId());

        if (!q.exec()) {
            qDebug() << "ERROR: UPDATE failed:" << q.lastError().text();
            db.rollback();
            return false;
        }
        qDebug() << "Cart updated, ID:" << cart.getId();
    }

    if (!syncCartItems(q, cart.getId(), cart.getItemIds())) {
        qDebug() << "ERROR: syncCartItems failed for cartId:" << cart.getId();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "ERROR: Commit failed:" << db.lastError().text();
        return false;
    }
    qDebug() << "SUCCESS: Cart saved. cartId:" << cart.getId();
    return true;
}

bool ShoppingCartRepository::saveInternal(ShoppingCart& cart) {
    qDebug() << "=== saveInternal called - cartId:" << cart.getId() << "userId:" << cart.getUserId();
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    if (cart.getId() == -1) {
        q.prepare("INSERT INTO ShoppingCarts (user_id, created_at) VALUES (?, ?)");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());

        if (!q.exec()) {
            qDebug() << "ERROR: INSERT failed:" << q.lastError().text();
            return false;
        }
        cart.setId(q.lastInsertId().toInt());
        qDebug() << "New cart inserted with ID:" << cart.getId();
    } else {
        q.prepare("UPDATE ShoppingCarts SET user_id = ?, created_at = ? WHERE id = ?");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());
        q.addBindValue(cart.getId());

        if (!q.exec()) {
            qDebug() << "ERROR: UPDATE failed:" << q.lastError().text();
            return false;
        }
        qDebug() << "Cart updated, ID:" << cart.getId();
    }

    if (!syncCartItems(q, cart.getId(), cart.getItemIds())) {
        qDebug() << "ERROR: syncCartItems failed for cartId:" << cart.getId();
        return false;
    }

    qDebug() << "SUCCESS: saveInternal done. cartId:" << cart.getId();
    return true;
}

std::optional<ShoppingCart> ShoppingCartRepository::findById(int id) {
    qDebug() << "=== findById called for cartId:" << id;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT * FROM ShoppingCarts WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec() || !q.next()) {
        qDebug() << "Cart not found for id:" << id;
        return std::nullopt;
    }

    ShoppingCart cart = fromQuery(q);
    qDebug() << "Cart found id:" << id << "userId:" << cart.getUserId();
    QList<int> items = loadCartItems(id);
    for (int bookId : items) {
        cart.addBook(bookId);
    }
    qDebug() << "Loaded items:" << items;
    return cart;
}

std::optional<ShoppingCart> ShoppingCartRepository::findByUserId(int userId) {
    qDebug() << "=== findByUserId called for userId:" << userId;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT * FROM ShoppingCarts WHERE user_id = ?");
    q.addBindValue(userId);

    if (!q.exec() || !q.next()) {
        qDebug() << "No cart found for userId:" << userId;
        return std::nullopt;
    }

    ShoppingCart cart = fromQuery(q);
    qDebug() << "Cart found id:" << cart.getId() << "userId:" << userId;
    QList<int> items = loadCartItems(cart.getId());
    for (int bookId : items) {
        cart.addBook(bookId);
    }
    qDebug() << "Loaded items:" << items;
    return cart;
}

bool ShoppingCartRepository::remove(int id) {
    qDebug() << "=== remove called for cartId:" << id;
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM CartItems WHERE cart_id = ?");
    q.addBindValue(id);
    if (!q.exec()) {
        qDebug() << "ERROR: DELETE CartItems failed:" << q.lastError().text();
        db.rollback();
        return false;
    }

    q.prepare("DELETE FROM ShoppingCarts WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec() || q.numRowsAffected() == 0) {
        qDebug() << "ERROR: DELETE ShoppingCarts failed or no rows affected:" << q.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "ERROR: Commit failed:" << db.lastError().text();
        return false;
    }
    qDebug() << "SUCCESS: Cart removed. cartId:" << id;
    return db.commit();
}

bool ShoppingCartRepository::syncCartItems(QSqlQuery& q, int cartId, const QList<int>& itemIds) {
    qDebug() << "=== syncCartItems - cartId:" << cartId << "items:" << itemIds;

    q.prepare("DELETE FROM CartItems WHERE cart_id = ?");
    q.addBindValue(cartId);
    if (!q.exec()) {
        qDebug() << "ERROR: DELETE existing items failed:" << q.lastError().text();
        return false;
    }

    for (int bookId : itemIds) {
        q.clear();
        q.prepare("INSERT INTO CartItems (cart_id, book_id) VALUES (?, ?)");
        q.addBindValue(cartId);
        q.addBindValue(bookId);
        if (!q.exec()) {
            qDebug() << "ERROR: INSERT CartItems failed for bookId:" << bookId << "error:" << q.lastError().text();
            return false;
        }
        qDebug() << "Inserted bookId:" << bookId << "into cartId:" << cartId;
    }
    qDebug() << "SUCCESS: syncCartItems done for cartId:" << cartId;
    return true;
}

QList<int> ShoppingCartRepository::loadCartItems(int cartId) {
    QList<int> items;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM CartItems WHERE cart_id = ?");
    q.addBindValue(cartId);

    if (q.exec()) {
        while (q.next()) {
            items.append(q.value(0).toInt());
        }
        qDebug() << "loadCartItems for cartId:" << cartId << "found:" << items;
    } else {
        qDebug() << "ERROR: loadCartItems query failed:" << q.lastError().text();
    }
    return items;
}