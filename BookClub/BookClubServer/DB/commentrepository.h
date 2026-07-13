#ifndef COMMENTREPOSITORY_H
#define COMMENTREPOSITORY_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
#include <optional>
#include "Comment.h"

class DatabaseManager;

class CommentRepository {

    DatabaseManager* dbManager;

public:
    explicit CommentRepository(DatabaseManager* manager);

    bool save(Comment& comment);
    bool remove(int id);
    std::optional<Comment> findById(int id) const;
    QList<Comment> findByBookId(int bookId) const;
    QList<Comment> findAll() const;
};

#endif
