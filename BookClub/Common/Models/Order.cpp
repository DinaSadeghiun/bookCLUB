#include "order.h"
#include <QDebug>

Order::Order(int userId, double rawPrice, double discountAmount,
             double finalPrice, const QList<int>& bookIds)
    : id(-1)
    , userId(userId)
    , orderDate(QDateTime::currentDateTime())
    , rawPrice(rawPrice >= 0 ? rawPrice : 0.0)
    , discountAmount(discountAmount >= 0 ? discountAmount : 0.0)
    , finalPrice(finalPrice >= 0 ? finalPrice : 0.0)
    , bookIds(bookIds)
{
    Q_ASSERT(!bookIds.isEmpty());
}

Order::Order(int id, int userId, QDateTime orderDate, double rawPrice,
             double discountAmount, double finalPrice, const QList<int>& bookIds)
    : id(id) ,
    userId(userId),
    orderDate(orderDate),
    rawPrice(rawPrice)
    , discountAmount(discountAmount),
    finalPrice(finalPrice),
    bookIds(bookIds)
{}

//getter
int Order::getId() const { return id; }
int Order::getUserId() const { return userId; }
QDateTime Order::getOrderDate() const { return orderDate; }
double Order::getRawPrice() const { return rawPrice; }
double Order::getDiscountAmount() const { return discountAmount; }
double Order::getFinalPrice() const { return finalPrice;}
const QList<int>& Order::getBookIds() const { return bookIds; }
int Order::getBooksCount() const { return bookIds.size(); }


//setter
void Order::setId(int newId) {
    if (id == -1) {
        id = newId;
    }
}
void Order::setBookIds(const QList<int>& newBookIds) {
    bookIds = newBookIds;
}