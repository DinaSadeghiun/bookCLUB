#include "commentrepository.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

CommentRepository::CommentRepository(DatabaseManager* manager)
    : dbManager(manager) {}


bool CommentRepository::save(Comment& comment) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    bool isInsert = (comment.getId() == -1);

    if (isInsert) {
        q.prepare("INSERT INTO Comments (user_id, book_id, text, rating, date) "
                  "VALUES (:uid, :bid, :text, :rating, :now)");
        q.bindValue(":now", QDateTime::currentSecsSinceEpoch());
    } else {
        q.prepare("UPDATE Comments SET user_id = :uid, book_id = :bid, "
                  "text = :text, rating = :rating WHERE id = :id");
        q.bindValue(":id", comment.getId());
    }

    q.bindValue(":uid", comment.getUserId());
    q.bindValue(":bid", comment.getBookId());
    q.bindValue(":text", comment.getText());
    q.bindValue(":rating", comment.getRating());

    if (!q.exec()) {
        qDebug() << "Comment save failed:" << q.lastError().text();
        return false;
    }

    if (isInsert) {
        comment.setId(q.lastInsertId().toInt());
    }

    return true;
}

bool CommentRepository::remove(int id) {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("DELETE FROM Comments WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec() && q.numRowsAffected() > 0;
}

std::optional<Comment> CommentRepository::findById(int id) const {
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, user_id, book_id, text, rating, date "
              "FROM Comments WHERE id = :id");
    q.bindValue(":id", id);

    if (q.exec() && q.next()) {
        return Comment(
            q.value("id").toInt(),
            q.value("user_id").toInt(),
            q.value("book_id").toInt(),
            q.value("text").toString(),
            q.value("rating").toInt(),
            QDateTime::fromSecsSinceEpoch(q.value("date").toLongLong())
            );
    }
    return std::nullopt;
}

QList<Comment> CommentRepository::findByBookId(int bookId) const {
    QList<Comment> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, user_id, book_id, text, rating, date "
              "FROM Comments WHERE book_id = :bid");
    q.bindValue(":bid", bookId);

    if (q.exec()) {
        while (q.next()) {
            list.append(Comment(
                q.value("id").toInt(),
                q.value("user_id").toInt(),
                q.value("book_id").toInt(),
                q.value("text").toString(),
                q.value("rating").toInt(),
                QDateTime::fromSecsSinceEpoch(q.value("date").toLongLong())
                ));
        }
    } else {
        qDebug() << "findByBookId error:" << q.lastError().text();
    }
    return list;
}

QList<Comment> CommentRepository::findAll() const {
    QList<Comment> list;
    QSqlDatabase db = dbManager->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT id, user_id, book_id, text, rating, date FROM Comments");

    if (q.exec()) {
        while (q.next()) {
            list.append(Comment(
                q.value("id").toInt(),
                q.value("user_id").toInt(),
                q.value("book_id").toInt(),
                q.value("text").toString(),
                q.value("rating").toInt(),
                QDateTime::fromSecsSinceEpoch(q.value("date").toLongLong())
                ));
        }
    } else {
        qDebug() << "findAll error:" << q.lastError().text();
    }
    return list;
}
