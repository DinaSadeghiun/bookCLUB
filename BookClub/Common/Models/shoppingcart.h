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
    explicit ShoppingCart(int userId);

    bool addBook(int bookId);
    bool removeBook(int bookId);
    void clearCart();

    // TODO (DB phase): inject BookRepository to calculate prices
    // double getTotalRawPrice(BookRepository* repo) const;
    // double getTotalDiscountAmount(BookRepository* repo) const;
    // double getFinalPayableAmount(BookRepository* repo) const;

    // TODO (DB phase): Order* checkout(BookRepository* repo);

    //getters
    int getId() const;
    int getUserId() const;
    const QList<int>& getItemIds() const;

    //setters
    void setUserId(int newId);

    bool isEmpty() const;
    QDateTime getCreatedAt() const;
};

#endif
