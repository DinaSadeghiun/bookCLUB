#ifndef COMMENTSERVICE_H
#define COMMENTSERVICE_H

#include <QList>
#include <QString>
#include <optional>
#include "Comment.h"

class CommentRepository;
class BookRepository;

class CommentService {
private:
    CommentRepository* commentRepo;
    BookRepository* bookRepo;

    bool updateBookStatistics(int bookId);

public:
    explicit CommentService(CommentRepository* cRepo, BookRepository* bRepo);
    ~CommentService() = default;

    bool addComment(Comment& comment);
    bool removeComment(int commentId);

    std::optional<Comment> getCommentById(int id) const;
    QList<Comment> getCommentsByBook(int bookId) const;
};

#endif
