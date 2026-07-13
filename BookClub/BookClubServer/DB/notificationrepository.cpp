#include "notificationrepository.h"
#include "databasemanager.h"

NotificationRepository::NotificationRepository(DatabaseManager* manager)
    : dbManager(manager) {}



Notification NotificationRepository::fromQuery(QSqlQuery& q) {
    int id = q.value("id").toInt();
    auto type = static_cast<NotificationType>(q.value("type").toInt());
    int recipientId = q.value("recipient_id").toInt();

    QVariant bookVal = q.value("related_book_id");
    int relatedBookId = bookVal.isNull() ? 0 : bookVal.toInt();

    QString message = q.value("message").toString();
    QDateTime createdAt = QDateTime::fromSecsSinceEpoch(q.value("created_at").toLongLong());
    bool isRead = q.value("is_read").toInt() == 1;

    return Notification(id, type, recipientId, relatedBookId, message, createdAt, isRead);
}

bool NotificationRepository::save(Notification& notification) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO Notifications (type, recipient_id, related_book_id, message, created_at, is_read) "
        "VALUES (?, ?, ?, ?, ?, ?)"
        );

    q.addBindValue(static_cast<int>(notification.getType()));
    q.addBindValue(notification.getRecipientId());

    if (notification.getRelatedBookId() > 0) {
        q.addBindValue(notification.getRelatedBookId());
    } else {
        q.addBindValue(QVariant());
    }


    q.addBindValue(notification.getMessage());
    q.addBindValue(notification.getCreatedAt().toSecsSinceEpoch());
    q.addBindValue(notification.getIsRead() ? 1 : 0);

    if (!q.exec()) {
        qDebug() << "NotificationRepository::save failed:" << q.lastError().text();
        return false;
    }

    notification.setId(q.lastInsertId().toInt());
    return true;
}

std::optional<Notification> NotificationRepository::findById(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare(
        "SELECT id, type, recipient_id, related_book_id, message, created_at, is_read "
        "FROM Notifications WHERE id = ?"
        );
    q.addBindValue(id);

    if (q.exec() && q.next()) {
        return fromQuery(q);
    }
    return std::nullopt;
}

QList<Notification> NotificationRepository::findByRecipientId(int recipientId) {
    QList<Notification> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare(
        "SELECT id, type, recipient_id, related_book_id, message, created_at, is_read "
        "FROM Notifications WHERE recipient_id = ? ORDER BY created_at DESC"
        );
    q.addBindValue(recipientId);

    if (q.exec()) {
        while (q.next()) {
            list.append(fromQuery(q));
        }
    } else {
        qDebug() << "NotificationRepository::findByRecipientId failed:" << q.lastError().text();
    }
    return list;
}

bool NotificationRepository::markAsRead(int notificationId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE Notifications SET is_read = 1 WHERE id = ?");
    q.addBindValue(notificationId);
    return q.exec();
}

bool NotificationRepository::markAllAsReadForUser(int recipientId) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE Notifications SET is_read = 1 WHERE recipient_id = ?");
    q.addBindValue(recipientId);
    return q.exec();
}

bool NotificationRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("DELETE FROM Notifications WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}
