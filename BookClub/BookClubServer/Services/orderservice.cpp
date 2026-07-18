#include "orderservice.h"
#include "DB/orderrepository.h"
#include "DB/personallibraryrepository.h"
#include "shoppingcartservice.h"
#include "Services/userservice.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <utility>

OrderService::OrderService(OrderRepository* oRepo,
                           ShoppingCartService* cartSvc,
                           UserService* userSvc,
                           PersonalLibraryRepository* libRepo,
                           QObject* parent)
    : QObject(parent), orderRepo(oRepo), cartService(cartSvc),
    userService(userSvc), personalLibRepo(libRepo)
{
    Q_ASSERT(orderRepo != nullptr);
    Q_ASSERT(cartService != nullptr);
    Q_ASSERT(userService != nullptr);
    Q_ASSERT(personalLibRepo != nullptr);
}

bool OrderService::checkout(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !orderRepo || !cartService || !userService || !personalLibRepo) {
        qDebug() << "Checkout Failed: Invalid user ID or uninitialized dependencies.";
        return false;
    }


    // 1. Fetch cart details and calculate prices
    CartDetails details = cartService->getCartDetails(userId);
    if (details.bookIds.isEmpty()) {
        qDebug() << "Checkout Failed: Shopping cart is empty for user:" << userId;
        return false;
    }

    // 2. Check wallet balance before transaction
    double userWallet = userService->getWalletBalance(userId);
    if (userWallet < details.finalPriceToPay) {
        qDebug() << "Checkout Failed: Insufficient wallet balance. Wallet:"
                 << userWallet << ", Required:" << details.finalPriceToPay;
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database("bookclub_db");

    // Start a nested transaction using SQLite SAVEPOINT
    QSqlQuery query(db);
    if (!query.exec("SAVEPOINT checkout_sp")) {
        qDebug() << "Checkout Failed: Could not create savepoint:" << query.lastError().text();
        return false;
    }

    // 3. Withdraw price from user wallet (runs inner transaction inside UserRepository)
    if (!userService->withdrawBalance(userId, details.finalPriceToPay)) {
        qDebug() << "Checkout Failed: Wallet withdrawal failed.";
        query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
        return false;
    }

    // 4. Save new order (runs inner transaction inside OrderRepository)
    Order newOrder(userId, details.rawTotalPrice, details.totalDiscountAmount,
                   details.finalPriceToPay, details.bookIds);
    if (!orderRepo->save(newOrder)) {
        qDebug() << "Checkout Failed: Could not save order in repository.";
        query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
        return false;
    }

    // 5. Transfer purchased books to user's personal library (runs inner transaction inside PersonalLibraryRepository)
    for (int bookId : std::as_const(details.bookIds)) {
        if (!personalLibRepo->addPurchasedBook(userId, bookId)) {
            qDebug() << "Checkout Failed: Could not add book" << bookId << "to Personal Library.";
            query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
            return false;
        }
    }

    // 6. Release (Commit) the savepoint to persist all changes
    if (!query.exec("RELEASE SAVEPOINT checkout_sp")) {
        qDebug() << "Checkout Failed: Release savepoint failed:" << query.lastError().text();
        query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
        return false;
    }

    emit checkoutCompleted(userId, newOrder.getId(), details.finalPriceToPay);

    // 7. Clear the cart (Non-critical operation, performed post-release)
    if (!cartService->clearCart(userId)) {
        qDebug() << "Warning: Could not clear shopping cart after successful purchase.";
    }

    qDebug() << "Checkout Completed Successfully for user:" << userId;
    return true;
}

QList<Order> OrderService::getOrderHistory(int userId) const {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !orderRepo) return {};
    return orderRepo->findByUserId(userId);
}