#include "book.h"

//creating new book
Book::Book(int publisherId, const QString& title, const QString& author, double price)
    : id(-1),
    publisherId(publisherId),
    discountId(-1), // No discount initially
    title(title),
    author(author),
    price(price),
    totalRating(0.0),
    averageRating(0.0),
    ratingCount(0),
    salesCount(0),
    isAvailable(true)
{}

//LOADING from DB
Book::Book(int id, int publisherId, const QString& title, const QString& author, double price)
    : id(id),
    publisherId(publisherId),
    discountId(-1), // Will be loaded separately if exists
    title(title),
    author(author),
    price(price),
    totalRating(0.0),
    averageRating(0.0),
    ratingCount(0),
    salesCount(0),
    isAvailable(true)
{}

//helper
void Book::updateAverageRating() {
    averageRating = (ratingCount > 0) ? (totalRating / ratingCount) : 0.0;
}

//getter
int Book::getId() const { return id; }
int Book::getPublisherId() const { return publisherId; }
int Book::getDiscountId() const { return discountId; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getGenre() const { return genre; }
QString Book::getDescription() const { return description; }
QString Book::getCoverImagePath() const { return coverImagePath; }
QString Book::getPdfFilePath() const { return pdfFilePath; }
double Book::getPrice() const { return price; }
int Book::getSalesCount() const { return salesCount; }
int Book::getRatingCount() const { return ratingCount; }
double Book::getAverageRating() const { return averageRating; }
bool Book::getIsAvailable() const { return isAvailable; }

double Book::getDiscountAmount() const { //***
    if (discountId == -1) return 0.0;

    // TODO: Fetch discount percentage from DB using discountId
    double percent = 0.0;
    /*
    Discount discount = DbManager::getInstance().getDiscountById(discountId);
    percent = discount.getPercentage();
    */
    return (price * percent) / 100.0;
}

double Book::getFinalPrice() const {
    return price - getDiscountAmount();
}


//setter
void Book::setId(int id) {
    if (this->id == -1) {
        this->id = id;
    }
}
void Book::setTitle(const QString& t) { title = t; }
void Book::setAuthor(const QString& a) { author = a; }
void Book::setGenre(const QString& g) { genre = g; }
void Book::setDescription(const QString& d) { description = d; }
void Book::setPrice(double p) { price = p; }
void Book::setCoverImagePath(const QString& path) { coverImagePath = path; }
void Book::setPdfFilePath(const QString& path) { pdfFilePath = path; }
void Book::setDiscountId(int dId) { discountId = dId; }
void Book::setAvailable(bool status) { isAvailable = status; }
void Book::removeDiscount() { discountId = -1; }
void Book::setPublisherId(int newId) {
    publisherId = newId;
}

void Book::incrementSalesCount() { salesCount++; }

void Book::addRating(double rating) {
    if (rating >= 1.0 && rating <= 5.0) {
        totalRating += rating;
        ratingCount++;
        updateAverageRating();
    }
}
