#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H

#include <QList>
#include <optional>
#include <QSqlDatabase>
#include "Order.h"

class QSqlQuery; // Forward declaration

class OrderRepository {
private:
    QSqlDatabase db;

    Order fromQuery(QSqlQuery& q);
    void loadOrderBooks(Order& order);

public:
    explicit OrderRepository();

    bool save(Order& order);
    std::optional<Order> findById(int id);
    QList<Order> findByUserId(int userId);
    QList<Order> findAll();
    bool remove(int id);

};

#endif
