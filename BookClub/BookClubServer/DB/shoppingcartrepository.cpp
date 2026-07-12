#include "shoppingcartrepository.h"
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

ShoppingCartRepository::ShoppingCartRepository() {
    db = QSqlDatabase::database("bookclub_db");
}

ShoppingCart ShoppingCartRepository::fromQuery(QSqlQuery& q) const {
    int id = q.value("id").toInt();
    int userId = q.value("user_id").toInt();
    QDateTime created = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());

    ShoppingCart cart(userId);
    cart.setId(id);
    return cart;
}

bool ShoppingCartRepository::save(ShoppingCart& cart) {
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    if (cart.getId() == -1) {
        q.prepare("INSERT INTO ShoppingCarts (user_id, created_at) VALUES (?, ?)");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());

        if (!q.exec()) {
            db.rollback();
            return false;
        }
        cart.setId(q.lastInsertId().toInt());
    } else {
        q.prepare("UPDATE ShoppingCarts SET user_id = ?, created_at = ? WHERE id = ?");
        q.addBindValue(cart.getUserId());
        q.addBindValue(cart.getCreatedAt().toSecsSinceEpoch());
        q.addBindValue(cart.getId());

        if (!q.exec()) {
            db.rollback();
            return false;
        }
    }

    if (!syncCartItems(cart.getId(), cart.getItemIds())) {
        db.rollback();
        return false;
    }

    return db.commit();
}

std::optional<ShoppingCart> ShoppingCartRepository::findById(int id) {
    QSqlQuery q(db);
    q.prepare("SELECT * FROM ShoppingCarts WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec() || !q.next()) return std::nullopt;

    ShoppingCart cart = fromQuery(q);
    for (int bookId : loadCartItems(id)) {
        cart.addBook(bookId);
    }
    return cart;
}

std::optional<ShoppingCart> ShoppingCartRepository::findByUserId(int userId) {
    QSqlQuery q(db);
    q.prepare("SELECT * FROM ShoppingCarts WHERE user_id = ?");
    q.addBindValue(userId);

    if (!q.exec() || !q.next()) return std::nullopt;

    ShoppingCart cart = fromQuery(q);
    for (int bookId : loadCartItems(cart.getId())) {
        cart.addBook(bookId);
    }
    return cart;
}

bool ShoppingCartRepository::remove(int id) {
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    q.prepare("DELETE FROM CartItems WHERE cart_id = ?");
    q.addBindValue(id);
    if (!q.exec()) { db.rollback(); return false; }

    q.prepare("DELETE FROM ShoppingCarts WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec() || q.numRowsAffected() == 0) { db.rollback(); return false; }

    return db.commit();
}

bool ShoppingCartRepository::syncCartItems(int cartId, const QList<int>& itemIds) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM CartItems WHERE cart_id = ?");
    q.addBindValue(cartId);
    if (!q.exec()) return false;

    q.prepare("INSERT INTO CartItems (cart_id, book_id) VALUES (?, ?)");
    for (int bookId : itemIds) {
        q.addBindValue(cartId);
        q.addBindValue(bookId);
        if (!q.exec()) return false;
    }
    return true;
}

QList<int> ShoppingCartRepository::loadCartItems(int cartId) {
    QList<int> items;
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM CartItems WHERE cart_id = ?");
    q.addBindValue(cartId);

    if (q.exec()) {
        while (q.next()) items.append(q.value(0).toInt());
    }
    return items;
}
