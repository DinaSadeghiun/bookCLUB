#ifndef PERSONALLIBRARYSERVICE_H
#define PERSONALLIBRARYSERVICE_H

#include <QObject>
#include <QList>
#include <QString>
#include <optional>

class PersonalLibraryRepository;
class BookRepository;

class PersonalLibraryService : public QObject {
    Q_OBJECT

private:
    PersonalLibraryRepository* personalLibRepo;
    BookRepository* bookRepo;

public:
    explicit PersonalLibraryService(PersonalLibraryRepository* personalLibRepo,
                                    BookRepository* bookRepo = nullptr,
                                    QObject* parent = nullptr);

    // Purchased Books
    QList<int> getPurchasedBooks(int userId);
    bool hasPurchased(int userId, int bookId);

    // Wishlist
    QList<int> getWishlist(int userId);
    bool addToWishlist(int userId, int bookId);
    bool removeFromWishlist(int userId, int bookId);
    bool isInWishlist(int userId, int bookId);

    // Custom Shelves
    bool createShelf(int userId, const QString& shelfName);
    bool deleteShelf(int userId, const QString& shelfName);
    QList<QString> getShelfNames(int userId);

    // Shelf Book Management
    bool addBookToShelf(int userId, const QString& shelfName, int bookId);
    bool removeBookFromShelf(int userId, const QString& shelfName, int bookId);
    QList<int> getBooksInShelf(int userId, const QString& shelfName);

    // Composite Operation: Move Book
    bool moveBookBetweenShelves(int userId, int bookId, const QString& fromShelf, const QString& toShelf);

signals:
    void wishlistUpdated(int userId);
    void shelvesUpdated(int userId);
    void shelfContentUpdated(int userId, const QString& shelfName);

};

#endif
