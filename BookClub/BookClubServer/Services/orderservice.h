#ifndef ORDERSERVICE_H
#define ORDERSERVICE_H

#include <QObject>
#include <QList>
#include <optional>
#include "Order.h"

class OrderRepository;
class ShoppingCartService;
class UserService;
class PersonalLibraryRepository;

class OrderService : public QObject {
    Q_OBJECT
private:
    OrderRepository* orderRepo;
    ShoppingCartService* cartService;
    UserService* userService;
    PersonalLibraryRepository* personalLibRepo;

public:
    explicit OrderService(OrderRepository* oRepo,
                          ShoppingCartService* cartSvc,
                          UserService* userSvc,
                          PersonalLibraryRepository* libRepo,
                          QObject* parent = nullptr);
    ~OrderService() = default;

    bool checkout(int userId);
    QList<Order> getOrderHistory(int userId) const;

signals:
    void checkoutCompleted(int userId, int orderId, double amountPaid);

};

#endif
