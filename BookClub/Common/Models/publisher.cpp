#include "publisher.h"

Publisher::Publisher()
    : Person(), companyName("") {}

Publisher::Publisher(int id, const QString& username, const QString& password,
                     const QString& email, const QString& companyName)
    : Person(id, username, password, email), companyName(companyName) {}

QString Publisher::getRole() const {
    return "Publisher";
}


void Publisher::addBook(int bookId) {
    if (!bookIds.contains(bookId)) {
        bookIds.append(bookId);
    }
}

void Publisher::removeBook(int bookId) {
    bookIds.removeAll(bookId);
}

bool Publisher::hasBook(int bookId) const {
    return bookIds.contains(bookId);
}

void Publisher::addDiscount(int discountId) {
    if (!discountIds.contains(discountId)) {
        discountIds.append(discountId);
    }
}

void Publisher::removeDiscount(int discountId) {
    discountIds.removeAll(discountId);
}

bool Publisher::hasDiscount(int discountId) const {
    return discountIds.contains(discountId);
}
