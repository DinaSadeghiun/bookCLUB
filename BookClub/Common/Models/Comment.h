#ifndef COMMENT_H
#define COMMENT_H

#include <QString>
#include <QDateTime>

class Comment {
private:
    int id;
    int userId;
    int bookId;
    QString text;
    int rating;
    QDateTime date;

public:
    //creating new comment
    Comment(int userId, int bookId, const QString &text, int rating);
    //LOADING from DB
    Comment(int id, int userId, int bookId, const QString &text, int rating, const QDateTime &date);

    // Getters
    int getId() const;
    int getUserId() const;
    int getBookId() const;
    QString getText() const;
    int getRating() const;
    QDateTime getDate() const;
    QString getFormattedDate() const;

    // Setters
    void setText(const QString &text);
    void setUserId(int id);
    void setBookId(int id);
    void setRating(int rating);
    void setId(int id);

    //edit comment
    void edit(const QString &newText, int newRating);
};

#endif
