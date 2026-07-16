#ifndef SHOPPINGCARTSERVICE_H
#define SHOPPINGCARTSERVICE_H

#include <QList>
#include <optional>
#include "ShoppingCart.h"

class ShoppingCartRepository;
class BookService;

//for UI
struct CartDetails {
    QList<int> bookIds;
    int itemsCount = 0;
    double rawTotalPrice = 0.0;
    double totalDiscountAmount = 0.0;
    double finalPriceToPay = 0.0;
};

class ShoppingCartService {
private:
    ShoppingCartRepository* cartRepo;
    BookService* bookService;

    ShoppingCart getOrCreateCart(int userId);

public:
    explicit ShoppingCartService(ShoppingCartRepository* repo, BookService* bookSvc);
    ~ShoppingCartService() = default;

    bool addBookToCart(int userId, int bookId);
    bool removeBookFromCart(int userId, int bookId);
    bool clearCart(int userId);

    CartDetails getCartDetails(int userId);
};

#endif
