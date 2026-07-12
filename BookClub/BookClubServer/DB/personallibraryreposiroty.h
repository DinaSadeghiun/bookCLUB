#ifndef PERSONALLIBRARYREPOSITORY_H
#define PERSONALLIBRARYREPOSITORY_H

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <optional>
#include <QPair>
#include "personallibrary.h"

class QSqlQuery;

class PersonalLibraryRepository {
public:
    explicit PersonalLibraryRepository();

    std::optional<PersonalLibrary> findByUserId(int userId);
    bool save(const PersonalLibrary& lib);

    bool addPurchasedBook(int userId, int bookId);
    bool addToWishlist(int userId, int bookId);
    bool removeFromWishlist(int userId, int bookId);

    bool addShelf(int userId, const QString& shelfName);
    bool removeShelf(int userId, const QString& shelfName);
    bool addBookToShelf(int userId, const QString& shelfName, int bookId);
    bool removeBookFromShelf(int userId, const QString& shelfName, int bookId);

private:
    QSqlDatabase db;

    QList<int> loadPurchasedBooks(int userId) const;
    QList<int> loadWishlist(int userId) const;
    QList<QPair<QString, QList<int>>> loadShelves(int userId) const;
    int resolveShelfId(int userId, const QString& name) const;
};

#endif
