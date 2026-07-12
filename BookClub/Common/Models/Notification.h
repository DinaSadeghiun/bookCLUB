#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>
#include <QDateTime>

enum class NotificationType {
    NewBookInFavoriteGenre, //for user
    DiscountOnWishlistBook, //for user
    NewSaleForBook, //for publisher
    NewReviewForBook // for publisher
};

class Notification {
private:
    int id; // -1 means not persisted yet
    NotificationType type;
    int recipientId; // userId or publisherId
    int relatedBookId;
    QString message;
    QDateTime createdAt;
    bool isRead;

public:
    //creating
    Notification(NotificationType type, int recipientId, int relatedBookId, const QString& message);
    //LOADING from DB
    Notification(int id, NotificationType type, int recipientId, int relatedBookId,
                 const QString& message, const QDateTime& timestamp, bool isRead);

    // Getters
    int getId() const;
    NotificationType getType() const;
    int getRecipientId() const;
    int getRelatedBookId() const;
    QString getMessage() const;
    QDateTime getCreatedAt() const;
    bool getIsRead() const;

    // Actions
    void markAsRead();

    // Setter for Repository
    void setId(int newId);

};

#endif
