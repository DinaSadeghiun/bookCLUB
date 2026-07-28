#include "readingprogressrepository.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

ReadingProgressRepository::ReadingProgressRepository(DatabaseManager* manager)
    : dbManager(manager) {}

bool ReadingProgressRepository::saveProgress(int userId, int bookId, int page) {
    qDebug() << "=== saveProgress called ===";
    qDebug() << "userId:" << userId << "bookId:" << bookId << "page:" << page;

    if (!dbManager) {
        qDebug() << "ERROR: dbManager is null!";
        return false;
    }

    QSqlDatabase db = dbManager->getDatabase();
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database is not open!";
        return false;
    }

    QSqlQuery q(db);

    q.prepare("UPDATE ReadingProgress SET last_page = :page, updated_at = :timestamp "
              "WHERE user_id = :userId AND book_id = :bookId");
    q.bindValue(":page", page);
    q.bindValue(":timestamp", QDateTime::currentSecsSinceEpoch());
    q.bindValue(":userId", userId);
    q.bindValue(":bookId", bookId);

    if (!q.exec()) {
        qDebug() << "UPDATE failed:" << q.lastError().text();
        return false;
    }

    qDebug() << "UPDATE rows affected:" << q.numRowsAffected();

    if (q.numRowsAffected() == 0) {
        q.prepare("INSERT INTO ReadingProgress (user_id, book_id, last_page, updated_at) "
                  "VALUES (:userId, :bookId, :page, :timestamp)");
        q.bindValue(":userId", userId);
        q.bindValue(":bookId", bookId);
        q.bindValue(":page", page);
        q.bindValue(":timestamp", QDateTime::currentSecsSinceEpoch());

        if (!q.exec()) {
            qDebug() << "INSERT failed:" << q.lastError().text();
            qDebug() << "Query:" << q.lastQuery();
            return false;
        }
        qDebug() << "INSERT successful, rows affected:" << q.numRowsAffected();
    }

    qDebug() << "=== saveProgress SUCCESS ===";
    return true;
}

std::optional<int> ReadingProgressRepository::getProgress(int userId, int bookId) const {
    qDebug() << "=== getProgress called ===";
    qDebug() << "userId:" << userId << "bookId:" << bookId;

    if (!dbManager) {
        qDebug() << "ERROR: dbManager is null!";
        return std::nullopt;
    }

    QSqlDatabase db = dbManager->getDatabase();
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database is not open!";
        return std::nullopt;
    }

    QSqlQuery q(db);
    q.prepare("SELECT last_page FROM ReadingProgress WHERE user_id = :userId AND book_id = :bookId");
    q.bindValue(":userId", userId);
    q.bindValue(":bookId", bookId);

    if (!q.exec()) {
        qDebug() << "SELECT failed:" << q.lastError().text();
        return std::nullopt;
    }

    if (q.next()) {
        int page = q.value(0).toInt();
        qDebug() << "Found page:" << page;
        return page;
    }

    qDebug() << "No record found, returning 0";
    return 0;
}