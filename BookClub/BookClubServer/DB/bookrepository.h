#ifndef BOOKREPOSITORY_H
#define BOOKREPOSITORY_H

#include <QList>
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <optional>
#include "Book.h"

class DatabaseManager;

class BookRepository {
private:
    DatabaseManager* dbManager;

    Book fromQuery(QSqlQuery& q) const;

public:
    explicit BookRepository(DatabaseManager* manager);

    bool save(Book& book);
    std::optional<Book> findById(int id);
    QList<Book> findAll();
    QList<Book> findByPublisherId(int publisherId);
    QList<Book> findByGenre(Genre genre);
    bool remove(int id); // Soft delete: is_available = 0
};

#endif
