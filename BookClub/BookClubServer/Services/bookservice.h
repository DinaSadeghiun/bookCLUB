#ifndef BOOKSERVICE_H
#define BOOKSERVICE_H

#include <QObject>
#include <QList>
#include <QString>
#include <optional>
#include "Book.h"
#include "genre.h"

class BookRepository;
class DiscountRepository;

class BookService : public QObject {
    Q_OBJECT
private:
    BookRepository* bookRepo;
    DiscountRepository* discountRepo;

    bool validateBook(const Book& book) const;

public:
    explicit BookService(BookRepository* repo, DiscountRepository* discRepo, QObject* parent = nullptr);
    ~BookService() = default;

    bool addBook(Book& book);
    bool updateBook(Book& book);
    bool removeBook(int bookId);

    std::optional<Book> getBookById(int id) const;
    QList<Book> getAllAvailableBooks() const;
    QList<Book> getBooksByPublisher(int publisherId) const;
    QList<Book> getBooksByGenre(Genre genre) const;
    QList<Book> search(const QString& query) const;

    std::optional<double> getBookFinalPrice(int bookId) const;
    bool rateBook(int bookId, double rating);

signals:
    void bookAdded(int bookId, int publisherId);
    void bookUpdated(int bookId);
    void bookRemoved(int bookId);
    void bookRatingUpdated(int bookId, double newAverageRating);
};

#endif