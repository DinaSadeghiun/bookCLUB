#include "shoppingcartservice.h"
#include "DB/shoppingcartrepository.h"
#include "bookservice.h"
#include <QDebug>

ShoppingCartService::ShoppingCartService(ShoppingCartRepository* repo, BookService* bookSvc, QObject* parent)
    : QObject(parent), cartRepo(repo), bookService(bookSvc)
{
    Q_ASSERT(cartRepo != nullptr);
    Q_ASSERT(bookService != nullptr);
}

ShoppingCart ShoppingCartService::getOrCreateCart(int userId) {
    qDebug() << "=== getOrCreateCart called for userId:" << userId;
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !cartRepo) {
        return ShoppingCart(userId);
    }
    auto cartOpt = cartRepo->findByUserId(userId);
    if (cartOpt.has_value()) {
        qDebug() << "Cart found for userId:" << userId << "cartId:" << cartOpt->getId();
        return cartOpt.value();
    }

    qDebug() << "No cart found for userId:" << userId << ", creating new cart";
    ShoppingCart newCart(userId);
    cartRepo->save(newCart);

    auto saved = cartRepo->findByUserId(userId);
    if (saved.has_value()) {
        qDebug() << "New cart created with id:" << saved->getId();
        return saved.value();
    }
    qDebug() << "Failed to create new cart, returning empty";
    return newCart;
}

bool ShoppingCartService::addBookToCart(int userId, int bookId) {
    qDebug() << "=== addBookToCart called - userId:" << userId << "bookId:" << bookId;
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !cartRepo || !bookService) {
        qDebug() << "ERROR: Invalid parameters or null repo/service";
        return false;
    }

    auto bookOpt = bookService->getBookById(bookId);
    if (!bookOpt.has_value()) {
        qDebug() << "ERROR: Book does not exist. bookId:" << bookId;
        return false;
    }
    qDebug() << "Book found:" << bookOpt->getTitle() << "ID:" << bookId;

    ShoppingCart cart = getOrCreateCart(userId);
    qDebug() << "Cart ID:" << cart.getId() << "Current items:" << cart.getItemIds();

    if (cart.getItemIds().contains(bookId)) {
        qDebug() << "ERROR: Book already in cart. bookId:" << bookId;
        return false;
    }

    cart.addBook(bookId);
    if (cartRepo->save(cart)) {
        qDebug() << "SUCCESS: Book added to cart. cartId:" << cart.getId() << "bookId:" << bookId;
        emit bookAddedToCart(userId, bookId);
        return true;
    } else {
        qDebug() << "ERROR: Failed to save cart. cartId:" << cart.getId();
        return false;
    }
}

bool ShoppingCartService::removeBookFromCart(int userId, int bookId) {
    qDebug() << "=== removeBookFromCart - userId:" << userId << "bookId:" << bookId;
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !cartRepo) {
        qDebug() << "ERROR: Invalid parameters";
        return false;
    }

    auto cartOpt = cartRepo->findByUserId(userId);
    if (!cartOpt.has_value()) {
        qDebug() << "ERROR: Cart not found for userId:" << userId;
        return false;
    }

    ShoppingCart cart = cartOpt.value();
    qDebug() << "Cart found, ID:" << cart.getId() << "items:" << cart.getItemIds();
    if (!cart.getItemIds().contains(bookId)) {
        qDebug() << "ERROR: Book not in cart. bookId:" << bookId;
        return false;
    }

    cart.removeBook(bookId);
    if (cartRepo->save(cart)) {
        qDebug() << "SUCCESS: Book removed from cart. cartId:" << cart.getId() << "bookId:" << bookId;
        emit bookRemovedFromCart(userId, bookId);
        return true;
    } else {
        qDebug() << "ERROR: Failed to save cart after removal. cartId:" << cart.getId();
        return false;
    }
}

bool ShoppingCartService::clearCart(int userId) {
    qDebug() << "=== clearCart called for userId:" << userId;
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !cartRepo) {
        qDebug() << "ERROR: Invalid userId or null repo";
        return false;
    }
    auto cartOpt = cartRepo->findByUserId(userId);
    if (!cartOpt.has_value()) {
        qDebug() << "No cart found for userId:" << userId << ", nothing to clear";
        return true;
    }

    ShoppingCart cart = cartOpt.value();
    qDebug() << "Cart found, ID:" << cart.getId() << "items count:" << cart.getItemIds().size();
    cart.clearCart();
    if (cartRepo->saveInternal(cart)) {
        qDebug() << "SUCCESS: Cart cleared. cartId:" << cart.getId();
        emit cartCleared(userId);
        return true;
    } else {
        qDebug() << "ERROR: Failed to clear cart. cartId:" << cart.getId();
        return false;
    }
}

CartDetails ShoppingCartService::getCartDetails(int userId) {
    CartDetails details;
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !cartRepo || !bookService) {
        return details;
    }

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
        details.books.append(book);

        double originalPrice = book.getPrice();
        double finalPrice = bookService->getBookFinalPrice(bookId).value_or(originalPrice);

        details.rawTotalPrice += originalPrice;
        details.finalPriceToPay += finalPrice;
    }

    details.totalDiscountAmount = details.rawTotalPrice - details.finalPriceToPay;

    return details;
}