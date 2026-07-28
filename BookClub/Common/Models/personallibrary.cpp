#include "personallibrary.h"

//shelf
PersonalLibrary::Shelf::Shelf(const QString &name)
    : shelfName(name) {}

QString PersonalLibrary::Shelf::getShelfName() const { return shelfName; }

void PersonalLibrary::Shelf::setShelfName(const QString &name) { shelfName = name; }

const QList<int>& PersonalLibrary::Shelf::getBookIds() const { return bookIds; }

void PersonalLibrary::Shelf::addBook(int bookId) {
    if (!bookIds.contains(bookId)) {
        bookIds.append(bookId);
    }
}

bool PersonalLibrary::Shelf::removeBook(int bookId) {
    return bookIds.removeOne(bookId);
}

//personal library
PersonalLibrary::PersonalLibrary(int userId)
    : userId(userId) {}

PersonalLibrary::~PersonalLibrary() {
    qDeleteAll(customShelves);
    customShelves.clear();
}


const QList<int>& PersonalLibrary::getPurchasedBooks() const {
    return purchasedBooks;
}

void PersonalLibrary::addPurchasedBook(int bookId) {
    if (!purchasedBooks.contains(bookId)) {
        purchasedBooks.append(bookId);
    }
}

bool PersonalLibrary::hasPurchased(int bookId) const {
    return purchasedBooks.contains(bookId);
}

//wish list
const QList<int>& PersonalLibrary::getWishlist() const {
    return wishlist;
}

void PersonalLibrary::addToWishlist(int bookId) {
    if (!wishlist.contains(bookId)) {
        wishlist.append(bookId);
    }
}

bool PersonalLibrary::removeFromWishlist(int bookId) {
    return wishlist.removeOne(bookId);
}

bool PersonalLibrary::isInWishlist(int bookId) const {
    return wishlist.contains(bookId);
}

//favorite books
const QList<int>& PersonalLibrary::getFaveBooks() const {
    return faveBooks;
}

void PersonalLibrary::addToFaveBooks(int bookId) {
    if (!faveBooks.contains(bookId)) {
        faveBooks.append(bookId);
    }
}

bool PersonalLibrary::removeFromFaveBooks(int bookId) {
    return faveBooks.removeOne(bookId);
}

bool PersonalLibrary::isInFaveBooks(int bookId) const {
    return faveBooks.contains(bookId);
}

//costum shelves
const QList<PersonalLibrary::Shelf*>& PersonalLibrary::getCustomShelves() const
{
    return customShelves;
}

PersonalLibrary::Shelf* PersonalLibrary::createShelf(const QString &name) {
    if (findShelf(name) != nullptr) {
        return nullptr;
    }

    Shelf* newShelf = new Shelf(name);
    customShelves.append(newShelf);
    return newShelf;
}

bool PersonalLibrary::deleteShelf(const QString &name) {
    for (int i = 0; i < customShelves.size(); ++i) {
        if (customShelves[i]->getShelfName() == name) {
            delete customShelves[i];
            customShelves.removeAt(i);
            return true;
        }
    }
    return false;
}

PersonalLibrary::Shelf* PersonalLibrary::findShelf(const QString &name) const {
    for (Shelf* shelf : customShelves) {
        if (shelf->getShelfName() == name) {
            return shelf;
        }
    }
    return nullptr;
}

bool PersonalLibrary::addBookToShelf(const QString &shelfName, int bookId) {
    if (!hasPurchased(bookId)) {
        return false;
    }

    Shelf* shelf = findShelf(shelfName);
    if (shelf != nullptr) {
        shelf->addBook(bookId);
        return true;
    }
    return false;
}

bool PersonalLibrary::removeBookFromShelf(const QString &shelfName, int bookId) {
    Shelf* shelf = findShelf(shelfName);
    if (shelf != nullptr) {
        return shelf->removeBook(bookId);
    }
    return false;
}


int PersonalLibrary::getUserId() const { return userId;}
void PersonalLibrary::setUserId(int newId) {
    userId = newId;
}


// Copy Constructor
PersonalLibrary::PersonalLibrary(const PersonalLibrary &other)
    : userId(other.userId), purchasedBooks(other.purchasedBooks), wishlist(other.wishlist), faveBooks(other.faveBooks) {
    for (const Shelf* shelf : other.customShelves) {
        if (shelf) {
            Shelf* newShelf = new Shelf(shelf->getShelfName());
            for (int bookId : shelf->getBookIds()) {
                newShelf->addBook(bookId);
            }
            this->customShelves.append(newShelf);
        }
    }
}

// Copy Assignment
PersonalLibrary& PersonalLibrary::operator=(const PersonalLibrary &other) {
    if (this != &other) {
        qDeleteAll(customShelves);
        customShelves.clear();

        userId = other.userId;
        purchasedBooks = other.purchasedBooks;
        wishlist = other.wishlist;
        faveBooks = other.faveBooks;

        for (const Shelf* shelf : other.customShelves) {
            if (shelf) {
                Shelf* newShelf = new Shelf(shelf->getShelfName());
                for (int bookId : shelf->getBookIds()) {
                    newShelf->addBook(bookId);
                }
                this->customShelves.append(newShelf);
            }
        }
    }
    return *this;
}

// Move Constructor
PersonalLibrary::PersonalLibrary(PersonalLibrary &&other) noexcept
    : userId(other.userId),
    purchasedBooks(std::move(other.purchasedBooks)),
    wishlist(std::move(other.wishlist)),
    faveBooks(std::move(other.faveBooks)),
    customShelves(std::move(other.customShelves))
{
    other.userId = 0;
    other.customShelves.clear();
}

// Move Assignment
PersonalLibrary& PersonalLibrary::operator=(PersonalLibrary &&other) noexcept {
    if (this != &other) {
        qDeleteAll(customShelves);
        customShelves.clear();

        userId = other.userId;
        purchasedBooks = std::move(other.purchasedBooks);
        wishlist = std::move(other.wishlist);
        faveBooks = std::move(other.faveBooks);
        customShelves = std::move(other.customShelves);

        other.userId = 0;
        other.customShelves.clear();
    }
    return *this;
}


