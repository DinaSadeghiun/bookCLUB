#ifndef READINGPROGRESSREPOSITORY_H
#define READINGPROGRESSREPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <optional>

class DatabaseManager;

class ReadingProgressRepository {
public:
    explicit ReadingProgressRepository(DatabaseManager* manager);

    bool saveProgress(int userId, int bookId, int page);
    std::optional<int> getProgress(int userId, int bookId) const;

private:
    DatabaseManager* dbManager;
};

#endif