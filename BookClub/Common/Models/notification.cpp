#include "notification.h"

Notification::Notification(NotificationType type, int recipientId, int relatedBookId, const QString& message)
    : id(-1),
    type(type),
    recipientId(recipientId),
    relatedBookId(relatedBookId),
    message(message),
    createdAt(QDateTime::currentDateTime()),
    isRead(false)
{}

// Constructor for LOADING from DB (all fields provided)
Notification::Notification(int id, NotificationType type, int recipientId, int relatedBookId,
                           const QString& message, const QDateTime& createdAt, bool isRead)
    : id(id),
    type(type),
    recipientId(recipientId),
    relatedBookId(relatedBookId),
    message(message),
    createdAt(createdAt),
    isRead(isRead)
{}

//getter
int Notification::getId() const { return id; }
NotificationType Notification::getType() const { return type; }
int Notification::getRecipientId() const { return recipientId; }
int Notification::getRelatedBookId() const { return relatedBookId; }
QString Notification::getMessage() const { return message; }
QDateTime Notification::getCreatedAt() const { return createdAt; }

//methods
bool Notification::readStatus() const { return isRead; }
void Notification::markAsRead() { isRead = true; }

//setter
void Notification::setId(int newId) {
    if (id == -1) {
        id = newId;
    }
}
