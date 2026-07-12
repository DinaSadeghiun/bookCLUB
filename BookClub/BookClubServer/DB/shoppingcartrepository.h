#ifndef SHOPPINGCARTREPOSITORY_H
#define SHOPPINGCARTREPOSITORY_H

#include "ShoppingCart.h"
#include <QString>
#include <QList>
#include <optional>
#include <QSqlDatabase>
#include <QSqlQuery>

class ShoppingCartRepository {
public:
    explicit ShoppingCartRepository();

    bool save(ShoppingCart& cart);
    std::optional<ShoppingCart> findById(int id);
    std::optional<ShoppingCart> findByUserId(int userId);
    bool remove(int id);

private:
    QSqlDatabase db;

    ShoppingCart fromQuery(QSqlQuery& q) const;
    bool syncCartItems(int cartId, const QList<int>& itemIds);
    QList<int> loadCartItems(int cartId);
};

#endif
