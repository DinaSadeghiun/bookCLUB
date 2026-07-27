#include "orderrepository.h"
#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

OrderRepository::OrderRepository(DatabaseManager* manager)
    : dbManager(manager) {}


bool OrderRepository::save(Order& order) {
    QSqlDatabase db = dbManager->getDatabase();

    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO Orders (user_id, order_date, raw_price, discount_amount, final_price) "
        "VALUES (?, ?, ?, ?, ?)"
        );
    q.addBindValue(order.getUserId());
    q.addBindValue(order.getOrderDate().toSecsSinceEpoch());
    q.addBindValue(order.getRawPrice());
    q.addBindValue(order.getDiscountAmount());
    q.addBindValue(order.getFinalPrice());

    if (!q.exec()) {
        qDebug() << "OrderRepository::save insert failed:" << q.lastError().text();
        return false;
    }

    int orderId = q.lastInsertId().toInt();
    order.setId(orderId);

    for (int bookId : order.getBookIds()) {
        q.prepare("INSERT INTO OrderBooks (order_id, book_id, price) VALUES (?, ?, ?)");
        q.addBindValue(orderId);
        q.addBindValue(bookId);
        q.addBindValue(0.0);

        if (!q.exec()) {
            qDebug() << "OrderRepository::save OrderBooks failed for bookId:" << bookId << "error:" << q.lastError().text();
            return false;
        }
    }

    return true;
}

std::optional<Order> OrderRepository::findById(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, user_id, order_date, raw_price, discount_amount, final_price "
              "FROM Orders WHERE id = ?");
    q.addBindValue(id);

    if (q.exec() && q.next()) {
        Order order = fromQuery(q);
        loadOrderBooks(order);
        return order;
    }
    return std::nullopt;
}

QList<Order> OrderRepository::findByUserId(int userId) {
    QList<Order> orders;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, user_id, order_date, raw_price, discount_amount, final_price "
              "FROM Orders WHERE user_id = ? ORDER BY order_date DESC");
    q.addBindValue(userId);

    if (q.exec()) {
        while (q.next()) {
            Order order = fromQuery(q);
            loadOrderBooks(order);
            orders.append(order);
        }
    }
    return orders;
}


QList<Order> OrderRepository::findAll() {
    QList<Order> orders;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);

    if (q.exec("SELECT id, user_id, order_date, raw_price, discount_amount, final_price "
               "FROM Orders ORDER BY order_date DESC")) {
        while (q.next()) {
            Order order = fromQuery(q);
            loadOrderBooks(order);
            orders.append(order);
        }
    }
    return orders;
}

bool OrderRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    if (!db.transaction()) return false;
    QSqlQuery q(db);

    q.prepare("DELETE FROM Orders WHERE id = ?");
    q.addBindValue(id);

    if (!q.exec()) {
        qDebug() << "OrderRepository::remove failed:" << q.lastError().text();
        db.rollback();
        return false;
    }
    return db.commit();
}

Order OrderRepository::fromQuery(QSqlQuery& q) {
    int id        = q.value("id").toInt();
    int userId    = q.value("user_id").toInt();
    QDateTime date = QDateTime::fromSecsSinceEpoch(q.value("order_date").toLongLong());
    double raw    = q.value("raw_price").toDouble();
    double disc   = q.value("discount_amount").toDouble();
    double finalP = q.value("final_price").toDouble();

    return Order(id, userId, date, raw, disc, finalP, QList<int>());
}

void OrderRepository::loadOrderBooks(Order& order) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM OrderBooks WHERE order_id = ?");
    q.addBindValue(order.getId());

    QList<int> books;
    if (q.exec()) {
        while (q.next())
            books.append(q.value(0).toInt());
    }
    order.setBookIds(books);
}
