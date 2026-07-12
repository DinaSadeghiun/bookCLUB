#include "ShoppingCart.h"

//creat new
ShoppingCart::ShoppingCart(int userId)
    : id(-1), userId(userId), createdAt(QDateTime::currentDateTime()) {}

//loading from DB
ShoppingCart::ShoppingCart(int id, int userId, const QDateTime& createdAt)
    : id(id), userId(userId), createdAt(createdAt) {}

bool ShoppingCart::addBook(int bookId) {
    if (itemIds.contains(bookId)) return false;
    itemIds.append(bookId);
    return true;
}

bool ShoppingCart::removeBook(int bookId) {
    return itemIds.removeOne(bookId);
}

void ShoppingCart::clearCart() {
    itemIds.clear();
}

//getters
int ShoppingCart::getId() const { return id; }
int ShoppingCart::getUserId() const { return userId; }
const QList<int>& ShoppingCart::getItemIds() const { return itemIds; }

//setters
void ShoppingCart::setUserId(int newId) { userId = newId; }
void ShoppingCart::setId(int newId) { id = newId; }


bool ShoppingCart::isEmpty() const { return itemIds.isEmpty(); }
QDateTime ShoppingCart::getCreatedAt() const { return createdAt; }
