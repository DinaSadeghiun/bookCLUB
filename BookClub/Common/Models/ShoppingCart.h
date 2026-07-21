#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include <QList>
#include <QDateTime>

class Order;

class ShoppingCart {
private:
    int id;
    int userId;
    QList<int> itemIds;
    QDateTime createdAt;

public:
    //creat new
    explicit ShoppingCart(int userId);

    // loading from DB
    ShoppingCart(int id, int userId, const QDateTime& createdAt);

    bool addBook(int bookId);
    bool removeBook(int bookId);
    void clearCart();

    //getters
    int getId() const;
    int getUserId() const;
    const QList<int>& getItemIds() const;

    //setters
    void setUserId(int newId);
    void setId(int newId);

    bool isEmpty() const;
    QDateTime getCreatedAt() const;
};

//for UI
struct CartDetails {
    QList<int> bookIds;
    int itemsCount = 0;
    double rawTotalPrice = 0.0;
    double totalDiscountAmount = 0.0;
    double finalPriceToPay = 0.0;
};

#endif
