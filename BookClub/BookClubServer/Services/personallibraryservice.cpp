#include "personallibraryservice.h"
#include "DB/personallibraryrepository.h"
#include "DB/bookrepository.h"
#include "personallibrary.h"

PersonalLibraryService::PersonalLibraryService(PersonalLibraryRepository* personalLibRepo,
                                               BookRepository* bookRepo,
                                               QObject* parent)
    : QObject(parent), personalLibRepo(personalLibRepo), bookRepo(bookRepo)
{
    Q_ASSERT(personalLibRepo != nullptr);
    Q_ASSERT(bookRepo != nullptr);
}

QList<int> PersonalLibraryService::getPurchasedBooks(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return QList<int>();
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    return libOpt->getPurchasedBooks();
}

bool PersonalLibraryService::hasPurchased(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }
    return libOpt->hasPurchased(bookId);
}

// wish list
QList<int> PersonalLibraryService::getWishlist(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return QList<int>();
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    return libOpt->getWishlist();
}

bool PersonalLibraryService::addToWishlist(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    if (hasPurchased(userId, bookId)) {
        return false;
    }
    if (personalLibRepo->addToWishlist(userId, bookId)) {
        emit wishlistUpdated(userId);
        return true;
    }
    return false;
}

bool PersonalLibraryService::removeFromWishlist(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    if (personalLibRepo->removeFromWishlist(userId, bookId)) {
        emit wishlistUpdated(userId);
        return true;
    }
    return false;
}

bool PersonalLibraryService::isInWishlist(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }
    return libOpt->isInWishlist(bookId);
}

// favorite books
QList<int> PersonalLibraryService::getFaveBooks(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return QList<int>();
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    return libOpt->getFaveBooks();
}

bool PersonalLibraryService::addToFaveBooks(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    if (personalLibRepo->addToFaveBooks(userId, bookId)) {
        emit favoritesUpdated(userId);
        return true;
    }
    return false;
}

bool PersonalLibraryService::removeFromFaveBooks(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    if (personalLibRepo->removeFromFaveBooks(userId, bookId)) {
        emit favoritesUpdated(userId);
        return true;
    }
    return false;
}

bool PersonalLibraryService::isInFaveBooks(int userId, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }
    return libOpt->isInFaveBooks(bookId);
}


// shelfs
bool PersonalLibraryService::createShelf(int userId, const QString& shelfName) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return false;
    }
    QString trimmedName = shelfName.trimmed();
    if (trimmedName.isEmpty()) {
        return false;
    }
    if (personalLibRepo->addShelf(userId, trimmedName)) {
        emit shelvesUpdated(userId);
        return true;
    }
    return false;
}

bool PersonalLibraryService::deleteShelf(int userId, const QString& shelfName) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return false;
    }
    QString trimmedName = shelfName.trimmed();
    if (trimmedName.isEmpty()) {
        return false;
    }
    if (personalLibRepo->removeShelf(userId, trimmedName)) {
        emit shelvesUpdated(userId);
        return true;
    }
    return false;
}

QList<QString> PersonalLibraryService::getShelfNames(int userId) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return {};
    }
    QList<QString> names;
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return names;
    }
    for (auto* shelf : libOpt->getCustomShelves()) {
        if (shelf) {
            names.append(shelf->getShelfName());
        }
    }
    return names;
}

bool PersonalLibraryService::addBookToShelf(int userId, const QString& shelfName, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    if (!hasPurchased(userId, bookId)) {
        return false;
    }
    QString trimmedName = shelfName.trimmed();
    if (personalLibRepo->addBookToShelf(userId, trimmedName, bookId)) {
        emit shelfContentUpdated(userId, trimmedName);
        return true;
    }
    return false;
}

bool PersonalLibraryService::removeBookFromShelf(int userId, const QString& shelfName, int bookId) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    QString trimmedName = shelfName.trimmed();
    if (personalLibRepo->removeBookFromShelf(userId, trimmedName, bookId)) {
        emit shelfContentUpdated(userId, trimmedName);
        return true;
    }
    return false;
}


QList<int> PersonalLibraryService::getBooksInShelf(int userId, const QString& shelfName) {
    Q_ASSERT(userId > 0);
    if (userId <= 0 || !personalLibRepo) {
        return QList<int>();
    }
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    auto* shelf = libOpt->findShelf(shelfName.trimmed());
    if (!shelf) {
        return QList<int>();
    }
    return shelf->getBookIds();
}

bool PersonalLibraryService::moveBookBetweenShelves(int userId, int bookId,
                                                    const QString& fromShelf,
                                                    const QString& toShelf) {
    Q_ASSERT(userId > 0);
    Q_ASSERT(bookId > 0);
    if (userId <= 0 || bookId <= 0 || !personalLibRepo) {
        return false;
    }
    QString trimmedFrom = fromShelf.trimmed();
    QString trimmedTo   = toShelf.trimmed();

    if (trimmedFrom == trimmedTo) return true;

    // verify book is in source shelf
    QList<int> books = getBooksInShelf(userId, trimmedFrom);
    if (!books.contains(bookId)) return false;

    // verify target shelf exists
    QList<QString> shelves = getShelfNames(userId);
    if (!shelves.contains(trimmedTo)) return false;

    if (!personalLibRepo->removeBookFromShelf(userId, trimmedFrom, bookId))
        return false;

    if (!personalLibRepo->addBookToShelf(userId, trimmedTo, bookId)) {
        // rollback
        personalLibRepo->addBookToShelf(userId, trimmedFrom, bookId);
        return false;
    }

    emit shelfContentUpdated(userId, trimmedFrom);
    emit shelfContentUpdated(userId, trimmedTo);
    return true;
}

bool PersonalLibraryService::renameShelf(int userId, const QString& oldShelfName, const QString& newShelfName) {
    QString trimmedOld = oldShelfName.trimmed();
    QString trimmedNew = newShelfName.trimmed();

    if (trimmedOld.isEmpty() || trimmedNew.isEmpty() || trimmedOld == trimmedNew) {
        return false;
    }
    return personalLibRepo->renameShelf(userId, trimmedOld, trimmedNew);
}
