#include "publisher.h"

//creat new
Publisher::Publisher(QString name, QString email, QString password, QString companyName)
    : Person(name, email, password), companyName(companyName)
{}

//LOAD from DB
Publisher::Publisher(int id, QString name, QString email, QString password, QString companyName)
    : Person(id, name, email, password), companyName(companyName)
{}


QString Publisher::getRole() const {
    return "Publisher";
}

// Getters
QString Publisher::getCompanyName() const { return companyName; }
QList<int> Publisher::getBookIds() const { return bookIds; }
QList<int> publisher::getDiscountIds() const { return discountIds; }

// Setters
void Publisher::setCompanyName(const QString& name) { this->companyName = name; }
void Publisher::setId(int newId) {
    Person::setId(newId);
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


// Statistics (computed externally, injected here)
SalesStats Publisher::getSalesStats() const { return salesStats; }
void Publisher::setSalesStats(const SalesStats& stats) { this->salesStats = stats; }