#include "shoppingcart.h"
//#include "databasemanager.h"

ShoppingCart::ShoppingCart(int userId)
    : userId(userId) {}

void ShoppingCart::addBook(int bookId) {
    if (!itemIds.contains(bookId)) {
        itemIds.append(bookId);
    }
}

bool ShoppingCart::removeBook(int bookId) {
    return itemIds.removeOne(bookId);
}

void ShoppingCart::clearCart() {
    itemIds.clear();
}

double ShoppingCart::getTotalFinalPrice() const {
    double total = 0.0;

 //not connected to databace yet, so price of all books is 50 for now
    for (int id : itemIds) {
        Q_UNUSED(id);
        total += 50.0;
    }

    return total;
}