#include "shoppingcartservice.h"
#include "DB/shoppingcartrepository.h"
#include "bookservice.h"
#include <QDebug>

ShoppingCartService::ShoppingCartService(ShoppingCartRepository* repo, BookService* bookSvc, QObject* parent)
    : QObject(parent), cartRepo(repo), bookService(bookSvc) {}


ShoppingCart ShoppingCartService::getOrCreateCart(int userId) {
    auto cartOpt = cartRepo->findByUserId(userId);
    if (cartOpt.has_value()) {
        return cartOpt.value();
    }

    ShoppingCart newCart(userId);
    cartRepo->save(newCart);

    return cartRepo->findByUserId(userId).value_or(newCart);
}


bool ShoppingCartService::addBookToCart(int userId, int bookId) {
    if (userId <= 0 || bookId <= 0) return false;

    auto bookOpt = bookService->getBookById(bookId);
    if (!bookOpt.has_value()) {
        qDebug() << "Add to Cart Failed: Book does not exist.";
        return false;
    }

    ShoppingCart cart = getOrCreateCart(userId);

    if (cart.getItemIds().contains(bookId)) {
        qDebug() << "Add to Cart Failed: Book already in cart.";
        return false;
    }

    cart.addBook(bookId);
    if (cartRepo->save(cart)) {
        emit bookAddedToCart(userId, bookId);
        return true;
    }
    return false;
}

bool ShoppingCartService::removeBookFromCart(int userId, int bookId) {
    if (userId <= 0 || bookId <= 0) return false;

    auto cartOpt = cartRepo->findByUserId(userId);
    if (!cartOpt.has_value()) return false;

    ShoppingCart cart = cartOpt.value();
    if (!cart.getItemIds().contains(bookId)) {
        return false;
    }

    cart.removeBook(bookId);
    if (cartRepo->save(cart)) {
        emit bookRemovedFromCart(userId, bookId);
        return true;
    }
    return false;
}

bool ShoppingCartService::clearCart(int userId) {
    if (userId <= 0) return false;

    auto cartOpt = cartRepo->findByUserId(userId);
    if (!cartOpt.has_value()) return true;

    ShoppingCart cart = cartOpt.value();
    cart.clearCart();
    if (cartRepo->save(cart)) {
        emit cartCleared(userId);
        return true;
    }
    return false;
}

CartDetails ShoppingCartService::getCartDetails(int userId) {
    CartDetails details;
    if (userId <= 0) return details;

    auto cartOpt = cartRepo->findByUserId(userId);
    if (!cartOpt.has_value()) {
        return details;
    }

    ShoppingCart cart = cartOpt.value();
    details.bookIds = cart.getItemIds();
    details.itemsCount = details.bookIds.size();

    for (int bookId : std::as_const(details.bookIds)) {
        auto bookOpt = bookService->getBookById(bookId);
        if (!bookOpt.has_value()) continue;

        Book book = bookOpt.value();
        double originalPrice = book.getPrice();

        double finalPrice = bookService->getBookFinalPrice(bookId).value_or(originalPrice);

        details.rawTotalPrice += originalPrice;
        details.finalPriceToPay += finalPrice;
    }

    details.totalDiscountAmount = details.rawTotalPrice - details.finalPriceToPay;

    return details;
}
