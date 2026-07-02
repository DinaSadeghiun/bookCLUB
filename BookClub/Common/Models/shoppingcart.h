#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include <QList>

class Book;

class ShoppingCart {
public:
    explicit ShoppingCart(int userId = 0);

    void addBook(int bookId);
    bool removeBook(int bookId);
    void clearCart();

    double getTotalFinalPrice() const;

    int getUserId() const { return userId; }
    const QList<int>& getItemIds() const { return itemIds; }
    bool isEmpty() const { return itemIds.isEmpty(); }

private:
    int userId;
    QList<int> itemIds;
};

#endif
