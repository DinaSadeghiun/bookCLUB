#include "personallibraryservice.h"
#include "DB/personallibraryrepository.h"
#include "DB/bookrepository.h"
#include "personallibrary.h"

PersonalLibraryService::PersonalLibraryService(PersonalLibraryRepository* personalLibRepo,
                                               BookRepository* bookRepo,
                                               QObject* parent)
    : QObject(parent), personalLibRepo(personalLibRepo), bookRepo(bookRepo) {}

QList<int> PersonalLibraryService::getPurchasedBooks(int userId) {
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    return libOpt->getPurchasedBooks();
}

bool PersonalLibraryService::hasPurchased(int userId, int bookId) {
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }
    return libOpt->hasPurchased(bookId);
}

QList<int> PersonalLibraryService::getWishlist(int userId) {
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return QList<int>();
    }
    return libOpt->getWishlist();
}

bool PersonalLibraryService::addToWishlist(int userId, int bookId) {
    if (hasPurchased(userId, bookId)) {
        return false;
    }
    return personalLibRepo->addToWishlist(userId, bookId);
}

bool PersonalLibraryService::removeFromWishlist(int userId, int bookId) {
    return personalLibRepo->removeFromWishlist(userId, bookId);
}

bool PersonalLibraryService::isInWishlist(int userId, int bookId) {
    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }
    return libOpt->isInWishlist(bookId);
}

bool PersonalLibraryService::createShelf(int userId, const QString& shelfName) {
    QString trimmedName = shelfName.trimmed();
    if (trimmedName.isEmpty()) {
        return false;
    }
    return personalLibRepo->addShelf(userId, trimmedName);
}

bool PersonalLibraryService::deleteShelf(int userId, const QString& shelfName) {
    return personalLibRepo->removeShelf(userId, shelfName.trimmed());
}

QList<QString> PersonalLibraryService::getShelfNames(int userId) {
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
    if (!hasPurchased(userId, bookId)) {
        return false;
    }
    return personalLibRepo->addBookToShelf(userId, shelfName.trimmed(), bookId);
}

bool PersonalLibraryService::removeBookFromShelf(int userId, const QString& shelfName, int bookId) {
    return personalLibRepo->removeBookFromShelf(userId, shelfName.trimmed(), bookId);
}

QList<int> PersonalLibraryService::getBooksInShelf(int userId, const QString& shelfName) {
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
    QString trimmedFrom = fromShelf.trimmed();
    QString trimmedTo = toShelf.trimmed();

    if (trimmedFrom == trimmedTo) {
        return true;
    }

    auto libOpt = personalLibRepo->findByUserId(userId);
    if (!libOpt.has_value()) {
        return false;
    }

    PersonalLibrary& lib = *libOpt;

    if (!lib.removeBookFromShelf(trimmedFrom, bookId)) {
        return false;
    }
    if (!lib.addBookToShelf(trimmedTo, bookId)) {
        return false;
    }

    return personalLibRepo->save(lib);
}
