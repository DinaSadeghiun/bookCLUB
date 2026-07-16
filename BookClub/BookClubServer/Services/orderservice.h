#ifndef ORDERSERVICE_H
#define ORDERSERVICE_H

#include <QList>
#include <optional>
#include "Order.h"

class OrderRepository;
class ShoppingCartService;
class UserService;
class PersonalLibraryRepository;

class OrderService {
private:
    OrderRepository* orderRepo;
    ShoppingCartService* cartService;
    UserService* userService;
    PersonalLibraryRepository* personalLibRepo;

public:
    explicit OrderService(OrderRepository* oRepo,
                          ShoppingCartService* cartSvc,
                          UserService* userSvc,
                          PersonalLibraryRepository* libRepo);
    ~OrderService() = default;

    bool checkout(int userId);
    QList<Order> getOrderHistory(int userId) const;
};

#endif
