#include "notificationservice.h"
#include <utility>

NotificationService::NotificationService(NotificationRepository* repo, QObject* parent)
    : QObject(parent), notificationRepo(repo)
{
    Q_ASSERT(notificationRepo != nullptr);
}

bool NotificationService::sendNotification(NotificationType type, int recipientId, int relatedBookId, const QString& message) {
    Q_ASSERT(recipientId > 0);
    Q_ASSERT(relatedBookId >= 0);
    if (recipientId <= 0 || !notificationRepo) {
        return false;
    }

    QString trimmedMessage = message.trimmed();
    if (trimmedMessage.isEmpty()) {
        return false;
    }

    Notification notification(type, recipientId, relatedBookId, trimmedMessage);
    if (notificationRepo->save(notification)) {
        emit notificationReceived(recipientId, notification);
        emit notificationsUpdated(recipientId);
        return true;
    }
    return false;
}

QList<Notification> NotificationService::getNotificationsForeRecipient(int recipientId) {
    Q_ASSERT(recipientId > 0);
    if (recipientId <= 0 || !notificationRepo) {
        return {};
    }
    return notificationRepo->findByRecipientId(recipientId);
}

QList<Notification> NotificationService::getUnreadNotificationsFoRecipient(int recipientId) {
    Q_ASSERT(recipientId > 0);
    if (recipientId <= 0 || !notificationRepo) {
        return {};
    }
    QList<Notification> all = notificationRepo->findByRecipientId(recipientId);
    QList<Notification> unread;
    for (const auto& n : std::as_const(all)) {
        if (!n.getIsRead()) {
            unread.append(n);
        }
    }
    return unread;
}

bool NotificationService::markAsRead(int notificationId, int recipientId) {
    Q_ASSERT(notificationId > 0);
    Q_ASSERT(recipientId > 0);
    if (notificationId <= 0 || recipientId <= 0 || !notificationRepo) {
        return false;
    }
    auto notificationOpt = notificationRepo->findById(notificationId);
    if (!notificationOpt.has_value() || notificationOpt->getRecipientId() != recipientId) {
        return false;
    }

    if (notificationRepo->markAsRead(notificationId)) {
        emit notificationsUpdated(recipientId);
        return true;
    }
    return false;
}

bool NotificationService::markAllAsRead(int recipientId) {
    Q_ASSERT(recipientId > 0);
    if (recipientId <= 0 || !notificationRepo) {
        return false;
    }

    if (notificationRepo->markAllAsReadForUser(recipientId)) {
        emit notificationsUpdated(recipientId);
        return true;
    }
    return false;
}

bool NotificationService::deleteNotification(int notificationId, int recipientId) {
    Q_ASSERT(notificationId > 0);
    Q_ASSERT(recipientId > 0);
    if (notificationId <= 0 || recipientId <= 0 || !notificationRepo) {
        return false;
    }
    auto notificationOpt = notificationRepo->findById(notificationId);
    if (!notificationOpt.has_value() || notificationOpt->getRecipientId() != recipientId) {
        return false;
    }

    if (notificationRepo->remove(notificationId)) {
        emit notificationsUpdated(recipientId);
        return true;
    }
    return false;
}
