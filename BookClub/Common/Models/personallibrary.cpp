#include "PersonalLibrary.h"

// Shelf implementation
PersonalLibrary::Shelf::Shelf(const QString &name) : shelfName(name) {}

QString PersonalLibrary::Shelf::getShelfName() const {
    return shelfName;
}

void PersonalLibrary::Shelf::setShelfName(const QString &name) {
    shelfName = name;
}

QList<int> PersonalLibrary::Shelf::getBookIds() const {
    return bookIds;
}

void PersonalLibrary::Shelf::addBook(int bookId) {
    if (!bookIds.contains(bookId)) {
        bookIds.append(bookId);
    }
}

void PersonalLibrary::Shelf::removeBook(int bookId) {
    bookIds.removeAll(bookId);
}

// PersonalLibrary implementation
PersonalLibrary::PersonalLibrary() {}

PersonalLibrary::~PersonalLibrary() {
    qDeleteAll(customShelves);
    customShelves.clear();
}

// Purchased books
QList<int> PersonalLibrary::getPurchasedBooks() const {
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

// Wishlist
QList<int> PersonalLibrary::getWishlist() const {
    return wishlist;
}

void PersonalLibrary::addToWishlist(int bookId) {
    if (!wishlist.contains(bookId)) {
        wishlist.append(bookId);
    }
}

void PersonalLibrary::removeFromWishlist(int bookId) {
    wishlist.removeAll(bookId);
}

bool PersonalLibrary::isInWishlist(int bookId) const {
    return wishlist.contains(bookId);
}

// Custom shelves
QList<PersonalLibrary::Shelf*> PersonalLibrary::getCustomShelves() const {
    return customShelves;
}

PersonalLibrary::Shelf* PersonalLibrary::createShelf(const QString &name) {
    if (name.trimmed().isEmpty()) {
        return nullptr;
    }

    if (findShelf(name) != nullptr) {
        return nullptr;
    }

    Shelf* newShelf = new Shelf(name);
    customShelves.append(newShelf);
    return newShelf;
}


void PersonalLibrary::deleteShelf(const QString &name) {
    for (int i = 0; i < customShelves.size(); ++i) {
        if (customShelves[i]->getShelfName() == name) {
            delete customShelves[i];
            customShelves.removeAt(i);
            return;
        }
    }
}

PersonalLibrary::Shelf* PersonalLibrary::findShelf(const QString &name) const {
    for (Shelf* shelf : customShelves) {
        if (shelf->getShelfName() == name) {
            return shelf;
        }
    }
    return nullptr;
}
