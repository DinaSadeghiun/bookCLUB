#ifndef COMMENTSERVICE_H
#define COMMENTSERVICE_H

#include <QObject>
#include <QList>
#include <QString>
#include <optional>
#include "Comment.h"

class CommentRepository;
class BookRepository;

class CommentService : public QObject {
    Q_OBJECT
private:
    CommentRepository* commentRepo;
    BookRepository* bookRepo;

    bool updateBookStatistics(int bookId);

public:
    explicit CommentService(CommentRepository* cRepo,
                            BookRepository* bRepo,
                            QObject* parent = nullptr);
    ~CommentService() = default;

    double getAverageRatingForBook(int bookId) const;

    bool addComment(Comment& comment);
    bool removeComment(int commentId);
    bool editComment(int commentId, const QString& newText, int newRating);

    std::optional<Comment> getCommentById(int id) const;
    QList<Comment> getCommentsByBook(int bookId) const;

signals:
    void commentAdded(int bookId, int commentId);
    void commentRemoved(int bookId, int commentId);
    void bookRatingUpdated(int bookId, double averageRating, int ratingCount);
};

#endif
