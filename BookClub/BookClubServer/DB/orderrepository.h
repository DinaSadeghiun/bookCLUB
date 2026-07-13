#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H

#include <QList>
#include <optional>
#include <QSqlDatabase>
#include "Order.h"

class DatabaseManager;

class OrderRepository {
private:
    DatabaseManager* dbManager;

    Order fromQuery(QSqlQuery& q);
    void loadOrderBooks(Order& order);

public:
    explicit OrderRepository(DatabaseManager* manager);

    bool save(Order& order);
    std::optional<Order> findById(int id);
    QList<Order> findByUserId(int userId);
    QList<Order> findAll();
    bool remove(int id);

};

#endif
