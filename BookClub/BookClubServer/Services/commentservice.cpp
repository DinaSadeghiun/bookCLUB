#include "commentservice.h"
#include "DB/commentrepository.h"
#include "DB/bookrepository.h"
#include <QDebug>
#include <utility>

CommentService::CommentService(CommentRepository* cRepo,
                               BookRepository* bRepo,
                               QObject* parent)
    : QObject(parent), commentRepo(cRepo), bookRepo(bRepo)
{
    Q_ASSERT(commentRepo != nullptr);
    Q_ASSERT(bRepo != nullptr);
}

double CommentService::getAverageRatingForBook(int bookId) const {
    if (!commentRepo) return 0.0;
    return commentRepo->getAverageRatingForBook(bookId);
}

bool CommentService::addComment(Comment& comment) {
    if (!commentRepo) {
        return false;
    }

    if (comment.getText().trimmed().isEmpty()) {
        qDebug() << "Add Comment Failed: Text cannot be empty.";
        return false;
    }
    if (comment.getUserId() <= 0 || comment.getBookId() <= 0) {
        qDebug() << "Add Comment Failed: Invalid user or book ID.";
        return false;
    }

    if (commentRepo->save(comment)) {
        emit commentAdded(comment.getBookId(), comment.getId());
        updateBookStatistics(comment.getBookId());
        return true;
    }
    return false;
}

bool CommentService::removeComment(int commentId) {
    Q_ASSERT(commentId > 0);
    if (commentId <= 0 || !commentRepo || !bookRepo) return false;

    auto commentOpt = commentRepo->findById(commentId);
    if (!commentOpt.has_value()) {
        return false;
    }

    int bookId = commentOpt.value().getBookId();

    if (commentRepo->remove(commentId)) {
        emit commentRemoved(bookId, commentId);
        updateBookStatistics(bookId);
        return true;
    }
    return false;
}

bool CommentService::editComment(int commentId, const QString& newText, int newRating) {
    auto commentOpt = commentRepo->findById(commentId);
    if (!commentOpt) return false;

    commentOpt->edit(newText, newRating);
    if (commentRepo->save(*commentOpt)) {
        updateBookStatistics(commentOpt->getBookId());
        return true;
    }
    return false;
}

std::optional<Comment> CommentService::getCommentById(int id) const {
    Q_ASSERT(id > 0);
    if (id <= 0 || !commentRepo) return std::nullopt;
    return commentRepo->findById(id);
}

QList<Comment> CommentService::getCommentsByBook(int bookId) const {
    Q_ASSERT(bookId > 0);
    if (bookId <= 0 || !commentRepo) return QList<Comment>();
    return commentRepo->findByBookId(bookId);
}

bool CommentService::updateBookStatistics(int bookId) {
    Q_ASSERT(bookId > 0);
    if (bookId <= 0 || !bookRepo || !commentRepo) return false;

    auto bookOpt = bookRepo->findById(bookId);
    if (!bookOpt.has_value()) return false;

    Book book = bookOpt.value();
    QList<Comment> comments = commentRepo->findByBookId(bookId);

    int count = comments.size();
    double total = 0.0;

    if (count > 0) {
        for (const auto& c : std::as_const(comments)) {
            total += c.getRating();
        }
        book.setTotalRating(total);
        book.setRatingCount(count);
    } else {
        book.setTotalRating(0.0);
        book.setRatingCount(0);
    }

    if (bookRepo->save(book)) {
        double averageRating = count > 0 ? (total / count) : 0.0;
        emit bookRatingUpdated(bookId, averageRating, count);
        return true;
    }
    return false;
}


