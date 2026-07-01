#ifndef PERSONALLIBRARY_H
#define PERSONALLIBRARY_H

#include <QString>
#include <QList>

class PersonalLibrary {
public:
    // Nested class: Shelf
    class Shelf {
    public:
        Shelf(const QString &name = "");

        QString getShelfName() const;
        void setShelfName(const QString &name);

        QList<int> getBookIds() const;
        void addBook(int bookId);
        void removeBook(int bookId);

    private:
        QString shelfName;
        QList<int> bookIds;
    };

    PersonalLibrary();
    ~PersonalLibrary();

    // Purchased books
    QList<int> getPurchasedBooks() const;
    void addPurchasedBook(int bookId);
    bool hasPurchased(int bookId) const;

    // Wishlist
    QList<int> getWishlist() const;
    void addToWishlist(int bookId);
    void removeFromWishlist(int bookId);
    bool isInWishlist(int bookId) const;

    // Custom shelves
    QList<Shelf*> getCustomShelves() const;
    Shelf* createShelf(const QString &name);
    void deleteShelf(const QString &name);
    Shelf* findShelf(const QString &name) const;

private:
    int UserID;
    QList<int> purchasedBooks;
    QList<int> wishlist;
    QList<Shelf*> customShelves;
};

#endif
