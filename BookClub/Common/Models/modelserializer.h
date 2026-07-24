#ifndef MODELSERIALIZER_H
#define MODELSERIALIZER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QDateTime>

// Includes
#include "Admin.h"
#include "Book.h"
#include "user.h"
#include "Publisher.h"
#include "Comment.h"
#include "Discount.h"
#include "Notification.h"
#include "shoppingcart.h"
#include "Order.h"

class ModelSerializer {
public:
    // Admin
    static QJsonObject serializeAdmin(const Admin& a);
    static QJsonArray serializeAdminList(const QList<Admin>& admins);
    static Admin deserializeAdmin(const QJsonObject& obj);

    // Book
    static QJsonObject serializeBook(const Book& b);
    static QJsonObject serializeBook(const Book& b, const std::optional<Discount>& discount);
    static QJsonArray serializeBookList(const QList<Book>& books);
    static Book deserializeBook(const QJsonObject& obj);

    // User
    static QJsonObject serializeUser(const User& u);
    static QJsonArray serializeUserList(const QList<User>& users);
    static User deserializeUser(const QJsonObject& obj);

    // Publisher
    static QJsonObject serializePublisher(const Publisher& p);
    static QJsonArray serializePublisherList(const QList<Publisher>& publishers);
    static Publisher deserializePublisher(const QJsonObject& obj);

    // Comment
    static QJsonObject serializeComment(const Comment& c);
    static QJsonArray serializeCommentList(const QList<Comment>& comments);
    static Comment deserializeComment(const QJsonObject& obj);

    // Discount
    static QJsonObject serializeDiscount(const Discount& d);
    static QJsonArray serializeDiscountList(const QList<Discount>& discounts);
    static Discount deserializeDiscount(const QJsonObject& obj);

    // Notification
    static QJsonObject serializeNotification(const Notification& n);
    static QJsonArray serializeNotificationList(const QList<Notification>& notifications);
    static Notification deserializeNotification(const QJsonObject& obj);

    //  CartDetails
    static QJsonObject serializeCartDetails(const CartDetails& details);

    // Order
    static QJsonObject serializeOrder(const Order& order);
    static QJsonArray serializeOrderList(const QList<Order>& orders);

    static QJsonArray serializeIntList(const QList<int>& list);
    static QJsonArray serializeStringList(const QList<QString>& list);
};

#endif
