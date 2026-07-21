#ifndef SHOPPINGCARTSERVICE_H
#define SHOPPINGCARTSERVICE_H

#include <QObject>
#include <QList>
#include <optional>
#include "ShoppingCart.h"

class ShoppingCartRepository;
class BookService;

class ShoppingCartService : public QObject {
    Q_OBJECT
private:
    ShoppingCartRepository* cartRepo;
    BookService* bookService;

    ShoppingCart getOrCreateCart(int userId);

public:
    explicit ShoppingCartService(ShoppingCartRepository* repo, BookService* bookSvc, QObject* parent = nullptr);
    ~ShoppingCartService() = default;

    bool addBookToCart(int userId, int bookId);
    bool removeBookFromCart(int userId, int bookId);
    bool clearCart(int userId);

    CartDetails getCartDetails(int userId);

signals:
    void bookAddedToCart(int userId, int bookId);
    void bookRemovedFromCart(int userId, int bookId);
    void cartCleared(int userId);

};

#endif
