#include "commentservice.h"
#include "DB/commentrepository.h"
#include "DB/bookrepository.h"
#include <QDebug>
#include <utility>

CommentService::CommentService(CommentRepository* cRepo, BookRepository* bRepo)
    : commentRepo(cRepo), bookRepo(bRepo) {}

bool CommentService::addComment(Comment& comment) {
    if (comment.getText().trimmed().isEmpty()) {
        qDebug() << "Add Comment Failed: Text cannot be empty.";
        return false;
    }
    if (comment.getUserId() <= 0 || comment.getBookId() <= 0) {
        qDebug() << "Add Comment Failed: Invalid user or book ID.";
        return false;
    }

    if (!commentRepo->save(comment)) {
        return false;
    }

    return updateBookStatistics(comment.getBookId());
}

bool CommentService::removeComment(int commentId) {
    if (commentId <= 0) return false;

    auto commentOpt = commentRepo->findById(commentId);
    if (!commentOpt.has_value()) {
        return false;
    }

    int bookId = commentOpt.value().getBookId();

    if (!commentRepo->remove(commentId)) {
        return false;
    }

    return updateBookStatistics(bookId);
}

std::optional<Comment> CommentService::getCommentById(int id) const {
    if (id <= 0) return std::nullopt;
    return commentRepo->findById(id);
}

QList<Comment> CommentService::getCommentsByBook(int bookId) const {
    if (bookId <= 0) return QList<Comment>();
    return commentRepo->findByBookId(bookId);
}

bool CommentService::updateBookStatistics(int bookId) {
    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) return false;

    Book book = bookOpt.value();
    QList<Comment> comments = commentRepo->findByBookId(bookId);

    int count = comments.size();

    if (count > 0) {
        double total = 0.0;
        for (const auto& c : std::as_const(comments)) {
            total += c.getRating();
        }
        book.setTotalRating(total);
        book.setRatingCount(count);
    } else {
        book.setTotalRating(0.0);
        book.setRatingCount(0);
    }

    return bookRepo->save(book);
}

