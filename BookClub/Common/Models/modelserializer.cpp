#include "modelserializer.h"
#include <QFile>


// Book
QJsonObject ModelSerializer::serializeBook(const Book& b) {
    QJsonObject o;
    o["id"] = b.getId();
    o["publisherId"] = b.getPublisherId();
    o["discountId"] = b.getDiscountId();
    o["title"] = b.getTitle();
    o["author"] = b.getAuthor();
    o["genre"] = genreToString(b.getGenre());
    o["description"] = b.getDescription();
    o["coverImagePath"] = b.getCoverImagePath();
    o["pdfFilePath"] = b.getPdfFilePath();
    o["price"] = b.getPrice();
    o["totalRating"] = b.getTotalRating();
    o["ratingCount"] = b.getRatingCount();
    o["salesCount"] = b.getSalesCount();
    o["isAvailable"] = b.getIsAvailable();
    o["discount"] = b.getDiscountAmount();

    QString coverData = "";
    if (!b.getCoverImagePath().isEmpty()) {
        QFile coverFile(b.getCoverImagePath());
        if (coverFile.open(QIODevice::ReadOnly)) {
            coverData = "data:image/jpeg;base64," + QString::fromLatin1(coverFile.readAll().toBase64());
        }
    }
    o["coverImageData"] = coverData;

    return o;
}

QJsonObject ModelSerializer::serializeBook(const Book& b, const std::optional<Discount>& discount) {
    QJsonObject o;
    o["id"] = b.getId();
    o["publisherId"] = b.getPublisherId();
    o["discountId"] = b.getDiscountId();
    o["title"] = b.getTitle();
    o["author"] = b.getAuthor();
    o["genre"] = genreToString(b.getGenre());
    o["description"] = b.getDescription();
    o["coverImagePath"] = b.getCoverImagePath();
    o["pdfFilePath"] = b.getPdfFilePath();
    o["price"] = b.getPrice();
    o["totalRating"] = b.getTotalRating();
    o["ratingCount"] = b.getRatingCount();
    o["salesCount"] = b.getSalesCount();
    o["isAvailable"] = b.getIsAvailable();

    if (discount.has_value()) {
        o["discountValue"] = discount->getValue();
        o["discountType"] = static_cast<int>(discount->getType());
    } else {
        o["discountValue"] = 0.0;
        o["discountType"] = 0;
    }
    QString coverData = "";
    if (!b.getCoverImagePath().isEmpty()) {
        QFile coverFile(b.getCoverImagePath());
        if (coverFile.open(QIODevice::ReadOnly)) {
            coverData = "data:image/jpeg;base64," + QString::fromLatin1(coverFile.readAll().toBase64());
        }
    }
    o["coverImageData"] = coverData;    return o;
}

Book ModelSerializer::deserializeBook(const QJsonObject& o) {
    return Book(
        o["id"].toInt(),
        o["publisherId"].toInt(),
        o["discountId"].toInt(),
        o["title"].toString(),
        o["author"].toString(),
        stringToGenre(o["genre"].toString()),
        o["description"].toString(),
        o["coverImagePath"].toString(),
        o["pdfFilePath"].toString(),
        o["price"].toDouble(),
        o["totalRating"].toDouble(),
        o["ratingCount"].toInt(),
        o["salesCount"].toInt(),
        o["isAvailable"].toBool()
        );
}

QJsonArray ModelSerializer::serializeBookList(const QList<Book>& books) {
    QJsonArray arr;
    for (const auto& b : books) arr.append(serializeBook(b));
    return arr;
}

// Admin
QJsonObject ModelSerializer::serializeAdmin(const Admin& a) {
    QJsonObject o;
    o["id"] = a.getId();
    o["username"] = a.getUsername();
    o["createdAt"] = a.getCreatedAt().toSecsSinceEpoch();
    o["isActive"] = a.getIsActive();
    o["role"] = "admin";
    return o;
}

Admin ModelSerializer::deserializeAdmin(const QJsonObject& o) {
    return Admin(
        o["id"].toInt(),
        o["username"].toString(),
        "",
        QDateTime::fromSecsSinceEpoch(o["createdAt"].toVariant().toLongLong()),
        o["isActive"].toBool(), ""
        );
}


QJsonArray ModelSerializer::serializeAdminList(const QList<Admin>& admins) {
    QJsonArray arr;
    for (const auto& a : admins) arr.append(serializeAdmin(a));
    return arr;
}


//User
QJsonObject ModelSerializer::serializeUser(const User& u) {
    QJsonObject o;
    o["id"] = u.getId();
    o["username"] = u.getUsername();
    o["createdAt"] = u.getCreatedAt().toSecsSinceEpoch();
    o["isActive"] = u.getIsActive();
    o["role"] = "user";

    QJsonArray favs;
    for (Genre g : u.getFavoriteGenres()) {
        favs.append(genreToString(g));
    }
    o["favoriteGenres"] = favs;
    return o;
}

User ModelSerializer::deserializeUser(const QJsonObject& o) {
    QList<Genre> favs;
    QJsonArray favsArr = o["favoriteGenres"].toArray();
    for (int i = 0; i < favsArr.size(); ++i) {
        favs.append(stringToGenre(favsArr.at(i).toString()));
    }

    User u(
        o["id"].toInt(),
        o["username"].toString(),
        "",
        QDateTime::fromSecsSinceEpoch(o["createdAt"].toVariant().toLongLong()),
        o["isActive"].toBool(), "");

    for (Genre g : favs) {
        u.addFavoriteGenre(g);
    }

    return u;
}


QJsonArray ModelSerializer::serializeUserList(const QList<User>& users) {
    QJsonArray arr;
    for (const auto& u : users) arr.append(serializeUser(u));
    return arr;
}

//Publisher
QJsonObject ModelSerializer::serializePublisher(const Publisher& p) {
    QJsonObject o;
    o["id"] = p.getId();
    o["username"] = p.getUsername();
    o["createdAt"] = p.getCreatedAt().toSecsSinceEpoch();
    o["isActive"] = p.getIsActive();
    o["companyName"] = p.getCompanyName();
    o["revenue"] = p.getRevenue();
    o["role"] = "publisher";
    return o;
}

Publisher ModelSerializer::deserializePublisher(const QJsonObject& o) {
    return Publisher(
        o["id"].toInt(),
        o["username"].toString(),
        "",
        QDateTime::fromSecsSinceEpoch(o["createdAt"].toVariant().toLongLong()),
        o["isActive"].toBool(), "",
        o["companyName"].toString(),
        o["revenue"].toDouble()
        );
}


QJsonArray ModelSerializer::serializePublisherList(const QList<Publisher>& publishers) {
    QJsonArray arr;
    for (const auto& p : publishers) arr.append(serializePublisher(p));
    return arr;
}

// Comment
QJsonObject ModelSerializer::serializeComment(const Comment& c) {
    QJsonObject o;
    o["id"] = c.getId();
    o["userId"] = c.getUserId();
    o["bookId"] = c.getBookId();
    o["text"] = c.getText();
    o["rating"] = c.getRating();
    o["date"] = c.getDate().toSecsSinceEpoch();
    return o;
}

Comment ModelSerializer::deserializeComment(const QJsonObject& o) {
    return Comment(
        o["id"].toInt(),
        o["userId"].toInt(),
        o["bookId"].toInt(),
        o["text"].toString(),
        o["rating"].toInt(),
        QDateTime::fromSecsSinceEpoch(o["date"].toVariant().toLongLong())
        );
}

QJsonArray ModelSerializer::serializeCommentList(const QList<Comment>& comments) {
    QJsonArray arr;
    for (const auto& c : comments) arr.append(serializeComment(c));
    return arr;
}

// Discount
QJsonObject ModelSerializer::serializeDiscount(const Discount& d) {
    QJsonObject o;
    o["id"] = d.getId();
    o["value"] = d.getValue();
    o["type"] = (d.getType() == Discount::Percentage ? "Percentage" : "FixedAmount");
    o["startDate"] = d.getStartDate().toSecsSinceEpoch();
    o["endDate"] = d.getEndDate().toSecsSinceEpoch();
    o["isActive"] = d.getIsActive();
    return o;
}

Discount ModelSerializer::deserializeDiscount(const QJsonObject& o) {
    Discount::DiscountType type = (o["type"].toString() == "FixedAmount" ?
                                       Discount::FixedAmount : Discount::Percentage);

    return Discount(
        o["id"].toInt(),
        o["value"].toDouble(),
        type,
        QDateTime::fromSecsSinceEpoch(o["startDate"].toVariant().toLongLong()),
        QDateTime::fromSecsSinceEpoch(o["endDate"].toVariant().toLongLong()),
        o["isActive"].toBool()
        );
}

//personal library
QJsonObject ModelSerializer::serializeCartDetails(const CartDetails& details) {
    QJsonObject json;
    json["itemsCount"] = details.itemsCount;
    json["rawTotalPrice"] = details.rawTotalPrice;
    json["totalDiscountAmount"] = details.totalDiscountAmount;
    json["finalPriceToPay"] = details.finalPriceToPay;
    json["bookIds"] = serializeIntList(details.bookIds);
    return json;
}

// Order
QJsonObject ModelSerializer::serializeOrder(const Order& order) {
    QJsonObject json;
    json["orderId"] = order.getId();
    json["userId"] = order.getUserId();
    json["rawTotalPrice"] = order.getRawPrice();
    json["totalDiscountAmount"] = order.getDiscountAmount();
    json["finalPriceToPay"] = order.getFinalPrice();
    json["orderDate"] = order.getOrderDate().toString(Qt::ISODate);
    json["bookIds"] = serializeIntList(order.getBookIds());
    return json;
}


QJsonArray ModelSerializer::serializeOrderList(const QList<Order>& orders) {
    QJsonArray arr;
    for (const Order& o : orders) {
        arr.append(serializeOrder(o));
    }
    return arr;
}

QJsonArray ModelSerializer::serializeIntList(const QList<int>& list) {
    QJsonArray arr;
    for (int val : list) {
        arr.append(val);
    }
    return arr;
}

QJsonArray ModelSerializer::serializeStringList(const QList<QString>& list) {
    QJsonArray arr;
    for (const QString& val : list) {
        arr.append(val);
    }
    return arr;
}

// Notification
QJsonObject ModelSerializer::serializeNotification(const Notification& n) {
    QJsonObject o;
    o["id"] = n.getId();
    o["type"] = static_cast<int>(n.getType());
    o["recipientId"] = n.getRecipientId();
    o["relatedBookId"] = n.getRelatedBookId();
    o["message"] = n.getMessage();
    o["createdAt"] = n.getCreatedAt().toSecsSinceEpoch();
    o["isRead"] = n.getIsRead();
    return o;
}

QJsonArray ModelSerializer::serializeNotificationList(const QList<Notification>& notifications) {
    QJsonArray arr;
    for (const auto& n : notifications) {
        arr.append(serializeNotification(n));
    }
    return arr;
}

Notification ModelSerializer::deserializeNotification(const QJsonObject& o) {
    return Notification(
        o["id"].toInt(),
        static_cast<NotificationType>(o["type"].toInt()),
        o["recipientId"].toInt(),
        o["relatedBookId"].toInt(),
        o["message"].toString(),
        QDateTime::fromSecsSinceEpoch(o["createdAt"].toVariant().toLongLong()),
        o["isRead"].toBool()
        );
}

