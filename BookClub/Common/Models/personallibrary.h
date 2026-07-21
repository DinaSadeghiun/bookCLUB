#ifndef PERSONALLIBRARY_H
#define PERSONALLIBRARY_H

#include <QString>
#include <QList>

class PersonalLibrary {
public:
    class Shelf {
    private:
        QString shelfName;
        QList<int> bookIds;


    public:
        explicit Shelf(const QString &name = "");

        QString getShelfName() const;
        void setShelfName(const QString &name);

        const QList<int>& getBookIds() const;
        void addBook(int bookId);
        bool removeBook(int bookId);

    };

    explicit PersonalLibrary(int userId);
    ~PersonalLibrary();

    //copy constructor
    PersonalLibrary(const PersonalLibrary &other);
    PersonalLibrary& operator=(const PersonalLibrary &other);

    PersonalLibrary(PersonalLibrary &&other) noexcept;
    PersonalLibrary& operator=(PersonalLibrary &&other) noexcept;

    //purchesed
    const QList<int>& getPurchasedBooks() const;
    void addPurchasedBook(int bookId);
    bool hasPurchased(int bookId) const;

    //whish list
    const QList<int>& getWishlist() const;
    void addToWishlist(int bookId);
    bool removeFromWishlist(int bookId);
    bool isInWishlist(int bookId) const;

    //favorite books
    const QList<int>& getFaveBooks() const;
    void addToFaveBooks(int bookId);
    bool removeFromFaveBooks(int bookId);
    bool isInFaveBooks(int bookId) const;


    //custom shelves
    const QList<Shelf*>& getCustomShelves() const;
    Shelf* createShelf(const QString &name);
    bool deleteShelf(const QString &name);
    Shelf* findShelf(const QString &name) const;

    int getUserId() const;
    void setUserId(int newId);

    //add and remove from shelf in the personal library
    bool removeBookFromShelf(const QString &shelfName, int bookId);
    bool addBookToShelf(const QString &shelfName, int bookId);

private:
    int userId;
    QList<int> purchasedBooks;
    QList<int> wishlist;
    QList<int> faveBooks;
    QList<Shelf*> customShelves;


};

#endif
