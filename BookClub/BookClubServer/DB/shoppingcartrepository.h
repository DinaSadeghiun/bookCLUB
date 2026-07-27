#ifndef SHOPPINGCARTREPOSITORY_H
#define SHOPPINGCARTREPOSITORY_H

#include "ShoppingCart.h"
#include <QString>
#include <QList>
#include <optional>
#include <QSqlDatabase>
#include <QSqlQuery>
class DatabaseManager;

class ShoppingCartRepository {
public:
    explicit ShoppingCartRepository(DatabaseManager* manager);

    bool save(ShoppingCart& cart);
    bool saveInternal(ShoppingCart& cart);
    std::optional<ShoppingCart> findById(int id);
    std::optional<ShoppingCart> findByUserId(int userId);
    bool remove(int id);

private:
    DatabaseManager* dbManager;

    ShoppingCart fromQuery(QSqlQuery& q) const;
    bool syncCartItems(QSqlQuery& q, int cartId, const QList<int>& itemIds);
    QList<int> loadCartItems(int cartId);
};

#endif
