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

class NotificationRepository {
private:
    QSqlDatabase db;
    Notification fromQuery(QSqlQuery& q);

public:
    NotificationRepository();

    bool save(Notification& notification);
    std::optional<Notification> findById(int id);
    QList<Notification> findByRecipientId(int recipientId);
    bool markAsRead(int notificationId);
    bool markAllAsReadForUser(int recipientId);
    bool remove(int id);
};

#endif
