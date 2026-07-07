#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QDateTime>
#include <QList>

class Order {
    int id;
    int userId;
    QDateTime orderDate;
    double rawPrice;
    double discountAmount;
    double finalPrice;
    QList<int> bookIds;

public:
    // New order (not saved yet)
    Order(int userId, double rawPrice, double discountAmount,
          double finalPrice, const QList<int>& bookIds);

    // Load from DB
    Order(int id, int userId, QDateTime orderDate, double rawPrice,
          double discountAmount, double finalPrice, const QList<int>& bookIds);

    void setId(int newId);

    int getId() const;
    int getUserId() const;
    QDateTime getOrderDate() const;
    double getRawPrice() const;
    double getDiscountAmount() const;
    double getFinalPrice() const;
    const QList<int>& getBookIds() const;
    int getBooksCount() const;
};

#endif