#include "comment.h"

//creating new comment
Comment::Comment(int userId, int bookId, const QString &text, int rating)
    : id(-1),
    userId(userId),
    bookId(bookId),
    text(""),
    rating(1),
    date(QDateTime::currentDateTime())
{
        setText(text),
        setRating(rating);
}

//LOADING from DB
Comment::Comment(int id, int userId, int bookId, const QString &text, int rating, const QDateTime &date)
    : id(id),
    userId(userId),
    bookId(bookId),
    text(""),
    rating(1),
    date(date)
{
    setText(text);
    setRating(rating);
}

//getters
int Comment::getId() const { return id; }
int Comment::getUserId() const { return userId; }
int Comment::getBookId() const { return bookId; }
QString Comment::getText() const { return text; }
int Comment::getRating() const { return rating; }
QDateTime Comment::getDate() const { return date; }
QString Comment::getFormattedDate() const { return date.toString("yyyy/MM/dd hh:mm"); }

// Setters
void Comment::setText(const QString &text) {
    if (!text.trimmed().isEmpty()) {
        this->text = text;
    }
}

void Comment::setRating(int rating) {
    if (rating < 1) {
        this->rating = 1;
    } else if (rating > 5) {
        this->rating = 5;
    } else {
        this->rating = rating;
    }
}

void Comment::setId(int id) {
    if (id == -1) {
        this->id = id;
    }
}

void Comment::setUserId(int id) {
    this->userId = id;
}

void Comment::setBookId(int id) {
    this->bookId = id;
}

//edit comment
void Comment::edit(const QString &newText, int newRating) {
    setText(newText);
    setRating(newRating);
}