#ifndef BOOKSERVICE_H
#define BOOKSERVICE_H

#include <QList>
#include <QString>
#include <optional>
#include "Book.h"
#include "genre.h"

class BookRepository;
class DiscountRepository;

class BookService {
private:
    BookRepository* bookRepo;
    DiscountRepository* discountRepo;

    bool validateBook(const Book& book) const;

public:
    explicit BookService(BookRepository* repo, DiscountRepository* discRepo);
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
};

#endif