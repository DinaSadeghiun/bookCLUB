#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include <QObject>
#include <QList>
#include "Notification.h"
#include <QMetaType>

#include "DB/notificationrepository.h"

class NotificationService : public QObject {
    Q_OBJECT
private:
    NotificationRepository* notificationRepo;

public:
    explicit NotificationService(NotificationRepository* repo, QObject* parent = nullptr);

    bool sendNotification(NotificationType type, int recipientId, int relatedBookId, const QString& message);

    QList<Notification> getNotificationsForeRecipient(int recipientId);

    QList<Notification> getUnreadNotificationsFoRecipient(int recipientId);

    bool markAsRead(int notificationId, int recipientId);

    bool markAllAsRead(int recipientId);

    bool deleteNotification(int notificationId, int recipientId);

signals:
    void notificationReceived(int recipientId, const Notification& notification);
    void notificationsUpdated(int recipientId);
};

#endif
