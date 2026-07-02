#ifndef PERSONALLIBRARY_H
#define PERSONALLIBRARY_H

#include <QString>
#include <QList>

class PersonalLibrary {
public:
    class Shelf {
    public:
        explicit Shelf(const QString &name = "");

        QString getShelfName() const;
        void setShelfName(const QString &name);

        const QList<int>& getBookIds() const;
        void addBook(int bookId);
        bool removeBook(int bookId);

    private:
        QString shelfName;
        QList<int> bookIds;
    };

    explicit PersonalLibrary(int userId = 0);
    ~PersonalLibrary();

    const QList<int>& getPurchasedBooks() const;
    void addPurchasedBook(int bookId);
    bool hasPurchased(int bookId) const;

    const QList<int>& getWishlist() const;
    void addToWishlist(int bookId);
    bool removeFromWishlist(int bookId);
    bool isInWishlist(int bookId) const;

    const QList<Shelf*>& getCustomShelves() const;
    Shelf* createShelf(const QString &name);
    bool deleteShelf(const QString &name);
    Shelf* findShelf(const QString &name) const;

    int getUserId() const { return userId; }

private:
    int userId;
    QList<int> purchasedBooks;
    QList<int> wishlist;
    QList<Shelf*> customShelves;
};

#endif
