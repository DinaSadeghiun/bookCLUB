#ifndef NOTIFICATIONREPOSITORY_H
#define NOTIFICATIONREPOSITORY_H

#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>
#include <optional>
#include "Notification.h"
class DatabaseManager;

class NotificationRepository {
private:
    DatabaseManager* dbManager;
    Notification fromQuery(QSqlQuery& q);

public:
    explicit NotificationRepository(DatabaseManager* manager);

    bool save(Notification& notification);
    std::optional<Notification> findById(int id);
    QList<Notification> findByRecipientId(int recipientId);
    bool markAsRead(int notificationId);
    bool markAllAsReadForUser(int recipientId);
    bool remove(int id);
};

#endif
