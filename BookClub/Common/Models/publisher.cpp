#include "publisher.h"

//creat new
Publisher::Publisher(const QString& name, const QString& password, const QString& sa, const QString& companyName)
    : Person(name, password, sa),
    companyName(companyName)
{}

//LOAD from DB
Publisher::Publisher(int id, const QString& name, const QString& passwordHash,
                     const QDateTime& createdAt, bool isActive, const QString& sa, const QString& companyName)
    : Person(id, name, passwordHash, createdAt, isActive, sa),
    companyName(companyName)
{}


QString Publisher::getRole() const {
    return "Publisher";
}

// Getters
QString Publisher::getCompanyName() const { return companyName; }
double Publisher::getRevenue() const { return revenue; }
//QList<int> Publisher::getBookIds() const { return bookIds; }
//QList<int> Publisher::getDiscountIds() const { return discountIds; }

// Setters
void Publisher::setCompanyName(const QString& name) { this->companyName = name; }
void Publisher::setId(int newId) {
    if (id == -1){
    id = newId;
    }
}
void Publisher::setRevenue(double amount) { revenue = amount; }


/*void Publisher::addBook(int bookId) {
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
*/

// Statistics (computed externally, injected here)
SalesStats Publisher::getSalesStats() const { return salesStats; }
void Publisher::setSalesStats(const SalesStats& stats) { this->salesStats = stats; }

void Publisher::addRevenue(double amount) { revenue += amount; }
