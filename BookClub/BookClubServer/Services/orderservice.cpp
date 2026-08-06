#include "orderservice.h"
#include "DB/orderrepository.h"
#include "DB/personallibraryrepository.h"
#include "shoppingcartservice.h"
#include "Services/userservice.h"
#include "DB/bookrepository.h"
#include "Services/publisherservice.h"
#include "Services/notificationservice.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <utility>

OrderService::OrderService(OrderRepository* oRepo,
                           ShoppingCartService* cartSvc,
                           UserService* userSvc,
                           PersonalLibraryRepository* libRepo,
                           BookRepository* bRepo,
                           PublisherService* pubSvc,
                           NotificationService* notifSvc,
                           QObject* parent)
    : QObject(parent),
    orderRepo(oRepo),
    cartService(cartSvc),
    userService(userSvc),
    personalLibRepo(libRepo),
    bookRepo(bRepo),
    publisherSvc(pubSvc),
    notificationSvc(notifSvc)
{
    Q_ASSERT(orderRepo != nullptr);
    Q_ASSERT(cartService != nullptr);
    Q_ASSERT(userService != nullptr);
    Q_ASSERT(personalLibRepo != nullptr);
    Q_ASSERT(bookRepo != nullptr);
    Q_ASSERT(publisherSvc != nullptr);
    Q_ASSERT(notificationSvc != nullptr);
}

bool OrderService::checkout(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !orderRepo || !cartService || !userService || !personalLibRepo || !bookRepo) {
        qDebug() << "Checkout Failed: Invalid user ID or uninitialized dependencies.";
        return false;
    }

    // Fetch cart details and calculate prices
    CartDetails details = cartService->getCartDetails(userId);
    if (details.bookIds.isEmpty()) {
        qDebug() << "Checkout Failed: Shopping cart is empty for user:" << userId;
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database("bookclubFinal.db");

    // Start a nested transaction using SQLite SAVEPOINT
    QSqlQuery query(db);
    if (!query.exec("SAVEPOINT checkout_sp")) {
        qDebug() << "Checkout Failed: Could not create savepoint:" << query.lastError().text();
        return false;
    }

    // Save new order (runs inner transaction inside OrderRepository)
    Order newOrder(userId, details.rawTotalPrice, details.totalDiscountAmount,
                   details.finalPriceToPay, details.bookIds);
    if (!orderRepo->save(newOrder)) {
        qDebug() << "Checkout Failed: Could not save order in repository.";
        query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
        return false;
    }

    // Transfer purchased books to user's personal library (runs inner transaction inside PersonalLibraryRepository)
    for (int bookId : std::as_const(details.bookIds)) {
        auto bookOpt = bookRepo->findById(bookId);
        if (!bookOpt.has_value()) {
            query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
            return false;
        }

        if (!publisherSvc->addRevenue(bookOpt->getPublisherId(), bookOpt->getFinalPrice())) {
            query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
            return false;
        }

        if (!personalLibRepo->addPurchasedBook(userId, bookId)) {
            query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
            return false;
        }

        if (!bookRepo->incrementSalesCount(bookId)) {
            qDebug() << "Checkout Failed: Could not increment sales_count for book:" << bookId;
            query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
            return false;
        }

        notificationSvc->sendNotification(
            NotificationType::NewSaleForBook,
            bookOpt->getPublisherId(),
            bookId,
            "💰 New sale for '" + bookOpt->getTitle() + "'!"
            );

    }

    // Release (Commit) the savepoint to persist all changes
    if (!query.exec("RELEASE SAVEPOINT checkout_sp")) {
        qDebug() << "Checkout Failed: Release savepoint failed:" << query.lastError().text();
        query.exec("ROLLBACK TO SAVEPOINT checkout_sp");
        return false;
    }

    emit checkoutCompleted(userId, newOrder.getId(), details.finalPriceToPay);

    // Clear the cart (Non-critical operation, performed post-release)
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